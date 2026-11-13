#ifndef CONFIG_CONFIG_H
#define CONFIG_CONFIG_H

#include <Arduino.h>

// --- Дисплей ----------------------------------------------------------------
constexpr int SCREEN_WIDTH  = 480;
constexpr int SCREEN_HEIGHT = 272;

// Период опроса тача. 50 Гц хватает, чтобы нажатия не слипались.
constexpr uint32_t TOUCH_TICK_MS = 20;

// Приборка перерисовывается не чаще 10 раз в секунду: полный кадр Canvas
// занимает десятки миллисекунд, а быстрее значения всё равно не меняются.
constexpr uint32_t DASHBOARD_REDRAW_MS = 100;

// Игра рисуется каждый кадр, но не чаще 30 Гц.
constexpr uint32_t GAME_FRAME_MS = 33;

// --- Связь с ядром ----------------------------------------------------------
constexpr uint32_t LINK_TIMEOUT_MS = 3000;
constexpr uint32_t WS_RECONNECT_MS = 1000;
constexpr uint32_t WS_PING_INTERVAL_MS = 15000;
constexpr uint32_t WS_PONG_TIMEOUT_MS = 3000;
constexpr uint8_t  WS_PONG_MISSES = 2;

// --- Сопряжение -------------------------------------------------------------
constexpr uint32_t WIFI_RETRY_MS = 20000;
constexpr uint32_t PAIR_SCAN_TIMEOUT_MS = 25000;
constexpr uint32_t PAIR_SCAN_INTERVAL_MS = 20000;
constexpr uint32_t PAIR_AFTER_LINK_LOSS_MS = 60000;

// --- Портал настройки -------------------------------------------------------
constexpr uint8_t PORTAL_AFTER_FAILED_BOOTS = 10;

// Сколько секунд после открытия портала экран показывает имя сети, пароль
// и адрес страницы.
constexpr uint32_t PORTAL_INFO_MS = 10000;

// --- КПП и квикшифтер -------------------------------------------------------
// Сколько из пяти опросов должна подтвердиться передача, чтобы считаться
// включённой. Как в заводской прошивке.
constexpr int KPP_SENSITIVE = 3;

// Длительность отсечки зажигания квикшифтером.
constexpr uint32_t QS_CUT_US = 150000;

#endif // CONFIG_CONFIG_H
