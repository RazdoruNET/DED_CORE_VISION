#ifndef UI_DRAW_H
#define UI_DRAW_H

#include <Arduino.h>

// Портал заводской прошивки: приветствие, стартовый экран, приборка, настройки.
void drawWellcomeScreen();
void drawInitialScreen();
void drawConnectScreen();
void drawDashboard();
void drawOptions();
void drawWarning(String text);

// Экран с параметрами точки доступа портала настройки.
void drawPortalInfo(const String &ssid, const String &pass, const String &ip);

// Игра. gameAlert показывает всплывающий текст поверх дороги.
void gameAlert(String txt, int x);
void game();

#endif // UI_DRAW_H
