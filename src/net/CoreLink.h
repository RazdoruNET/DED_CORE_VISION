#ifndef NET_CORE_LINK_H
#define NET_CORE_LINK_H

#include <Arduino.h>
#include <WebSocketsClient.h>

// Телеметрия ядра в объёме, который нужен приборке.
struct CoreTelemetry {
  bool valid = false;

  int card = -1;        // active_cart, 0…2
  int selectCart = 0;   // select_cart, 1…3
  int rpm = 0;
  int maxRpm = 0;
  int type = 0;         // 0/1/2 → 7/12/22 точки
  int throttle = 0;
  int initAngle = 0;
  int advanceAngle = 0;
  int micros = 0;

  int servo = 0;
  int servoPosition = 0;
  int servoType = 0;
  int servoRpm = 0;
  int servoPercent = 0;

  int points = 0;
  float pointRpm[22] = {0};
  float pointAngle[22] = {0};

  unsigned long frames = 0;
  unsigned long lastFrameMs = 0;
};

// WebSocket-клиент к ядру.
//
// Контракт V4: кадр {"event":"message","data":{…}} рассылается всем клиентам,
// команды принимаются тем же сокетом. Модуль подтверждает токен кадром
// module_hello и получает смену параметров сети кадром module_credentials.
class CoreLink {
public:
  static CoreLink &instance();

  void begin(const String &host, uint16_t port);
  void loop();

  // Адрес или порт ядра изменились: сокет закрывается, main.cpp поднимет его
  // заново по сохранённым настройкам.
  void rebind();

  bool started() const { return started_; }
  bool connected();
  const CoreTelemetry &telemetry() const { return telemetry_; }

  // Новые параметры сети, присланные ядром. true — ровно один раз на кадр.
  bool takeCredentials(String &ssid, String &password, String &host, uint16_t &port);

  // true, если последний кадр пришёл не позже timeoutMs назад.
  bool frameFresh(uint32_t timeoutMs) const;

  // Команда переключения карты, cart — 1…3 как в интерфейсе ядра.
  void sendCart(int cart);

private:
  CoreLink() = default;

  static void onEvent(WStype_t type, uint8_t *payload, size_t length);
  void handleEvent(WStype_t type, uint8_t *payload, size_t length);
  void handleFrame(const char *payload, size_t length);
  void sendHello();
  void sendText(String &frame, bool logPayload = true);

  WebSocketsClient socket_;
  CoreTelemetry telemetry_;

  String host_;
  uint16_t port_ = 0;

  bool started_ = false;
  bool connected_ = false;
  bool authorized_ = false;

  bool hasPendingCredentials_ = false;
  String pendingSsid_;
  String pendingPassword_;
  String pendingHost_;
  uint16_t pendingPort_ = 0;
};

#endif // NET_CORE_LINK_H
