#ifndef UI_GAME_H
#define UI_GAME_H

#include <Arduino.h>

// Предмет на дороге. lane: 0 — левая, 1 — средняя, 2 — правая.
// z: 1 — горизонт, ~6.5 — линия машины, больше — ушёл за экран.
struct GameItem {
  uint8_t lane = 0;
  uint8_t type = 0;
  float z = 1.0f;
  bool hit = false;
};

enum GameItemType : uint8_t {
  ITEM_CONE = 0,
  ITEM_BARREL,
  ITEM_CAR,
  ITEM_BLOCK,
  ITEM_PUDDLE,
  ITEM_NITRO,
  ITEM_COIN,
};

namespace game {

// Экран приглашения и старт заезда.
void attract();
void begin();

// Выход на приборку.
void exit();

// Физика. Вызывается из loop() с реальным dt в секундах.
void update(float dt);

// Тач: кнопки полос 1/2/3, выход, старт/рестарт.
void handleTouch(int16_t x, int16_t y);

// Свайп: -1 влево, +1 вправо.
void handleSwipe(int8_t direction);

// Полный кадр игры: дорога и машина из заводского game(), предметы, HUD.
void drawFrame();

// Вызываются из game() в ui/Draw.cpp, чтобы предметы легли под машину.
void drawItems();
void drawHud();

}  // namespace game

#endif // UI_GAME_H
