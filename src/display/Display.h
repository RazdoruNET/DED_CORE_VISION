#ifndef DISPLAY_DISPLAY_H
#define DISPLAY_DISPLAY_H

#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// Единственный объект отрисовки. Canvas 480x272: все функции приборки и игры
// рисуют в буфер и затем вызывают gfx->flush().
extern Arduino_GFX *gfx;

namespace display {

// Инициализация QSPI-панели NV3041A и Canvas. Подсветка выключена.
void begin();

// Яркость подсветки, 0…100 %.
void setBacklight(uint8_t percent);

// Заливка белым и цветные полосы — проверка панели из портала.
void testPattern();

}  // namespace display

#endif // DISPLAY_DISPLAY_H
