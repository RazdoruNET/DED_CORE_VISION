#include "ui/Game.h"

#include <U8g2lib.h>
#include <esp_random.h>

#include "app/State.h"
#include "app/Status.h"
#include "config/CONFIG.h"
#include "display/Display.h"
#include "ui/Draw.h"

namespace {

constexpr int kMaxItems = 24;

GameItem items[kMaxItems];
int itemCount = 0;

// После входа в игру палец ещё лежит на кнопке OPTIONS: короткая пауза,
// чтобы этот же тап не запустил заезд.
uint32_t s_startGuardUntil = 0;

// Позиции полос игрока совпадают с mot_x заводской game(): 60 / 190 / 320.
constexpr int kLaneX[3] = {60, 190, 320};

// Геометрия заводской дороги: горизонт на y=135, размер растёт как z^2.5.
struct Projection {
  int x;
  int y;
  float scale;
};

Projection project(uint8_t lane, float z) {
  const float scale = powf(z, 2.5f);
  const int y = 135 + static_cast<int>(1.2f * scale);
  float t = (y - 135) / 135.0f;

  if (t < 0.0f) {
    t = 0.0f;
  }

  Projection p;
  p.x = 240 + static_cast<int>((kLaneX[lane] - 240) * t);
  p.y = y;
  p.scale = scale;
  return p;
}

void addItem(uint8_t lane, uint8_t type) {
  if (itemCount >= kMaxItems) {
    return;
  }

  items[itemCount].lane = lane;
  items[itemCount].type = type;
  items[itemCount].z = 1.0f;
  items[itemCount].hit = false;
  ++itemCount;
}

void removeItem(int index) {
  for (int i = index; i < itemCount - 1; ++i) {
    items[i] = items[i + 1];
  }

  --itemCount;
}

void clearItems() {
  itemCount = 0;
}

uint8_t randomObstacle() {
  const uint32_t r = esp_random() % 100;

  if (r < 22) return ITEM_CONE;
  if (r < 42) return ITEM_BARREL;
  if (r < 62) return ITEM_CAR;
  if (r < 78) return ITEM_BLOCK;
  return ITEM_PUDDLE;
}

void spawnWave() {
  // Одна полоса всегда остаётся свободной: иначе заезд непроходим.
  const uint8_t freeLane = static_cast<uint8_t>(esp_random() % 3);

  uint8_t candidates[2];
  int candidateCount = 0;

  for (uint8_t lane = 0; lane < 3; ++lane) {
    if (lane != freeLane) {
      candidates[candidateCount++] = lane;
    }
  }

  const bool doubleWave = (esp_random() % 100) < (15 + static_cast<int>(game_time * 2.0f));
  const int obstacles = doubleWave ? 2 : 1;

  for (int i = 0; i < obstacles && candidateCount > 0; ++i) {
    const int pick = static_cast<int>(esp_random() % candidateCount);
    addItem(candidates[pick], randomObstacle());

    for (int j = pick; j < candidateCount - 1; ++j) {
      candidates[j] = candidates[j + 1];
    }
    --candidateCount;
  }

  if ((esp_random() % 100) < 45) {
    addItem(static_cast<uint8_t>(esp_random() % 3),
            (esp_random() % 100) < 35 ? ITEM_NITRO : ITEM_COIN);
  }
}

bool isBonus(uint8_t type) {
  return type == ITEM_NITRO || type == ITEM_COIN;
}

void drawItem(const GameItem &item) {
  const Projection p = project(item.lane, item.z);

  if (p.y < 120 || p.y > SCREEN_HEIGHT + 40) {
    return;
  }

  const int w = static_cast<int>(p.scale * 0.62f);
  const int h = static_cast<int>(p.scale * 0.74f);

  if (w < 2 || h < 2) {
    return;
  }

  const int left = p.x - w / 2;
  const int top = p.y - h;

  switch (item.type) {
    case ITEM_CONE:
      gfx->fillTriangle(p.x, top, p.x + w / 2, p.y, p.x - w / 2, p.y, 0xFD20);
      gfx->fillRect(p.x - w / 5, top + h / 2, w * 2 / 5, h / 7, WHITE);
      break;

    case ITEM_BARREL:
      gfx->fillRect(left, top, w, h, RED);
      gfx->fillRect(left, top + h / 3, w, h / 8, YELLOW);
      gfx->drawRect(left, top, w, h, 0x7800);
      break;

    case ITEM_CAR:
      gfx->fillRect(left, top, w, h, BLUE);
      gfx->fillRect(left + w / 6, top + h / 8, w * 2 / 3, h / 3, 0x0010);
      gfx->fillRect(left + w / 6, top + h / 2, w * 2 / 3, h / 3, 0x0210);
      gfx->fillRect(left, p.y - h / 12, w / 5, h / 12, BLACK);
      gfx->fillRect(left + w - w / 5, p.y - h / 12, w / 5, h / 12, BLACK);
      break;

    case ITEM_BLOCK:
      gfx->fillRect(left, top, w, h, YELLOW);
      for (int i = 0; i < 3; ++i) {
        gfx->fillRect(left + i * w / 3, top, w / 6, h, BLACK);
      }
      break;

    case ITEM_PUDDLE:
      gfx->fillEllipse(p.x, p.y - h / 4, w * 3 / 5, h / 3, 0x2104);
      break;

    case ITEM_NITRO: {
      const int r = w / 2 + 2;
      gfx->fillTriangle(p.x, p.y - h - r, p.x + r, p.y - h, p.x, p.y - h + r, 0x2C64);
      gfx->fillTriangle(p.x, p.y - h - r, p.x - r, p.y - h, p.x, p.y - h + r, 0x2C64);
      break;
    }

    case ITEM_COIN:
      gfx->fillCircle(p.x, p.y - h / 2, w / 2, YELLOW);
      gfx->drawCircle(p.x, p.y - h / 2, w / 2, 0x8A80);
      break;

    default:
      break;
  }
}

}  // namespace

namespace gameui {

void attract() {
  game_mode = true;
  screen_index = SCREEN_GAME;
  runtime.screen = SCREEN_GAME;

  game_state = 0;
  game_score_1 = 0;
  game_score_1_tik = 0;
  game_speed = 0.15f;
  alert_tik = 0;
  game_time = 0;
  game_spawn = 0.8f;
  game_lane = 1;
  clearItems();

  s_startGuardUntil = millis() + 400;
}

void begin() {
  attract();
  game_state = 1;
}

void exit() {
  // Выход из игры не должен терять новый рекорд.
  if (game_score_1 > game_best) {
    game_best = game_score_1;
    preferences.putInt("game_best", game_best);
    runtime.gameBest = game_best;
  }

  game_mode = false;
  screen_index = SCREEN_DASHBOARD;
  runtime.screen = SCREEN_DASHBOARD;
  clearItems();
}

void update(float dt) {
  if (game_state == 0) {
    // Приглашение: редкие предметы медленно ползут, чтобы дорога жила.
    game_spawn -= dt;
    if (game_spawn <= 0) {
      spawnWave();
      game_spawn = 1.6f;
    }

    for (int i = 0; i < itemCount; ++i) {
      items[i].z += dt * 1.2f;
    }

    for (int i = itemCount - 1; i >= 0; --i) {
      if (items[i].z > 7.0f) {
        removeItem(i);
      }
    }

    return;
  }

  if (game_state != 1) {
    return;
  }

  game_time += dt;

  game_spawn -= dt;
  if (game_spawn <= 0) {
    spawnWave();
    game_spawn = max(0.55f, 1.35f - game_time * 0.02f);
  }

  for (int i = 0; i < itemCount; ++i) {
    GameItem &item = items[i];

    item.z += dt * game_speed * 30.0f;

    if (item.hit || item.z < 5.85f || item.z > 6.85f) {
      continue;
    }

    if (item.lane != game_lane) {
      continue;
    }

    item.hit = true;

    if (item.type == ITEM_COIN) {
      game_score_1 += 100;
      gameAlert("+100", 190);
    } else if (item.type == ITEM_NITRO) {
      game_score_1 += 250;
      gameAlert("N2O +250", 150);
    } else {
      game_state = 2;
      // Дорога замирает: кадр аварии не должен «ехать» дальше.
      game_speed = 0.0f;

      if (game_score_1 > game_best) {
        game_best = game_score_1;
        preferences.putInt("game_best", game_best);
      }

      runtime.gameBest = game_best;
      break;
    }
  }

  for (int i = itemCount - 1; i >= 0; --i) {
    if (items[i].z > 7.2f) {
      removeItem(i);
    }
  }
}

void handleTouch(int16_t x, int16_t y) {
  // Выход — правый верхний угол, в том числе с экрана приглашения.
  if (y < 26 && x > 425) {
    exit();
    return;
  }

  if (game_state == 0) {
    if (static_cast<int32_t>(millis() - s_startGuardUntil) < 0) {
      return;
    }

    begin();
    return;
  }

  if (game_state == 2) {
    begin();
    return;
  }

  // Полосы — те же три кнопки, что и карты на приборке.
  if (y >= 26 && y <= 54 && x < 230) {
    const int lane = x / 77;

    if (lane >= 0 && lane <= 2) {
      game_lane = lane;
    }
  }
}

void handleSwipe(int8_t direction) {
  if (game_state != 1) {
    return;
  }

  const int lane = game_lane + direction;

  if (lane >= 0 && lane <= 2) {
    game_lane = lane;
  }
}

void drawItems() {
  // Дальние предметы рисуем первыми, ближние перекрывают их.
  for (int z = 0; z < 16; ++z) {
    for (int i = 0; i < itemCount; ++i) {
      const int bucket = static_cast<int>(items[i].z * 2.0f);

      if (bucket == z) {
        drawItem(items[i]);
      }
    }
  }
}

void drawHud() {
  static const uint16_t kLaneColor[3] = {0x2480, BLUE, RED};

  for (int i = 0; i < 3; ++i) {
    const int x = 4 + i * 70;
    const uint16_t fill = (game_lane == i) ? kLaneColor[i] : 0x2104;

    gfx->fillRect(x, 28, 64, 20, fill);
    gfx->drawRect(x, 28, 64, 20, WHITE);

    gfx->setTextSize(1);
    gfx->setTextColor(WHITE);
    gfx->setCursor(x + 28, 44);
    gfx->println(i + 1);
  }

  gfx->setFont(u8g2_font_crox2hb_tf);
  gfx->setTextSize(1);
  gfx->setTextColor(WHITE);
  gfx->setCursor(250, 44);
  gfx->print("РЕКОРД ");
  gfx->println(game_best);

  gfx->setTextColor(0xF800);
  gfx->setCursor(440, 20);
  gfx->println("X");
}

void drawFrame() {
  ::game();
  drawHud();

  if (game_state == 0) {
    gfx->fillRect(70, 90, 340, 104, BLACK);
    gfx->drawRect(70, 90, 340, 104, WHITE);

    gfx->setFont(u8g2_font_crox4hb_tf);
    gfx->setTextSize(1);
    gfx->setTextColor(WHITE);
    gfx->setCursor(170, 128);
    gfx->println("ИГРА");

    gfx->setFont(u8g2_font_crox2hb_tf);
    gfx->setTextColor(0xC618);
    gfx->setCursor(150, 152);
    gfx->println("Кнопки 1-2-3 — полосы");

    gfx->setTextColor(WHITE);
    gfx->setCursor(175, 176);
    gfx->println("ТАП — СТАРТ");
  }

  if (game_state == 2) {
    gfx->fillRect(60, 82, 360, 118, BLACK);
    gfx->drawRect(60, 82, 360, 118, 0xF800);

    gfx->setFont(u8g2_font_crox4hb_tf);
    gfx->setTextSize(1);
    gfx->setTextColor(0xF800);
    gfx->setCursor(170, 118);
    gfx->println("АВАРИЯ");

    gfx->setFont(u8g2_font_crox2hb_tf);
    gfx->setTextColor(WHITE);
    gfx->setCursor(120, 146);
    gfx->print("СЧЁТ ");
    gfx->print(game_score_1);
    gfx->print("   РЕКОРД ");
    gfx->println(game_best);

    gfx->setTextColor(0xC618);
    gfx->setCursor(160, 178);
    gfx->println("ТАП — ЗАНОВО");
  }

  gfx->flush();
}

}  // namespace gameui
