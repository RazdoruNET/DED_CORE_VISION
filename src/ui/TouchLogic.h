#ifndef UI_TOUCH_LOGIC_H
#define UI_TOUCH_LOGIC_H

#include <Arduino.h>

// Один проход обработки тача. Вызывается из задачи тача каждые TOUCH_TICK_MS.
void TouchHandler();

#endif // UI_TOUCH_LOGIC_H
