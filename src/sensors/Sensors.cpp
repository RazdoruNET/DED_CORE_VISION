#include "sensors/Sensors.h"

#include <driver/gpio.h>

#include "app/State.h"
#include "app/Time.h"
#include "config/CONFIG.h"

void sensorsBegin() {
  pinMode(t_7, OUTPUT);
  digitalWrite(t_7, LOW);

  const int inputs[] = {t_1, t_2, t_3, t_4, t_5, t_6, t_9};

  for (int i = 0; i < 7; ++i) {
    pinMode(inputs[i], INPUT);
    gpio_set_pull_mode(static_cast<gpio_num_t>(inputs[i]), GPIO_PULLUP_ONLY);
  }

  pinMode(t_8, INPUT);
  gpio_set_pull_mode(static_cast<gpio_num_t>(t_8), GPIO_PULLDOWN_ONLY);
}

// Перенос KPPHandler заводской прошивки: пять опросов по 2 мс, включённой
// считается передача, набравшая больше sensetive_qshifter подтверждений.
void kppHandler() {
  int kpp_arr[] = {0, 0, 0, 0, 0, 0, 0};

  for (int i = 0; i < 5; ++i) {
    kpp_arr[0] += (digitalRead(t_1) == LOW) ? 1 : -1;
    kpp_arr[1] += (digitalRead(t_2) == LOW) ? 1 : -1;
    kpp_arr[2] += (digitalRead(t_3) == LOW) ? 1 : -1;
    kpp_arr[3] += (digitalRead(t_4) == LOW) ? 1 : -1;
    kpp_arr[4] += (digitalRead(t_5) == LOW) ? 1 : -1;
    kpp_arr[5] += (digitalRead(t_6) == LOW) ? 1 : -1;
    kpp_arr[6] += (digitalRead(t_9) == LOW) ? 1 : -1;
    vTaskDelay(pdMS_TO_TICKS(2));
  }

  int kpp_index_max = -1;

  for (int i = 0; i < 7; ++i) {
    if (kpp_arr[i] > sensetive_qshifter) {
      kpp_index_max = i;
      break;
    }
  }

  kpp = kpp_index_max;

  if (init_kpp == false) {
    last_kpp = kpp;
    init_kpp = true;
    quiqshifter = 0;
    digitalWrite(t_7, LOW);
    return;
  }

  if (last_kpp > -1 && kpp == -1 && quiqshifter == 0) {
    if (active_quiqshifter) {
      quiqshifter = micros() + QS_CUT_US;
      last_kpp = -1;
      digitalWrite(t_7, HIGH);
      use_quiqshifter = true;
    }
  } else if (quiqshifter != 0 && timeReached(quiqshifter)) {
    quiqshifter = 0;

    if (kpp > -1) {
      last_kpp = kpp;
    }

    digitalWrite(t_7, LOW);
    use_quiqshifter = false;
  } else {
    if (kpp > -1) {
      last_kpp = kpp;
    }
  }
}

// Перенос SpeedMeterHandler: импульс геркона/Холла, одометр и скорость.
void speedMeterHandler() {
  const int senser = digitalRead(t_8);

  if (senser == HIGH && last_speed_sense != senser) {
    // Фронт фиксируем до фильтра: иначе отброшенный дребезг обрабатывался
    // повторно и удлинял измеряемый интервал.
    last_speed_sense = HIGH;

    const uint32_t diff_pic = micros() - speed_meter_time;

    if (diff_pic > 10000) {
      const float rpm_sp = 3600000000.0f / static_cast<float>(diff_pic);

      odometr += dlinna * 1000;

      // Одометр в метрах. Пишем в NVS раз в километр, а не на каждый импульс:
      // заводская прошивка делала запись каждые ~1,5 м и убивала флеш.
      static int savedOdometer = -1;

      if (savedOdometer < 0) {
        savedOdometer = preferences.getInt("odometr", 0);
      }

      if (odometr - savedOdometer >= 1000000 || odometr < savedOdometer) {
        preferences.putInt("odometr", odometr);
        savedOdometer = odometr;
      }

      speed = rpm_sp * dlinna;
      speed_meter_time = micros();
    }
  } else {
    last_speed_sense = senser;
  }

  if ((micros() - speed_meter_time) > 500000) {
    speed = 0;
    speed_draw = 0;
  }
}
