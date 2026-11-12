#include "app/Tasks.h"

#include "app/State.h"
#include "config/CONFIG.h"
#include "sensors/Sensors.h"
#include "ui/TouchLogic.h"

namespace {

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

  // Моточасы копим в RAM и пишем в NVS не чаще раза в минуту. Заводская
  // прошивка писала каждую секунду — это ~86 тыс. циклов в сутки и убитый NVS.
  static uint32_t lastSaveMs = 0;
  static bool mhDirty = false;
  static bool wasRunning = false;

  if (rpm > 0) {
    const int nmh = (micros() / 1000) - start_engin;

    if (nmh > 1000) {
      mh += nmh;
      mhDirty = true;
      start_engin = 0;

      if (lastSaveMs == 0 || static_cast<uint32_t>(millis() - lastSaveMs) >= 60000) {
        preferences.putInt("mh_h", mh);
        lastSaveMs = millis();
        mhDirty = false;
      }
    }

    wasRunning = true;
  } else {
    // Двигатель заглох — сохраняем накопленное сразу.
    if (wasRunning && mhDirty) {
      preferences.putInt("mh_h", mh);
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
    if (speed > speed_draw && speed_draw < 200) {
      if (speed - speed_draw < 5) {
        speed_draw += 5;
      } else if (speed - speed_draw < 10) {
        speed_draw += 10;
      } else if (speed - speed_draw < 20) {
        speed_draw += 20;
      } else {
        speed_draw++;
      }
    }

    if (speed < speed_draw && speed_draw > 0) {
      speed_draw -= 10;

      if (speed_draw < 0) {
        speed_draw = 0;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
