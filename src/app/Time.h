#ifndef APP_TIME_H
#define APP_TIME_H

#include <Arduino.h>

// Сравнение дедлайна micros() без переполнения: int32_t разности корректен,
// пока интервал меньше ~35 минут. Заводская прошивка хранила такие метки в int
// и через 35 минут молча теряла тач и залипала реле квикшифтера.
inline bool timeReached(uint32_t deadline) {
  // 0 — дедлайн ещё не назначался; иначе после 35 минут аптайма «микросекунды
  // без дедлайна» стали бы отрицательными и тач перестал бы отвечать.
  if (deadline == 0) {
    return true;
  }

  return static_cast<int32_t>(micros() - deadline) >= 0;
}

#endif // APP_TIME_H
