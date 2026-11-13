#ifndef NET_SETUP_PORTAL_H
#define NET_SETUP_PORTAL_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// Портал настройки модуля.
//
// Поднимается, когда модуль ещё не знает ядро или потерял его: точка доступа
// DED_CORE_VISION с паролем из NVS, страница с состоянием, ручной ввод
// параметров ядра, запуск сопряжения, проверка дисплея и обновление прошивки.
// Пароль точки доступа и адрес показываются на дисплее при открытии портала.
// В рабочем режиме портала нет.
class SetupPortal {
public:
  static SetupPortal &instance();

  // manual=true — портал открыт кнопкой из OPTIONS и не закрывается сам,
  // когда связь с ядром появилась.
  void begin(bool manual = false);
  void loop();
  void stop();

  bool active() const { return active_; }
  bool manual() const { return manual_; }

  void requestPairing();
  bool takePairingRequest();

  // Проверка дисплея: маршрут только выставляет заявку, рисует loop().
  bool takeDisplayTestRequest();

  // true — первые секунды после открытия портала: экран показывает имя сети,
  // пароль точки доступа и адрес страницы.
  bool showInfo();

  void setPairingStatus(const String &text);

private:
  SetupPortal() = default;

  void registerRoutes();
  String buildPage() const;
  String buildStatusJson() const;

  // Маршруты настроек не отвечают, когда портал закрыт: сервер остаётся
  // слушать и после остановки точки доступа. Исключение — /update: его
  // регистрирует ElegantOTA, он защищён паролем и доступен из сети ядра.
  bool allowRequest(AsyncWebServerRequest *request) const;

  AsyncWebServer server_{80};

  bool started_ = false;
  bool active_ = false;
  bool manual_ = false;

  bool pairingRequested_ = false;
  String pairingStatus_;

  bool displayTestRequested_ = false;
  unsigned long infoUntil_ = 0;

  unsigned long restartAt_ = 0;
};

#endif // NET_SETUP_PORTAL_H
