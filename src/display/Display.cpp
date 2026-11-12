#include "display/Display.h"

#include "config/CONFIG.h"
#include "config/PINS.h"

namespace {

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    PIN_GFX_CS, PIN_GFX_SCK, PIN_GFX_D0, PIN_GFX_D1, PIN_GFX_D2, PIN_GFX_D3);

Arduino_GFX *panel =
    new Arduino_NV3041A(bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */, true /* IPS */);

}  // namespace

Arduino_GFX *gfx = new Arduino_Canvas(SCREEN_WIDTH, SCREEN_HEIGHT, panel);

namespace display {

void begin() {
  // Подсветку гасим до первой отрисовки: на холодном старте на панели виден
  // мусор, пока не пришёл первый кадр.
  pinMode(PIN_GFX_BL, OUTPUT);
  digitalWrite(PIN_GFX_BL, LOW);

  if (!gfx->begin()) {
    Serial.println("DISPLAY: панель не ответила");
  }

  gfx->fillScreen(BLACK);
  gfx->flush();
}

void setBacklight(uint8_t percent) {
  if (percent > 100) {
    percent = 100;
  }

  analogWrite(PIN_GFX_BL, static_cast<int>(2.5f * percent));
}

void testPattern() {
  gfx->fillScreen(BLACK);

  static const uint16_t colors[] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE};
  const int barWidth = SCREEN_WIDTH / 7;

  for (int i = 0; i < 7; ++i) {
    gfx->fillRect(i * barWidth, 0, barWidth, SCREEN_HEIGHT, colors[i]);
  }

  gfx->setTextSize(2);
  gfx->setTextColor(BLACK);
  gfx->setCursor(12, SCREEN_HEIGHT / 2);
  gfx->println(F("DISPLAY TEST"));

  gfx->flush();
}

}  // namespace display
