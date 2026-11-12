#ifndef APP_TASKS_H
#define APP_TASKS_H

#include <Arduino.h>

// Задачи FreeRTOS. Тач и плавность оборотов — на ядре 1 вместе с loop(),
// датчики — на ядре 0 рядом с WiFi-стеком, как в заводской прошивке.
void TaskTouchCode(void *parameter);
void TaskKppCode(void *parameter);
void TaskSpeedCode(void *parameter);
void TaskRpmCode(void *parameter);
void TaskDrawPropCode(void *parameter);

#endif // APP_TASKS_H
