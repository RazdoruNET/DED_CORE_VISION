#include "ui/TouchLogic.h"

#include <WiFi.h>

#include "app/State.h"
#include "config/CONFIG.h"
#include "display/Touch.h"
#include "net/CoreLink.h"
#include "net/SetupPortal.h"
#include "ui/Game.h"

// Обработка тача — перенос TASK_CODE.h заводской прошивки. Логика нажатий
// сохранена; команда карты уходит через CoreLink, а не в старый WebSocket.

int16_t last_game_touch_x = 0;

typedef struct keyAndValue_ {
  int16_t x_1;
  int16_t y_1;
  int16_t x_2;
  int16_t y_2;
} clickPoint_t;


clickPoint_t cartArray[] = {
  {0, 0, 75, 60},
  {78, 0, 151, 60},
  {156, 0, 230, 60}
};

clickPoint_t qshift_point = {245, 0, 325, 75};

void touchQShiftHandle()
{
  if (next_senser_tap < micros())
  {
    if (!active_quiqshifter_click)
    {
      if (touch_last_x > qshift_point.x_1 && touch_last_x < qshift_point.x_2 && touch_last_y > qshift_point.y_1 && touch_last_y < qshift_point.y_2)
      {
        next_senser_tap = micros() + 500000;
        active_quiqshifter_click = true;

        if (last_quickshifter_click < micros())
        {
          active_quiqshifter = !active_quiqshifter;
          last_quickshifter_click = micros() + 500000;
        }
      }
    }
  }
}

clickPoint_t options_point = {345, 0, 450, 75};
clickPoint_t options_exit_point = {426, 0, 480, 280};
clickPoint_t lock_point = {10, 0, 240, 54};
clickPoint_t wifi_point = {10, 216, 240, 270};
clickPoint_t exit_fix_point = {140, 180, 340, 234};
clickPoint_t info_point = {240, 216, 330, 270};
clickPoint_t bw_point = {340, 216, 420, 270};

clickPoint_t wheel_plus_point = {180, 50, 240, 100};
clickPoint_t wheel_minus_point = {0, 50, 100, 100};

clickPoint_t light_plus_point = {180, 110, 240, 155};
clickPoint_t light_minus_point = {0, 110, 100, 155};

// Четвёртый пункт экрана настроек: кнопка «ИГРА».
clickPoint_t game_point = {10, 167, 230, 211};

void touchOptionsHandle()
{
  if (next_senser_tap < micros())
  {
    if (!active_option_click)
    {
      bool has_opt_point = (touch_last_x > options_point.x_1 && touch_last_x < options_point.x_2 && touch_last_y > options_point.y_1 && touch_last_y < options_point.y_2);
      bool has_opt_exit_point = (touch_last_x > options_exit_point.x_1 && touch_last_x < options_exit_point.x_2 && touch_last_y > options_exit_point.y_1 && touch_last_y < options_exit_point.y_2);
      bool has_lock_point = (touch_last_x > lock_point.x_1 && touch_last_x < lock_point.x_2 && touch_last_y > lock_point.y_1 && touch_last_y < lock_point.y_2);
      bool has_wifi_point = (touch_last_x > wifi_point.x_1 && touch_last_x < wifi_point.x_2 && touch_last_y > wifi_point.y_1 && touch_last_y < wifi_point.y_2);
      bool has_info_point = (touch_last_x > info_point.x_1 && touch_last_x < info_point.x_2 && touch_last_y > info_point.y_1 && touch_last_y < info_point.y_2);
      bool has_bw_point = (touch_last_x > bw_point.x_1 && touch_last_x < bw_point.x_2 && touch_last_y > bw_point.y_1 && touch_last_y < bw_point.y_2);

      bool has_wheel_plus_point = (touch_last_x > wheel_plus_point.x_1 && touch_last_x < wheel_plus_point.x_2 && touch_last_y > wheel_plus_point.y_1 && touch_last_y < wheel_plus_point.y_2);
      bool has_wheel_minus_point = (touch_last_x > wheel_minus_point.x_1 && touch_last_x < wheel_minus_point.x_2 && touch_last_y > wheel_minus_point.y_1 && touch_last_y < wheel_minus_point.y_2);

      bool has_light_plus_point = (touch_last_x > light_plus_point.x_1 && touch_last_x < light_plus_point.x_2 && touch_last_y > light_plus_point.y_1 && touch_last_y < light_plus_point.y_2);
      bool has_light_minus_point = (touch_last_x > light_minus_point.x_1 && touch_last_x < light_minus_point.x_2 && touch_last_y > light_minus_point.y_1 && touch_last_y < light_minus_point.y_2);
      bool has_game_point = (touch_last_x > game_point.x_1 && touch_last_x < game_point.x_2 && touch_last_y > game_point.y_1 && touch_last_y < game_point.y_2);

      bool sens = false;

      if (screen_index == 0)
      {
        if (has_opt_point)
        {
          sens = true;
        }
      }
      else if (screen_index == 1)
      {
        if (has_game_point)
        {
          next_senser_tap = micros() + 500000;
          game::begin();
        }
        else if (has_opt_exit_point)
        {
          sens = true;
        }
        else if (has_light_plus_point)
        {
          if (opt_click_light_plus == false)
          {
            opt_click_light_plus = true;

            if (optins_var[2] < 100)
            {
              optins_var[2] += 10;
              preferences.putInt("light", optins_var[2]);
              analogWrite(GFX_BL, 2.5 * optins_var[2]);
            }
          }
        }
        else if (has_light_minus_point)
        {
          if (opt_click_light_minus == false)
          {
            opt_click_light_minus = true;

            if (optins_var[2] > 10)
            {
              optins_var[2] -= 10;
              preferences.putInt("light", optins_var[2]);
              analogWrite(GFX_BL, 2.5 * optins_var[2]);
            }
          }
        }
        else if (has_lock_point)
        {
          next_senser_tap = micros() + 500000;
          screen_index = 0;
          lock_screen = true;
          preferences.putBool("lock_screen", lock_screen);
        }
        else if (has_info_point)
        {
          next_senser_tap = micros() + 500000;
          opt_click_info = true;

          screen_index = 0;
          toggle_info = !toggle_info;
        }
        else if (has_bw_point)
        {
          next_senser_tap = micros() + 500000;
          opt_click_bw = true;

          bool bw = !preferences.getBool("black", true);

          preferences.putBool("black", bw);

          COLOR_BLACK = (bw) ? WHITE : BLACK;
          COLOR_WHITE = (bw) ? BLACK : WHITE;

          screen_index = 0;
        }
        else if (has_wifi_point)
        {
          opt_click_wifi = true;

          // Точка доступа портала: в V4 отдельного recovery-режима нет.
          SetupPortal::instance().begin();
          screen_index = 0;
        }
        else if (has_wheel_plus_point)
        {
          if (opt_click_plus == false)
          {
            opt_click_plus = true;

            if (optins_var[1] < 30)
            {
              optins_var[1] += 1;
              preferences.putInt("wheel", optins_var[1]);
              dlinna = 3.14 * ((float)optins_var[1] / (float)39370);
            }
          }
        }
        else if (has_wheel_minus_point)
        {
          if (opt_click_minus == false)
          {
            opt_click_minus = true;

            if (optins_var[1] > 10)
            {
              optins_var[1] -= 1;
              preferences.putInt("wheel", optins_var[1]);
              dlinna = 3.14 * ((float)optins_var[1] / (float)39370);
            }
          }
        }
      }

      if (sens)
      {
        next_senser_tap = micros() + 500000;
        active_option_click = true;

        if (last_option_click < micros())
        {
          active_option = !active_option;
          last_option_click = micros() + 500000;

          if (screen_index == 0)
          {
            screen_index = 1;
          }
          else if (screen_index == 1)
          {
            screen_index = 0;
          }
          else
          {
            screen_index = 0;
          }
        }

        vTaskDelay(20);
      }
    }
    else
    {
      active_option_click = false;
    }
  }
}

clickPoint_t unlock_point = {345, 0, 450, 75};

void touchUnlockHandle()
{
  bool has_opt_point = (touch_last_x > unlock_point.x_1 && touch_last_x < unlock_point.x_2 && touch_last_y > unlock_point.y_1 && touch_last_y < unlock_point.y_2);

  if (has_opt_point == true)
  {
    //Serial.println("has_opt_point");

    if (unlock_sense == 0)
    {
      unlock_sense = micros();
      //Serial.println("unlock_sense");
      //Serial.println(unlock_sense);
    }
    else
    {
      int time = micros() - unlock_sense;
      //Serial.println("time");
      //Serial.println(time);
      if (time > 1000000)
      {
        next_senser_tap = micros() + 1000000;
        lock_screen = false;
        preferences.putBool("lock_screen", lock_screen);
      }
    }
  }
  else
  {
    //Serial.println("has_opt_point_test");
    //Serial.println(has_opt_point_test);

    if (has_opt_point_test > 10)
    {
      has_opt_point_test = 0;
      unlock_sense = 0;
    }

    has_opt_point_test += 1;
  }
}

void touchCardMenuHandle()
{
  // Повторная отправка, пока палец на кнопке, не нужна: флаги сбрасываются
  // при отпускании.
  if (cart_1_click || cart_2_click || cart_3_click)
  {
    return;
  }

  if (!WiFi.isConnected())
  {
    return;
  }

  for (int i = 0; i < 3; i++)
  {
    if (touch_last_x > cartArray[i].x_1 && touch_last_x < cartArray[i].x_2 && touch_last_y > cartArray[i].y_1 && touch_last_y < cartArray[i].y_2)
    {
      cart_1_click = (i == 0);
      cart_2_click = (i == 1);
      cart_3_click = (i == 2);

      CoreLink::instance().sendCart(i + 1);

      next_senser_tap = micros() + 500000;
      return;
    }
  }
}

void swipeSense()
{
  if (next_senser_tap < micros())
  {
    swipe_time = micros();

    if (swipe_start - swipe_time < 500)
    {
      if (touch_last_x < 228 && touch_last_y > 80)
      {
        swipe_x = touch_last_x - swipe_x_old;

        if (swipe_x > swipe_sensetive)
        {
          //Serial.println("SWIPE L");

          if (swipe_x_start == 0)
          {
            swipe_x_start = touch_last_x;
          }

          swipe = true;

          if (micros() - swipe_sleep > swipe_sleep_time)
          {
            toggle_left = !toggle_left;
            preferences.putBool("toggle_left", toggle_left);
            next_senser_tap = micros() + 500000;
          }
          else
          {
            swipe = false;
          }

          swipe_sleep = micros();
        }
        else if (swipe_x < swipe_sensetive * -1)
        {
          //Serial.println("SWIPE R");

          if (swipe_x_start == 0)
          {
            swipe_x_start = touch_last_x;
          }

          swipe = true;

          if (micros() - swipe_sleep > swipe_sleep_time)
          {
            toggle_left = !toggle_left;
            preferences.putBool("toggle_left", toggle_left);
            next_senser_tap = micros() + 500000;
          }
          else
          {
            swipe = false;
          }

          swipe_sleep = micros();
        }

        swipe_x_old = touch_last_x;
      }
      if (touch_last_x > 228 && touch_last_y > 100)
      {
        swipe_y = touch_last_y - swipe_y_old;

        if (swipe_y > swipe_sensetive)
        {
          //Serial.println("SWIPE U");

          if (swipe_y_start == 0)
          {
            swipe_y_start = touch_last_y;
          }

          swipe = true;

          if (micros() - swipe_sleep > swipe_sleep_time)
          {
            toggle_right = !toggle_right;
            preferences.putBool("toggle_right", toggle_right);
            next_senser_tap = micros() + 500000;
          }
          else
          {
            swipe = false;
          }

          swipe_sleep = micros();
        }
        else if (swipe_y < swipe_sensetive * -1)
        {
          //Serial.println("SWIPE D");

          if (swipe_y_start == 0)
          {
            swipe_y_start = touch_last_y;
          }

          swipe = true;

          if (micros() - swipe_sleep > swipe_sleep_time)
          {
            toggle_right = !toggle_right;
            preferences.putBool("toggle_right", toggle_right);
            next_senser_tap = micros() + 500000;
          }
          else
          {
            swipe = false;
          }

          swipe_sleep = micros();
        }

        swipe_y_old = touch_last_y;
      }

      swipe_start = micros();
    }
    else
    {
      swipe_x_old = 0;
      swipe_y_old = 0;
      swipe_x_start = 0;
      swipe_y_start = 0;
      swipe = false;
      swipe_start = 0;
    }
  }
}

bool gameTouchDown = false;

void gameTouchHandle()
{
  // Старт/рестарт — только по первому касанию, иначе игра перезапускалась бы
  // каждые 50 мс, пока палец на экране.
  if (!gameTouchDown)
  {
    game::handleTouch(touch_last_x, touch_last_y);
    gameTouchDown = true;
    last_game_touch_x = touch_last_x;
  }

  if (game_state != 1 || touch_last_y < 60)
  {
    return;
  }

  const int16_t dx = touch_last_x - last_game_touch_x;

  if (abs(dx) > 40 && next_senser_tap < micros())
  {
    game::handleSwipe(dx < 0 ? -1 : 1);
    next_senser_tap = micros() + 250000;
    last_game_touch_x = touch_last_x;
  }
}

void TouchHandler()
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      switch (screen_index)
      {
        case SCREEN_GAME:

          gameTouchHandle();
          break;

        case SCREEN_OPTIONS:

          touchOptionsHandle();
          break;

        default:

          if (lock_screen == false)
          {
            swipeSense();
            touchCardMenuHandle();
            touchQShiftHandle();
            touchOptionsHandle();
          }
          else
          {
            touchUnlockHandle();
          }

          break;
      }
    }
    else
    {
      gameTouchDown = false;
      last_game_touch_x = 0;

      cart_1_click = false;
      cart_2_click = false;
      cart_3_click = false;

      opt_click_plus = false;
      opt_click_minus = false;
      opt_click_wifi = false;
      opt_click_info = false;
      opt_click_bw = false;
      opt_click_light_plus = false;
      opt_click_light_minus = false;

      active_quiqshifter_click = false;

      if (has_opt_point_test > 5)
      {
        has_opt_point_test = 0;
        unlock_sense = 0;
      }

      has_opt_point_test += 1;
    }
  }
}

