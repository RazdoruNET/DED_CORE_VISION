#ifndef SENSORS_SENSORS_H
#define SENSORS_SENSORS_H

#include <Arduino.h>

// Пины КПП, датчика скорости и реле квикшифтера — как в заводской прошивке.
void sensorsBegin();

// Опрос передач с антидребезгом и логика квикшифтера.
void kppHandler();

// Счёт импульсов датчика скорости, одометр и скорость.
void speedMeterHandler();

#endif // SENSORS_SENSORS_H
