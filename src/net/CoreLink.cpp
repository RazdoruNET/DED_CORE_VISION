#include "net/CoreLink.h"

#include <ArduinoJson.h>
#include <WiFi.h>

#include "app/Status.h"
#include "config/CONFIG.h"
#include "config/PROPS.h"
#include "net/Settings.h"

namespace {

CoreLink *g_coreLink = nullptr;

// Кадр ядра с 22 точками карты занимает около 1,5 КБ текста. Буферы
// статические: разбор кадра не должен жить на стеке задачи.
char g_frameBuffer[2560];
StaticJsonBuffer<6144> g_jsonBuffer;

constexpr int kMaxPoints = 22;

}  // namespace

CoreLink &CoreLink::instance() {
  static CoreLink link;
  return link;
}

void CoreLink::begin(const String &host, uint16_t port) {
  host_ = host;
  port_ = port;
  started_ = true;
  g_coreLink = this;

  socket_.begin(host.c_str(), port, "/");
  socket_.onEvent(&CoreLink::onEvent);
  socket_.setReconnectInterval(WS_RECONNECT_MS);

  // Без heartbeat разрыв по радио остаётся незамеченным: TCP висит открытым,
  // кадры просто перестают приходить, а приборка показывает устаревшие данные.
  socket_.enableHeartbeat(WS_PING_INTERVAL_MS, WS_PONG_TIMEOUT_MS, WS_PONG_MISSES);

  Serial.printf("CORE: WebSocket %s:%u\n", host_.c_str(), static_cast<unsigned>(port_));
}

void CoreLink::loop() {
  socket_.loop();
}

void CoreLink::rebind() {
  if (!started_) {
    return;
  }

  socket_.disconnect();
  started_ = false;
}

bool CoreLink::connected() {
  return socket_.isConnected();
}

bool CoreLink::frameFresh(uint32_t timeoutMs) const {
  return telemetry_.valid &&
         static_cast<uint32_t>(millis() - telemetry_.lastFrameMs) <= timeoutMs;
}

void CoreLink::sendCart(int cart) {
  if (!socket_.isConnected()) {
    return;
  }

  String frame = "{\"event\":\"cart\",\"data\":{\"cart\":";
  frame += String(cart);
  frame += "}}";

  sendText(frame);
}

void CoreLink::sendHello() {
  String frame = "{\"event\":\"module_hello\",\"data\":{\"mac\":\"";
  frame += WiFi.macAddress();
  frame += "\",\"fw\":\"";
  frame += FIRMWARE_VERSION;
  frame += "\",\"token\":\"";
  frame += Settings::instance().token();
  frame += "\"}}";

  // Кадр содержит токен: в лог его целиком не печатаем.
  sendText(frame, false);
}

bool CoreLink::takeCredentials(String &ssid, String &password, String &host, uint16_t &port) {
  if (!hasPendingCredentials_) {
    return false;
  }

  hasPendingCredentials_ = false;

  ssid = pendingSsid_;
  password = pendingPassword_;
  host = pendingHost_;
  port = pendingPort_;

  return true;
}

void CoreLink::sendText(String &frame, bool logPayload) {
  socket_.sendTXT(frame);

  if (logPayload) {
    Serial.printf("CORE > %s\n", frame.c_str());
  } else {
    Serial.println("CORE > module_hello (токен скрыт)");
  }
}

void CoreLink::onEvent(WStype_t type, uint8_t *payload, size_t length) {
  if (g_coreLink != nullptr) {
    g_coreLink->handleEvent(type, payload, length);
  }
}

void CoreLink::handleEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      connected_ = true;
      authorized_ = false;

      Serial.println("CORE: WebSocket подключён");

      // Токен подтверждаем сразу: без него ядро не пришлёт смену сети.
      sendHello();
      break;

    case WStype_DISCONNECTED:
      if (connected_) {
        Serial.println("CORE: WebSocket отключён");
      }
      connected_ = false;
      break;

    case WStype_TEXT:
      handleFrame(reinterpret_cast<const char *>(payload), length);
      break;

    default:
      break;
  }
}

void CoreLink::handleFrame(const char *payload, size_t length) {
  if (payload == nullptr || length == 0 || length >= sizeof(g_frameBuffer)) {
    return;
  }

  // payload не завершён нулём: без копии разбор читал бы память за концом кадра.
  memcpy(g_frameBuffer, payload, length);
  g_frameBuffer[length] = '\0';

  g_jsonBuffer.clear();
  JsonObject &root = g_jsonBuffer.parseObject(g_frameBuffer);

  if (!root.success()) {
    return;
  }

  const char *event = root["event"];

  if (event == nullptr) {
    return;
  }

  JsonObject &data = root["data"];

  if (strcmp(event, "module_hello") == 0) {
    authorized_ = (data["trusted"] | 0) != 0;

    Serial.println(authorized_ ? "CORE: ядро подтвердило модуль"
                               : "CORE: ядро не знает токен модуля");

    return;
  }

  if (strcmp(event, "module_credentials") == 0) {
    const char *ssid = data["ssid"];
    const char *password = data["pass"];
    const char *host = data["host"];

    if (ssid == nullptr || password == nullptr) {
      return;
    }

    pendingSsid_ = ssid;
    pendingPassword_ = password;
    pendingHost_ = host != nullptr ? String(host) : host_;
    pendingPort_ = static_cast<uint16_t>(data["port"] | CORE_DEFAULT_PORT);
    hasPendingCredentials_ = true;

    Serial.println("CORE: ядро прислало новые параметры сети");

    return;
  }

  if (strcmp(event, "message") != 0 || !data.success()) {
    return;
  }

  telemetry_.card = data["card"] | -1;
  telemetry_.selectCart = data["select_cart"] | 0;
  telemetry_.rpm = data["rpm"] | 0;
  telemetry_.maxRpm = data["max_rpm"] | 0;
  telemetry_.type = data["type"] | 0;
  telemetry_.throttle = data["throttle"] | 0;
  telemetry_.initAngle = data["init_angle"] | 0;
  telemetry_.advanceAngle = data["aa"] | 0;
  telemetry_.micros = static_cast<int>(data["micros"] | 0);

  telemetry_.servo = data["servo"] | 0;
  telemetry_.servoPosition = data["servo_position"] | 0;
  telemetry_.servoType = data["servo_type"] | 0;
  telemetry_.servoRpm = data["servo_rpm"] | 0;
  telemetry_.servoPercent = data["servo_percent"] | 0;

  JsonArray &points = data["angles"];
  int count = points.size();

  if (count > kMaxPoints) {
    count = kMaxPoints;
  }

  for (int i = 0; i < count; ++i) {
    JsonArray &item = points[i];

    telemetry_.pointRpm[i] = item[0].as<float>();
    telemetry_.pointAngle[i] = item[1].as<float>();
  }

  telemetry_.points = count;

  ++telemetry_.frames;
  telemetry_.lastFrameMs = millis();
  telemetry_.valid = true;

  runtime.frames = telemetry_.frames;
  runtime.lastFrameMs = telemetry_.lastFrameMs;
  runtime.rpm = telemetry_.rpm;
  runtime.card = telemetry_.card;
}
