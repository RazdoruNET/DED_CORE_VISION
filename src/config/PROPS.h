#ifndef CONFIG_PROPS_H
#define CONFIG_PROPS_H

#include <Arduino.h>

// Версия прошивки модуля: уходит в портал и в кадр module_hello.
#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "1.0.0"
#endif

// Имя точки доступа портала настройки. Портал поднимается, когда модуль не
// настроен или потерял ядро.
constexpr const char *PORTAL_AP_SSID = "DED_CORE_VISION";

// Ядро поднимает сеть сопряжения с этим префиксом: "DED_PAIR_" + хвост MAC.
constexpr const char *PAIR_AP_PREFIX = "DED_PAIR_";

// Адрес ядра в его собственной сети и порт WebSocket.
constexpr const char *CORE_DEFAULT_HOST = "192.168.4.1";
constexpr uint16_t CORE_DEFAULT_PORT = 81;

// NVS-пространства: настройки связи и пользовательские настройки экрана
// разведены, чтобы два Preferences-хэндла не писали в один namespace.
constexpr const char *NVS_NAMESPACE_NET = "net";
constexpr const char *NVS_NAMESPACE_UI = "vision";

#endif // CONFIG_PROPS_H
