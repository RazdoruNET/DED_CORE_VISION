#include "app/Tasks.h"

#include "app/State.h"
#include "config/CONFIG.h"
#include "sensors/Sensors.h"
#include "ui/TouchLogic.h"

namespace {

// Моточасы копятся в миллисекундах и не влезают в int (2^31 мс ≈ 596 ч):
// в RAM счётчик беззнаковый, а в NVS пишем с насыщением, чтобы старое
// значение не превратилось в отрицательное.
void saveEngineHours() {
  const int32_t stored =
      (mh > static_cast<uint32_t>(INT32_MAX)) ? INT32_MAX : static_cast<int32_t>(mh);

  preferences.putInt("mh_h", stored);
}

// Сглаживание оборотов и моточасы — перенос ServerHandler заводской прошивки.
void rpmHandler() {
  int rpm_step = 100;

  if (rpm > 3000) {
    rpm_step = 200;
  } else if (rpm > 1000) {
    rpm_step = 100;
  } else if (rpm > 500) {
    rpm_step = 50;
  } else {
    rpm_step = 25;
  }

  if (rpm_new > rpm) {
    rpm += (rpm_new - rpm >= rpm_step) ? rpm_step : (rpm_new - rpm);
  } else if (rpm_new < rpm) {
    rpm -= (rpm - rpm_new >= rpm_step) ? rpm_step : (rpm - rpm_new);
  }

  if (start_engin == 0) {
    if (rpm > 0) {
      start_engin = micros() / 1000;
    }
  }

  // Моточасы копим в RAM и пишем в NVS раз в 5 минут и на остановке.
  // Заводская прошивка писала каждую секунду — это ~86 тыс. циклов в сутки.
  static uint32_t lastSaveMs = 0;
  static bool mhDirty = false;
  static bool wasRunning = false;

  if (rpm > 0) {
    const uint32_t nmh = micros() / 1000 - start_engin;

    if (nmh > 1000) {
      mh += nmh;
      mhDirty = true;
      start_engin = 0;

      if (lastSaveMs == 0 || static_cast<uint32_t>(millis() - lastSaveMs) >= 300000) {
        saveEngineHours();
        lastSaveMs = millis();
        mhDirty = false;
      }
    }

    wasRunning = true;
  } else {
    // Двигатель заглох — сохраняем накопленное сразу.
    if (wasRunning && mhDirty) {
      saveEngineHours();
      mhDirty = false;
    }

    wasRunning = false;
    start_engin = 0;
  }
}

}  // namespace

void TaskTouchCode(void *parameter) {
  (void)parameter;

  Serial.printf("TASK: тач на ядре %d\n", xPortGetCoreID());

  for (;;) {
    TouchHandler();
    vTaskDelay(pdMS_TO_TICKS(TOUCH_TICK_MS));
  }
}

void TaskKppCode(void *parameter) {
  (void)parameter;

  for (;;) {
    kppHandler();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void TaskSpeedCode(void *parameter) {
  (void)parameter;

  for (;;) {
    speedMeterHandler();
    vTaskDelay(1);
  }
}

void TaskRpmCode(void *parameter) {
  (void)parameter;

  for (;;) {
    rpmHandler();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// Плавное изменение цифры скорости: рывки датчика не должны дёргать индикатор.
void TaskDrawPropCode(void *parameter) {
  (void)parameter;

  for (;;) {
    const int gap = speed - speed_draw;

    if (gap != 0) {
      // Не больше 10 км/ч за тик: рывки датчика не дёргают цифру, но цель
      // всегда достигается. Раньше шаг был перевёрнут (у цели +5/+10/+20,
      // издалека +1) и цифра застревала на ~9 км/ч ниже фактической скорости.
      const int step = (gap > 10) ? 10 : (gap < -10) ? -10 : gap;

      speed_draw += step;

      if (speed_draw < 0) {
        speed_draw = 0;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
