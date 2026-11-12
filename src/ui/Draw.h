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

// Игра. gameAlert показывает всплывающий текст поверх дороги.
void gameAlert(String txt, int x);
void game();

#endif // UI_DRAW_H
