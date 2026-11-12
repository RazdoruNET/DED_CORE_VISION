#include <Arduino.h>
#include <WiFi.h>

#include "app/State.h"
#include "app/Status.h"
#include "app/Tasks.h"
#include "config/CONFIG.h"
#include "config/PROPS.h"
#include "display/Display.h"
#include "display/Touch.h"
#include "net/CoreLink.h"
#include "net/PairingLink.h"
#include "net/Settings.h"
#include "net/SetupPortal.h"
#include "net/WifiLink.h"
#include "sensors/Sensors.h"
#include "ui/Draw.h"
#include "ui/Game.h"

RuntimeStatus runtime;

namespace {

unsigned long g_pairAttemptAt = 0;
unsigned long g_wifiDownSince = 0;
bool g_everOnline = false;

unsigned long g_lastDrawMs = 0;
unsigned long g_lastFrameMs = 0;

void loadUiSettings() {
  odometr = preferences.getInt("odometr", 0);
  mh = preferences.getInt("mh_h", 0);

  toggle_right = preferences.getBool("toggle_right", false);
  toggle_left = preferences.getBool("toggle_left", false);

  optins_var[1] = preferences.getInt("wheel", 19);

  dlinna = 3.14f * (static_cast<float>(optins_var[1]) / 39370.0f);

  optins_var[2] = preferences.getInt("light", 90);

  // В заводской прошивке флаг "black" инвертирует тему: COLOR_BLACK хранит
  // цвет фона. Поведение сохранено.
  const bool black = preferences.getBool("black", true);
  COLOR_BLACK = black ? WHITE : BLACK;
  COLOR_WHITE = black ? BLACK : WHITE;

  lock_screen = preferences.getBool("lock_screen", false);
  game_best = preferences.getInt("game_best", 0);
  runtime.gameBest = game_best;
}

void runPairing(bool explicitRequest) {
  CoreCredentials credentials;
  Settings &settings = Settings::instance();

  const String expectedBssid = explicitRequest ? String() : settings.bssid();

  itin_step = 0;
  drawInitialScreen();

  SetupPortal::instance().setPairingStatus("Ищу сеть сопряжения ядра (DED_PAIR_*)…");

  if (!PairingLink::instance().pair(credentials, expectedBssid)) {
    SetupPortal::instance().setPairingStatus(
        "Сеть сопряжения не найдена. Откройте в приложении «Модули» → «Найти модуль» и повторите.");
    return;
  }

  settings.saveCore(credentials.ssid, credentials.password, credentials.host, credentials.port);
  settings.saveToken(credentials.token);
  settings.saveBssid(credentials.bssid);

  runtime.coreProvisioned = true;
  runtime.coreSsid = credentials.ssid;
  runtime.coreHost = credentials.host;

  SetupPortal::instance().setPairingStatus("Ядро найдено: " + credentials.ssid + ". Перезагрузка…");

  itin_step = 3;
  drawInitialScreen();

  delay(1500);
  ESP.restart();
}

void schedulePairing() {
  Settings &settings = Settings::instance();
  const unsigned long now = millis();

  if (WifiLink::instance().isConnected()) {
    g_wifiDownSince = now;
    return;
  }

  if (g_pairAttemptAt != 0 &&
      static_cast<uint32_t>(now - g_pairAttemptAt) < PAIR_SCAN_INTERVAL_MS) {
    return;
  }

  const bool unprovisioned = !settings.provisioned();
  const bool linkLostLongEnough =
      settings.provisioned() &&
      static_cast<uint32_t>(now - g_wifiDownSince) >= PAIR_AFTER_LINK_LOSS_MS;

  if (!unprovisioned && !linkLostLongEnough) {
    return;
  }

  // Настроенный модуль без BSSID не знает, какое ядро искать: новое ядро
  // выбирают из портала.
  if (!unprovisioned && settings.bssid().length() == 0) {
    return;
  }

  g_pairAttemptAt = now;
  runPairing(false);
}

void applyCoreCredentials() {
  String ssid;
  String password;
  String host;
  uint16_t port = 0;

  if (!CoreLink::instance().takeCredentials(ssid, password, host, port)) {
    return;
  }

  Settings::instance().saveCore(ssid, password, host, port);

  runtime.coreSsid = ssid;
  runtime.coreHost = host;
  runtime.coreProvisioned = true;

  WifiLink::instance().applyCredentials(ssid, password);
  CoreLink::instance().rebind();
}

void updateLinkState() {
  const bool fresh = CoreLink::instance().frameFresh(LINK_TIMEOUT_MS);

  if (fresh) {
    if (!g_everOnline) {
      Serial.println("CORE: телеметрия пошла");
      Settings::instance().setFailedBoots(0);

      // Портал, открытый вручную из OPTIONS, не закрываем: владелец сам
      // нажал «Портал настройки».
      if (SetupPortal::instance().active() && !SetupPortal::instance().manual()) {
        SetupPortal::instance().stop();
      }
    }

    g_everOnline = true;
    coreOnline = true;
    coreStale = false;
  } else {
    if (coreOnline) {
      Serial.println("CORE: кадров нет");
    }

    coreOnline = false;
    coreStale = g_everOnline;
  }

  runtime.linkOnline = coreOnline;
}

void applyTelemetry() {
  CoreLink &core = CoreLink::instance();

  if (!core.frameFresh(LINK_TIMEOUT_MS)) {
    runtime.gear = kpp;
    runtime.screen = screen_index;
    return;
  }

  const CoreTelemetry &t = core.telemetry();

  rpm_new = t.rpm;
  max_rpm = t.maxRpm;
  cart_type = t.type;
  init_angle = t.initAngle;
  power = t.throttle;
  micro = t.micros;

  if (t.card >= 0 && t.card <= 2) {
    cart = t.card;
  }

  if (t.points > 0) {
    for (int i = 0; i < t.points && i < 22; ++i) {
      rpms[i] = t.pointRpm[i];
      angles[i] = t.pointAngle[i];
    }

    max_cart_rpm = static_cast<int>(t.pointRpm[t.points - 1]);
  }

  runtime.card = cart;
  runtime.rpm = rpm_new;
  runtime.gear = kpp;
  runtime.screen = screen_index;
}

void drawScreen() {
  const unsigned long now = millis();

  runtime.screen = screen_index;

  if (displayTestUntil != 0) {
    if (static_cast<int32_t>(now - displayTestUntil) < 0) {
      return;
    }

    displayTestUntil = 0;
    g_lastDrawMs = 0;
  }

  if (screen_index == SCREEN_GAME) {
    const float dt = (g_lastFrameMs == 0)
                         ? 0.0f
                         : static_cast<float>(now - g_lastFrameMs) / 1000.0f;

    g_lastFrameMs = now;
    gameui::update(dt > 0.1f ? 0.1f : dt);

    if (static_cast<uint32_t>(now - g_lastDrawMs) >= GAME_FRAME_MS) {
      g_lastDrawMs = now;
      gameui::drawFrame();
    }

    return;
  }

  g_lastFrameMs = 0;

  const uint32_t period =
      (screen_index == SCREEN_OPTIONS) ? 200 : DASHBOARD_REDRAW_MS;

  if (g_lastDrawMs != 0 && static_cast<uint32_t>(now - g_lastDrawMs) < period) {
    return;
  }

  g_lastDrawMs = now;

  if (screen_index == SCREEN_OPTIONS) {
    drawOptions();
  } else {
    drawDashboard();
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(100);

  // До первого WiFi.mode(): режим хранения WiFi-конфига фиксируется при
  // инициализации стека.
  WiFi.persistent(false);

  // Модуль стоит рядом с двигателем: лишний нагрев ни к чему.
  setCpuFrequencyMhz(160);

  Serial.printf("\nDED CORE VISION %s\n", FIRMWARE_VERSION);

  preferences.begin(NVS_NAMESPACE_UI, false);
  loadUiSettings();

  display::begin();
  display::setBacklight(static_cast<uint8_t>(optins_var[2]));

  drawWellcomeScreen();

  touch_init(SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  sensorsBegin();

  Settings &settings = Settings::instance();
  settings.begin();

  // Счётчик неудачных стартов: модуль с неверным паролем ядра иначе остался бы
  // без сети и без портала настройки.
  const uint8_t failedBoots = settings.failedBoots();
  settings.setFailedBoots(static_cast<uint8_t>(failedBoots + 1));

  runtime.coreProvisioned = settings.provisioned();
  runtime.coreSsid = settings.ssid();
  runtime.coreHost = settings.host();

  if (!settings.provisioned() || failedBoots >= PORTAL_AFTER_FAILED_BOOTS) {
    Serial.println("SETUP: открыт портал настройки");
    SetupPortal::instance().begin();
  }

  if (settings.provisioned()) {
    WifiLink::instance().begin(settings.ssid(), settings.password());
  }

  xTaskCreatePinnedToCore(TaskTouchCode, "Touch", 8000, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(TaskKppCode, "Kpp", 3000, nullptr, 1, nullptr, 0);
  xTaskCreatePinnedToCore(TaskSpeedCode, "Speed", 2000, nullptr, 1, nullptr, 0);
  xTaskCreatePinnedToCore(TaskRpmCode, "Rpm", 2048, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(TaskDrawPropCode, "DrawProp", 1024, nullptr, 1, nullptr, 0);

  g_wifiDownSince = millis();
}

void loop() {
  WifiLink::instance().loop();
  SetupPortal::instance().loop();

  if (SetupPortal::instance().takePairingRequest()) {
    runPairing(true);
  }

  CoreLink &core = CoreLink::instance();

  if (WifiLink::instance().isConnected() && !core.started()) {
    core.begin(Settings::instance().host(), Settings::instance().port());
  }

  core.loop();

  applyCoreCredentials();
  updateLinkState();
  applyTelemetry();
  schedulePairing();
  drawScreen();

  delay(1);
}
