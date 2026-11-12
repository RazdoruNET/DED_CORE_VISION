#ifndef APP_STATUS_H
#define APP_STATUS_H

#include <Arduino.h>

// Состояние модуля для портала настройки. Пишут сетевой слой, датчики и UI,
// читает портал.
struct RuntimeStatus {
  bool linkOnline = false;
  bool coreProvisioned = false;

  String coreSsid;
  String coreHost;

  // Активная карта из кадра ядра: 0…2, -1 — данных ещё не было.
  int card = -1;
  int rpm = 0;
  int gear = -1;
  int screen = 0;
  int gameBest = 0;

  unsigned long frames = 0;
  unsigned long lastFrameMs = 0;
};

extern RuntimeStatus runtime;

#endif // APP_STATUS_H
