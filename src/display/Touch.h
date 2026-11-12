#ifndef DISPLAY_TOUCH_H
#define DISPLAY_TOUCH_H

#include <Arduino.h>

// Тач GT911 на I2C. Координаты приводятся к системе дисплея с учётом поворота.
void touch_init(int16_t w, int16_t h, uint8_t rotation);

bool touch_has_signal();
bool touch_touched();
bool touch_released();

extern int16_t touch_last_x;
extern int16_t touch_last_y;

#endif // DISPLAY_TOUCH_H
