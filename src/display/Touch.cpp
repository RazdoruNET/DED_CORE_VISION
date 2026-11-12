#include "display/Touch.h"

#include <TouchLib.h>
#include <Wire.h>

#include "config/PINS.h"

namespace {

// GT911 на адресе 0x5D — как на плате JC4827W543.
TouchLib touch(Wire, PIN_TOUCH_SDA, PIN_TOUCH_SCL, GT911_SLAVE_ADDRESS1);

bool touch_swap_xy = false;
int16_t touch_map_x1 = -1;
int16_t touch_map_x2 = -1;
int16_t touch_map_y1 = -1;
int16_t touch_map_y2 = -1;

int16_t touch_max_x = 0;
int16_t touch_max_y = 0;
int16_t touch_raw_x = 0;
int16_t touch_raw_y = 0;

}  // namespace

int16_t touch_last_x = 0;
int16_t touch_last_y = 0;

void touch_init(int16_t w, int16_t h, uint8_t rotation) {
  touch_max_x = w - 1;
  touch_max_y = h - 1;

  // Калибровка по повороту экрана — как в заводской прошивке.
  switch (rotation) {
    case 3:
      touch_swap_xy = true;
      touch_map_x1 = touch_max_x;
      touch_map_x2 = 0;
      touch_map_y1 = 0;
      touch_map_y2 = touch_max_y;
      break;
    case 2:
      touch_swap_xy = false;
      touch_map_x1 = touch_max_x;
      touch_map_x2 = 0;
      touch_map_y1 = touch_max_y;
      touch_map_y2 = 0;
      break;
    case 1:
      touch_swap_xy = true;
      touch_map_x1 = 0;
      touch_map_x2 = touch_max_x;
      touch_map_y1 = touch_max_y;
      touch_map_y2 = 0;
      break;
    default:
      touch_swap_xy = false;
      touch_map_x1 = 0;
      touch_map_x2 = touch_max_x;
      touch_map_y1 = 0;
      touch_map_y2 = touch_max_y;
      break;
  }

  pinMode(PIN_TOUCH_RES, OUTPUT);
  digitalWrite(PIN_TOUCH_RES, LOW);
  delay(200);
  digitalWrite(PIN_TOUCH_RES, HIGH);
  delay(200);

  Wire.begin(PIN_TOUCH_SDA, PIN_TOUCH_SCL);
  touch.init();

  Serial.println("TOUCH: GT911 инициализирован");
}

bool touch_has_signal() {
  return true;
}

namespace {

void translate_touch_raw() {
  if (touch_swap_xy) {
    touch_last_x = map(touch_raw_y, touch_map_x1, touch_map_x2, 0, touch_max_x);
    touch_last_y = map(touch_raw_x, touch_map_y1, touch_map_y2, 0, touch_max_y);
  } else {
    touch_last_x = map(touch_raw_x, touch_map_x1, touch_map_x2, 0, touch_max_x);
    touch_last_y = map(touch_raw_y, touch_map_y1, touch_map_y2, 0, touch_max_y);
  }
}

}  // namespace

bool touch_touched() {
  if (!touch.read()) {
    return false;
  }

  const TP_Point point = touch.getPoint(0);
  touch_raw_x = point.x;
  touch_raw_y = point.y;

  translate_touch_raw();
  return true;
}

bool touch_released() {
  return false;
}
