#ifndef CONFIG_PINS_H
#define CONFIG_PINS_H

#include <Arduino.h>

// Распиновка платы JC4827W543. Не меняется: прошивка ставится на готовое
// неразборное железо.

// --- Дисплей NV3041A, 4-битный QSPI -----------------------------------------
constexpr int8_t PIN_GFX_CS  = 45;
constexpr int8_t PIN_GFX_SCK = 47;
constexpr int8_t PIN_GFX_D0  = 21;
constexpr int8_t PIN_GFX_D1  = 48;
constexpr int8_t PIN_GFX_D2  = 40;
constexpr int8_t PIN_GFX_D3  = 39;
constexpr uint8_t PIN_GFX_BL = 1;

// --- Тач GT911, I2C ---------------------------------------------------------
constexpr uint8_t PIN_TOUCH_SDA = 8;
constexpr uint8_t PIN_TOUCH_SCL = 4;
constexpr uint8_t PIN_TOUCH_RES = 38;
constexpr uint8_t PIN_TOUCH_INT = 3;

// --- Датчики КПП, скорости и квикшифтер -------------------------------------
// Номер в имени — передача на корпусе; в массиве KPP индекс 0 — нейтраль.
constexpr uint8_t PIN_GEAR_N  = 18;  // нейтраль
constexpr uint8_t PIN_GEAR_1  = 17;
constexpr uint8_t PIN_GEAR_2  = 16;
constexpr uint8_t PIN_GEAR_3  = 14;
constexpr uint8_t PIN_GEAR_4  = 5;
constexpr uint8_t PIN_GEAR_5  = 15;
constexpr uint8_t PIN_GEAR_6  = 6;

constexpr uint8_t PIN_QS_RELAY = 9;  // выход отсечки зажигания
constexpr uint8_t PIN_SPEED    = 7;  // датчик скорости (геркон/Холл)

#endif // CONFIG_PINS_H
