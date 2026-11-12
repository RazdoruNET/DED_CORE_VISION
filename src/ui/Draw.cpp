#include "ui/Draw.h"

#include <U8g2lib.h>

#include "app/State.h"
#include "app/Status.h"
#include "config/CONFIG.h"
#include "config/PINS.h"
#include "display/Display.h"
#include "ui/Game.h"

// Отрисовка приборки, настроек и игры — перенос DRAW_CODE.h заводской
// прошивки. Логика и координаты сохранены; переименованы только флаги связи,
// чтобы не конфликтовать с POSIX connect().

void drawWellcomeScreen()
{
  gfx->fillScreen(COLOR_WHITE);
  gfx->setCursor(180, 125);

  gfx->setTextSize(1);
  gfx->setTextColor(COLOR_BLACK);
  gfx->println(F("WELCOME TO DED VISION"));

  gfx->flush();

  delay(1000);
}

void drawInitialScreen()
{
  gfx->fillScreen(COLOR_BLACK);
  gfx->setFont(u8g2_font_crox2hb_tf);
  gfx->setTextSize(1);
  gfx->setTextColor(COLOR_WHITE);

  if (itin_step == 0)
  {
    gfx->setCursor(96, 105);
    gfx->println(F("ОТКРОЙТЕ МОДУЛИ - НАЙТИ МОДУЛЬ"));

    gfx->setCursor(118, 125);
    gfx->println(F("ИЩУ СЕТЬ СОПРЯЖЕНИЯ DED_PAIR_*"));
  }
  else if (itin_step == 1)
  {
    gfx->setCursor(178, 125);
    gfx->println(F("ПОДКЛЮЧЕНИЕ К ЯДРУ"));
  }
  else if (itin_step == 2)
  {
    gfx->setCursor(150, 125);
    gfx->println(F("СИНХРОНИЗАЦИЯ С ЯДРОМ"));
  }
  else if (itin_step == 3)
  {
    gfx->setCursor(190, 125);
    gfx->println(F("ГОТОВО"));
  }

  gfx->flush();
}

void drawConnectScreen()
{
  
  if (coreOnline == false)
  {
    gfx->setFont(u8g2_font_spleen8x16_mf);
    
    gfx->setTextSize(1);
    
    if (coreStale == true)
    {
      gfx->fillRect(50,90,370,100, COLOR_WHITE);
      gfx->drawRect(50,90,370,100, COLOR_BLACK);
      
      gfx->setTextColor(RED);

      gfx->setCursor(145, 135);
      gfx->println(F("YOU DED BOX IS OFFLINE!!!"));

      gfx->setTextColor(COLOR_BLACK);

      gfx->setCursor(130, 155);
      gfx->println(F("PLEASE CHECK IT WORKS DED BOX"));
    }
    else
    {
      gfx->fillRect(50,115,370,50, COLOR_WHITE);
      gfx->drawRect(50,115,370,50, COLOR_BLACK);
    
      gfx->setTextColor(COLOR_BLACK);
      
      gfx->setCursor(105, 145);
      gfx->println(F("ONE MOMENT I'AM CONNECT TO DED BOX"));
    }
  }
}

void drawSuccesedConnectScreen()
{
 // CODE HEAR
}

void drawTahometr()
{
  float one_persent = max_rpm / 10;
  int rpm_persent = (rpm / one_persent);

  for (int i = 1; i <= 10; i++)
  {
    uint32_t color = 0x08A0;
    
    int cell_max_rpm = one_persent * i;
    
    if (i > 7)
    { 
      if (rpm < cell_max_rpm)
      {
        color = 0x1000; // DARK RED
      }
      else
      {
        color = RED; // RED
      }
    }
    else if (i > 3)
    { 
      if (rpm < cell_max_rpm)
      {
        color = 0x20E1; // DARK YELLOW
      }
      else
      {
        color = 0xCC80; // YELLOW 
      }
    }
    else 
    { 
      if (rpm < cell_max_rpm)
      {
        color = 0x08A0; // DARK GREEN
      }
      else
      {
        color = GREEN; // GREEN 
      }
    }
    
    gfx->fillRect(454,247-((i-1)*27),25,25, color);

    //////

    gfx->setFont(u8g2_font_crox2hb_tf);
    gfx->setTextSize(1);
    
    String max_rpm_str = "MAX " + String((max_rpm > max_cart_rpm) ? max_cart_rpm : max_rpm);

    if (coreStale == true)
    {
      max_rpm_str = "ERROR TO";
    }

    gfx->setCursor(251, 257);
   
    gfx->setTextColor(RED);
    gfx->println(max_rpm_str);
    
    String rpm_str = String(rpm);
    
    if (coreStale == true)
    {
      rpm_str = "DED BOX";
    }
    else
    {
      gfx->setFont(u8g2_font_crox4hb_tf);
      gfx->setTextSize(1);
    }

    int x = 440 - (rpm_str.length() * 11);
    
    gfx->setCursor(x, 260);
    gfx->setTextColor(COLOR_BLACK);
    gfx->println(rpm_str);
  }
}

void drawSpeedometr()
{
  gfx->setFont(u8g2_font_crox2hb_tf);

  if (toggle_left == false)
  {
    if (speed_draw < 10)
    {
      gfx->setTextColor(COLOR_BLACK);
      gfx->setCursor(27, 260);
    }
    else if (speed_draw < 100)
    {
      gfx->setTextColor(COLOR_BLACK);
      gfx->setCursor(20, 260);
    }
    else 
    {
      gfx->setTextColor(RED);
      gfx->setCursor(10, 260);
    }

    gfx->setTextSize(2);
    gfx->println(String(speed_draw));

  }
  else
  {
    
    gfx->setTextColor(COLOR_BLACK);
    gfx->setTextSize(2);
    if (kpp == -1)
    {
      gfx->setCursor(27, 260);
      gfx->println("--");
    }
    else if (kpp == 0)
    {
      gfx->setCursor(23, 260);
      gfx->println("N");
    }
    else
    {
      gfx->setCursor(27, 260);
      gfx->println(String(kpp));
    }
  }

  gfx->setFont(u8g2_font_crox2hb_tf);
  gfx->setCursor(80, 257);
  gfx->setTextSize(1);
  gfx->setTextColor(COLOR_BLACK);
  gfx->println("OD: " + String(odometr / 1000));

  gfx->setCursor(170, 257);
  gfx->setTextSize(1);
  gfx->setTextColor(COLOR_BLACK);
  
  int draw_mh = (mh / 3600000 < 0) ? 0 : mh / 3600000;

  gfx->println("MH: " + String(draw_mh));
}

void drawCart()
{ 
  if (sizeof(rpms[0])>0)
  {
    int dot_count = (cart_type == 0) ? 7 : (cart_type == 1) ? 12 : 22;
    
    float dc_max_rpm = rpms[dot_count - 1];

    float dc_max_angle = init_angle;  // или какое-то значение по умолчанию

    if (dot_count > 0) {
      for (int i = 1; i < dot_count; i++) {
        if (angles[i] > dc_max_angle) {
          dc_max_angle = angles[i];
        }
      }
    }

    if (dc_max_rpm > 0 && dc_max_angle > 0)
    {
      float x_min = 245;
      float x_max = 440;
      float y_min = 90;
      float y_max = 170;

      if (toggle_right == false)
      {
        y_max = 220;
      }
      
      gfx->drawLine(x_min,y_max,x_max,y_max, 0x31A6);
      gfx->drawLine(x_min,y_min,x_min,y_max, 0x31A6);

      float w = x_max - x_min;
      float h = y_max - y_min; // 90

      float count_step = dot_count;

      float ignition_step = dc_max_angle / h; // 0,37777778
      float rpm_step = dc_max_rpm / w;

      if (ignition_step > 0 && rpm_step > 0)
      {
        float dots_x[] = {};
        float dots_y[] = {};

        for (int i = 0; i < count_step - 1; i++)
        {
          float rpm_dot_1 = rpms[i];
          float angle_dot_1 = angles[i];

          float dot_x_1 = x_min + (rpm_dot_1/rpm_step);
          float dot_y_1 = y_max - (angle_dot_1/ignition_step);
        
          float rpm_dot_2 = rpms[i + 1];
          float angle_dot_2 = angles[i + 1];

          float dot_x_2 = x_min + (rpm_dot_2/rpm_step);
          float dot_y_2 = y_max - (angle_dot_2/ignition_step);

          gfx->drawLine(dot_x_1,dot_y_1,dot_x_2,dot_y_2, cart_color);

          gfx->fillCircle(dot_x_1,dot_y_1,2, COLOR_BLACK);
          
          if (i + 1 == count_step - 1)
          {
            gfx->fillCircle(dot_x_2,dot_y_2,2, COLOR_BLACK);
          }
        }

        if (rpm > 0)
        {
          float x_rpm = x_min + (rpm/rpm_step);
          gfx->drawLine(x_rpm,y_min,x_rpm,y_max, RED);
        }
      }
    }
  }
}

void drawCartSelector()
{

  int sel_cart = cart + 1;
  
  int cart_1_Y = (cart_1_click) ? 2 : 0;
  int cart_2_Y = (cart_2_click) ? 2 : 0;
  int cart_3_Y = (cart_3_click) ? 2 : 0;

  if (sel_cart == 1) 
  { 
    cart_color = 0x2480; 
    gfx->fillRect(0,5 + cart_1_Y,70,40 + cart_1_Y, cart_color); //green 
    gfx->drawRect(0,5 + cart_1_Y,70,40 + cart_1_Y, COLOR_WHITE); // COLOR_WHITE 
  }    
  else 
  { 
    gfx->fillRect(0,5 + cart_1_Y,70,40 + cart_1_Y, COLOR_WHITE); // dark green 
    gfx->drawRect(0,5 + cart_1_Y,70,40 + cart_1_Y, 0x4208); // GRAY 
  }                                          

  if (sel_cart == 2) 
  { 
    cart_color = BLUE; 
    gfx->fillRect(78,5 + cart_2_Y,70,40 + cart_2_Y, cart_color); // blue 
    gfx->drawRect(78,5 + cart_2_Y,70,40 + cart_2_Y, COLOR_WHITE); // COLOR_WHITE 
  }    
  else 
  { 
    gfx->fillRect(78,5 + cart_2_Y,70,40 + cart_2_Y, COLOR_WHITE); // dark blue 
    gfx->drawRect(78,5 + cart_2_Y,70,40 + cart_2_Y, 0x4208); // GRAY 
  }                                         

  if (sel_cart == 3) 
  { 
    cart_color = RED; 
    gfx->fillRect(156,5 + cart_3_Y,70,40 + cart_3_Y, cart_color); // red
    gfx->drawRect(156,5 + cart_3_Y,70,40 + cart_3_Y, COLOR_WHITE); // COLOR_WHITE
  } 
  else 
  { 
    gfx->fillRect(156,5 + cart_3_Y,70,40 + cart_3_Y, COLOR_WHITE); // dark red 
    gfx->drawRect(156,5 + cart_3_Y,70,40 + cart_3_Y, 0x4208); // GRAY
  }                                        
  

  gfx->setCursor(30, 36 + cart_1_Y);
  gfx->setTextSize(2);

  if (lock_screen == true && sel_cart == 1)
  {
    gfx->setTextColor(COLOR_BLACK);
  }
  else if (lock_screen == true)
  {
    gfx->setTextColor(0x8410);
  }
  else
  {
    gfx->setTextColor(COLOR_BLACK);
  }

  gfx->setFont(u8g2_font_lastapprenticebold_tr);
  gfx->println("1");

  gfx->setCursor(107, 36 + cart_2_Y);
  gfx->setTextSize(2);
  
  if (lock_screen == true && sel_cart == 2)
  {
    gfx->setTextColor(COLOR_BLACK);
  }
  else if (lock_screen == true)
  {
    gfx->setTextColor(0x8410);
  }
  else
  {
    gfx->setTextColor(COLOR_BLACK);
  }

  gfx->setFont(u8g2_font_lastapprenticebold_tr);
  gfx->println("2");

  gfx->setCursor(184, 36 + cart_3_Y);
  gfx->setTextSize(2);
  
  if (lock_screen == true && sel_cart == 3)
  {
    gfx->setTextColor(COLOR_BLACK);
  }
  else if (lock_screen == true)
  {
    gfx->setTextColor(0x8410);
  }
  else
  {
    gfx->setTextColor(COLOR_BLACK);
  }
  
  gfx->setFont(u8g2_font_lastapprenticebold_tr);
  gfx->println("3");


  if (coreStale == false && toggle_info == false)
  {
    if (toggle_right == true)
    {
      gfx->setCursor(255, 160);
    }
    else
    {
      gfx->setCursor(255, 210);
    }

    gfx->setTextSize(1);
    gfx->setTextColor(COLOR_BLACK);
    gfx->setFont(u8g2_font_spleen8x16_mf);
    gfx->println("IGNITION MAP:");
    
    if (toggle_right == true)
    {
      gfx->setCursor(410, 160);
      gfx->setTextSize(3);
    }
    else
    {
      gfx->setCursor(410, 210);
      gfx->setTextSize(4);
    }

    
    gfx->setTextColor(cart_color);
    gfx->setFont(u8g2_font_spleen8x16_mf);
    gfx->println(String(sel_cart));
  } 
}

void drawRootMenu()
{
  if (active_quiqshifter)
  {
    if (use_quiqshifter)
    {
      gfx->fillRect(245,5+active_quiqshifter_click*3,100,70-active_quiqshifter_click*5, COLOR_BLACK);
    }
    else
    {
      gfx->fillRect(245,5+active_quiqshifter_click*3,100,70-active_quiqshifter_click*5, RED);
    }

    gfx->setTextColor(COLOR_WHITE);
  }
  else
  {

    if (lock_screen == false)
    {
      gfx->drawRect(245,5+active_quiqshifter_click*3,100,70-active_quiqshifter_click*5, COLOR_BLACK);
      gfx->setTextColor(COLOR_BLACK);
    }
    else
    {
      gfx->drawRect(245,5+active_quiqshifter_click*3,100,70-active_quiqshifter_click*5, 0x8410);
      gfx->setTextColor(0x8410);
    }
    
  }

  gfx->setTextSize(1);
  gfx->setFont(u8g2_font_spleen8x16_mf);

  gfx->setCursor(258, 45);
  gfx->println("Q-SHIFTER");

    
  if (lock_screen == false)
  {
    gfx->drawRect(360,5+active_option_click*3,80,70-active_option_click*5, COLOR_BLACK);
    gfx->setTextColor(COLOR_BLACK);
    gfx->setCursor(373, 45);
    gfx->println("OPTIONS");
  }
  else
  {
    if (unlock_sense > 1000)
    {
      gfx->fillRect(360, 0, 80, 75, COLOR_BLACK);
    }
    else
    {
      gfx->fillRect(360, 0, 80, 75, RED);
    }
    gfx->setTextColor(COLOR_WHITE);
    gfx->setCursor(376, 45);
    gfx->println("UNLOCK");
  }

}

void drawOptionMenu()
{
  gfx->fillRect(426,0,54,280, RED);
  gfx->setTextSize(1);
  gfx->setFont(u8g2_font_crox2hb_tf);
  gfx->setTextColor(COLOR_WHITE);
  gfx->setCursor(438, 140);  
  gfx->println("ВЫХОД");
}

void drawTransmition()
{
  if (toggle_left == false)
  {
    gfx->setTextSize(7);
    gfx->setFont(u8g2_font_crox4hb_tf);

    if (kpp == -1)
    {
      gfx->fillRect(10,50,210,170, COLOR_WHITE);
      gfx->setCursor(85, 160);
      gfx->setTextSize(5);
      gfx->setTextColor(0x31A6);
      gfx->println("--");
    }
    else if (kpp == 0)
    {
      gfx->fillRect(10,50,210,170, 0x2480);
      gfx->setCursor(60, 185);
      gfx->setTextColor(COLOR_BLACK);
      gfx->println("N");
    }
    else if (kpp == 3)
    {
      gfx->fillRect(10,50,210,170, 0xCC80);
      gfx->setCursor(80, 185);
      gfx->setTextColor(COLOR_BLACK);
      gfx->println("3");
    }
    else if (kpp == 4 || kpp == 5 || kpp == 6)
    {
      gfx->fillRect(10,50,210,170, RED);
      gfx->setCursor(80, 185);
      gfx->setTextColor(COLOR_WHITE);
      gfx->println(kpp);
    }
    else
    {
      gfx->fillRect(10,50,210,170, COLOR_WHITE);
      gfx->setCursor(80, 185);
      gfx->setTextColor(COLOR_BLACK);
      gfx->println(kpp);
    }
  }
  else
  {
    if (speed_draw == 0)
    {
      gfx->fillRect(10,50,210,170, COLOR_WHITE);
      gfx->setFont(u8g2_font_crox4hb_tf);
      gfx->setCursor(60, 170);
      gfx->setTextSize(5);
      gfx->setTextColor(0x31A6);
      gfx->println("00");
    }
    else if (speed_draw > 0 && speed_draw < 10)
    {
      gfx->fillRect(10,50,210,170, COLOR_WHITE);
      gfx->setFont(u8g2_font_crox4hb_tf);
      gfx->setCursor(60, 170);
      gfx->setTextSize(5);
      gfx->setTextColor(COLOR_BLACK);
      gfx->println("0" + String(speed_draw));
    }
    else if (speed_draw >= 10 && speed_draw < 99)
    {
      gfx->fillRect(10,50,210,170, COLOR_WHITE);
      gfx->setFont(u8g2_font_crox4hb_tf);
      gfx->setCursor(60, 170);
      gfx->setTextSize(5);
      gfx->setTextColor(COLOR_BLACK);
      gfx->println(String(speed_draw));
    }
    else if (speed_draw > 99)
    {
      gfx->fillRect(10,50,210,170, COLOR_WHITE);
      gfx->setFont(u8g2_font_crox4hb_tf);
      gfx->setCursor(30, 170);
      gfx->setTextSize(5);
      gfx->setTextColor(RED);
      gfx->println(speed_draw);
    }
  }
}

void drawINFO()
{
 
  size_t freeSRAM = ESP.getFreeHeap();

  // Получение доступного heap
  size_t fq = ESP.getFlashChipSpeed();

  gfx->setTextSize(1);
  
  gfx->setTextColor(COLOR_BLACK);

  gfx->setCursor(250, 95);
  gfx->println("RAM:" + String(ESP.getFreeHeap()) + " / " + String(ESP.getHeapSize()));

  gfx->setCursor(250, 115);
  gfx->println("CPU " + String(ESP.getCpuFreqMHz()));


  gfx->setCursor(250, 140);
  gfx->println("OD METER: " + String(odometr));

  gfx->setCursor(250, 160);
  gfx->println("MH SEK: " + String(mh/1000));

  gfx->setCursor(250, 180);
  gfx->println("DLINNA WH: " + String(dlinna*1000));


  gfx->setCursor(250, 215);
  gfx->println("SENSER: " + String(digitalRead(t_8)));
}

void drawPower()
{
  if (toggle_right == true)
  {
    if (coreOnline)
    {
      if (power_draw - power < 5)
      {
        power_draw = power;
      }
      else
      {
        if (power_draw > power)
        {
          power_draw -= 5;
        }
        else
        {
          power_draw += 5;
        }
      }
    }

    gfx->fillRect(245,180,195,40, COLOR_BLACK);

    int x = power_draw * 1.95;

    int color_bg = GREEN;
    
    if (x > 90)
    {
      color_bg = RED;
    }
    else if (x > 50)
    {
      color_bg = ORANGE;
    }

    gfx->fillRect(245,180,x,40, color_bg);
    
    gfx->setCursor(260, 205);
    gfx->setFont(u8g2_font_spleen8x16_mf);
    gfx->setTextSize(1);
    gfx->setTextColor(COLOR_WHITE);
    gfx->print("POWER: ");
    gfx->println(String(power_draw));
  }
}

void drawOptions()
{
  gfx->fillScreen(COLOR_WHITE);
 
  int height_item = 54;

  int width = gfx->width();
  int height = gfx->height();
  int cont = height / height_item;
  
  for (int i = 1; i <= cont; i++)
  {
    gfx->drawLine(0, i*height_item, width-60, i*height_item, 0x31A6);
    gfx->setFont(u8g2_font_crox2hb_tf);
    gfx->setTextSize(1);
    gfx->setTextColor(0x4208);
    gfx->setCursor(240, i*height_item-22); 
    gfx->println(optins_name[i-1]);

    switch (optins_var[i-1]) {

      case -1:

        switch (optins_action[i-1]) {

          case 4:
          
          gfx->fillRect(10, (i-1)*height_item+5, 220, height_item-10, RED);
          gfx->setTextSize(1);
          gfx->setTextColor(COLOR_WHITE);
          gfx->setCursor(70, i*height_item-22); 
          gfx->println("ЗАБЛОКИРОВАТЬ");

          break;

          case 5:
          
          if (opt_click_wifi)
          {
            gfx->fillRect(10, (i-1)*height_item+5, 220, height_item-10, RED);
            gfx->setTextColor(COLOR_WHITE);
          }
          else
          {
            gfx->drawRect(10, (i-1)*height_item+5, 220, height_item-10, RED);
            gfx->setTextColor(RED);
          }

          gfx->setTextSize(1);
          gfx->setCursor(30, i*height_item-22); 
          gfx->println("ПОРТАЛ НАСТРОЙКИ");

          if (opt_click_info)
          {
            gfx->fillRect(240, (i-1)*height_item+5, 90, height_item-10, COLOR_BLACK);
            gfx->setTextColor(COLOR_WHITE);
          }
          else
          {
            gfx->drawRect(240, (i-1)*height_item+5, 90, height_item-10, COLOR_BLACK);
            gfx->setTextColor(COLOR_BLACK);
          }

          gfx->setTextSize(1);
          gfx->setCursor(270, i*height_item-22); 
          gfx->println("ИНФО");

          if (opt_click_bw)
          {
            gfx->fillRect(340, (i-1)*height_item+5, 80, height_item-10, COLOR_BLACK);
            gfx->setTextColor(COLOR_WHITE);
          }
          else
          {
            gfx->drawRect(340, (i-1)*height_item+5, 80, height_item-10, COLOR_BLACK);
            gfx->setTextColor(COLOR_BLACK);
          }

          gfx->setTextSize(1);
          gfx->setCursor(370, i*height_item-22); 
          gfx->println("Ч/Б");

          break;

        }

        break;

      default:

        switch (optins_action[i-1]) {

          case 1:
          
          gfx->setTextSize(2);
          gfx->setTextColor(COLOR_BLACK);
          gfx->setCursor(95, i*height_item-15); 
          gfx->println("");

          break;

          case 2:
          
          gfx->setTextSize(2);
          
          gfx->setCursor(95, i*height_item-15); 
          gfx->setTextColor(COLOR_BLACK);
          gfx->println(optins_var[i-1]);
          
          if (opt_click_minus == false)
          {
            gfx->setTextColor(COLOR_BLACK);
            gfx->drawRect(10, (i-1)*height_item+5, 50, height_item-10, COLOR_BLACK);
          }
          else
          {
            gfx->setTextColor(COLOR_WHITE);
            gfx->fillRect(10, (i-1)*height_item+5, 50, height_item-10, COLOR_BLACK);
          }

          gfx->setCursor(26, i*height_item-18); 
          gfx->println("-");

          if (opt_click_plus == false)
          {
            gfx->setTextColor(COLOR_BLACK);
            gfx->drawRect(180, (i-1)*height_item+5, 50, height_item-10, COLOR_BLACK);
          }
          else
          {
            gfx->setTextColor(COLOR_WHITE);
            gfx->fillRect(180, (i-1)*height_item+5, 50, height_item-10, COLOR_BLACK);
          }

          gfx->setCursor(196, i*height_item-18); 
          gfx->println("+");

          break;

          case 6:

          gfx->fillRect(10, (i-1)*height_item+5, 220, height_item-10, 0x2480);
          gfx->setTextSize(1);
          gfx->setTextColor(COLOR_WHITE);
          gfx->setCursor(34, i*height_item-16);
          gfx->println("ИГРАТЬ  >");

          break;

          case 3:
          
          gfx->setTextSize(2);
          gfx->setTextColor(COLOR_BLACK);
          gfx->setCursor(95, i*height_item-15); 
          gfx->println((int)(optins_var[i-1]));

          if (opt_click_light_minus == false)
          {
            gfx->setTextColor(COLOR_BLACK);
            gfx->drawRect(10, (i-1)*height_item+5, 50, height_item-10, COLOR_BLACK);
          }
          else
          {
            gfx->setTextColor(COLOR_WHITE);
            gfx->fillRect(10, (i-1)*height_item+5, 50, height_item-10, COLOR_BLACK);
          }

          gfx->setCursor(26, i*height_item-18);
          gfx->println("-");

          if (opt_click_light_plus == false)
          {
            gfx->setTextColor(COLOR_BLACK);
            gfx->drawRect(180, (i-1)*height_item+5, 50, height_item-10, COLOR_BLACK);
          }
          else
          {
            gfx->setTextColor(COLOR_WHITE);
            gfx->fillRect(180, (i-1)*height_item+5, 50, height_item-10, COLOR_BLACK);
          }

          gfx->setCursor(196, i*height_item-18); 
          gfx->println("+");

          break;

        }

        break;

    }
  }

  drawOptionMenu();

  gfx->flush();
}

void gameAlert(String txt, int x)
{
  alert_tik = 25;
  alert_value = txt;
  alert_x = x;
}

void game()
{
  gfx->fillRect(0, 0, 480, 24, COLOR_BLACK);
  
  gfx->setTextSize(2);
  gfx->setTextColor(COLOR_WHITE);
  gfx->setCursor(5, 5); 
  gfx->println(game_score_1);

  int mot_x = 240;
  int mot_y = 160;
  int s_cy = 0;

  // В игре положение машины задаёт выбранная полоса, а не карта ядра:
  // карта при этом не переключается.
  switch(game_lane) {

    case 2:

      mot_x = 320;
      s_cy = -50;
      break;

    case 1:

      mot_x = 190;
      s_cy = 0;
      break;

    default: 

      mot_x = 60;
      s_cy = 50;
      break;

  }

  int w = 480;
  int h = 270;

  int cx = 240; // Центр экрана (X)
  int cy = 135; // Центр экрана (Y) - точка схода

  gfx->fillRect(0, 24, w, 80, CYAN);
  gfx->fillRect(0, 80, w, 40, 0x6FFE);
  gfx->fillRect(0, 120, w, 15, 0xBFFE);
  
  gfx->fillRect(0, cy, w, 270, 0x2D00);

  static float offset = 0;

  offset += game_speed;
  
  if (offset >= 1.0) offset -= 1.0;

  // 1. РИСУЕМ ВЕРТИКАЛЬНЫЕ ЛИНИИ (сходящиеся в центре горизонта)
  for (int i = -25; i <= 25; i++) {
    if (i%2 != 0)
    {
      int x_start = ((w / 2) + (i * 10)); // Точка на горизонте
      int x_end = ((w / 2) + (i * 100)) + s_cy;  // Точка внизу экрана
      gfx->drawLine(x_start, cy, x_end, h, 0x3F20);
    }
  }

  // 2. КВАДРАТЫ/ОКРУЖНОСТИ (Горизонтальные линии, расширяющиеся на нас)
  for (int i = 1; i < 12; i++) {
      // z — это расстояние от центра. Чем больше z, тем ближе линия к нам.
      float z = i + offset;
      
      // Экспоненциальный рост размера, чтобы создать эффект скорости
      int size_w = (int)(pow(z, 2.5) * 2);
      int size_h = (int)(pow(z, 2.5) * 1.2);

      // Верхняя грань
      //gfx->drawLine(0, cy - size_h, w, cy - size_h, COLOR_BLACK);
      // Нижняя грань (земля)
      gfx->drawLine(0, cy + size_h, w, cy + size_h, 0x3F20);
  }
  
  // Предметы едут по дороге под машиной. Рисуем их до треугольников, чтобы
  // ближние перекрывались силуэтом машины.
  gameui::drawItems();

  // Очки и разгон считаются только во время заезда: на экране приглашения
  // счёт должен стоять на нуле.
  if (game_state == 1)
  {
    game_score_1_tik++;

    if (game_score_1_tik > 250)
    {
      game_score_1 += 100;
      gameAlert("+100", 190);

      if (game_score_1 % 500 == 0)
      {
        game_speed += 0.05;
        gameAlert("SPEED UP", 150);
      }

      game_score_1_tik = 0;
    }
  }
  //gfx->fillRect(0, 120, w, 15, 0xBFFE);
  
  if (alert_tik > 0)
  {
    gfx->setTextSize(4);
    gfx->setTextColor(COLOR_WHITE);
    gfx->setCursor(alert_x, 65); 
    gfx->println(alert_value);
    alert_tik --;
  }

  gfx->fillTriangle(-60 + s_cy, 275, 240, 275, 210, 135, 0x3942);

  gfx->fillTriangle(210, 135, 275, 135, 240, 275, 0x3942);

  gfx->fillTriangle(240, 275, 540 + s_cy, 275, 270, 135, 0x3942);

  for (int i = 1; i < 12; i += 0.5) {
      float z = i + offset;      
      int size_w = (int)(pow(z, 2.5) * 2) + 25;
      int size_h = (int)(pow(z * 0.5, 2.5) * 1.2);
      gfx->drawLine(cx - size_w, cy + size_h, cx + size_w, cy + size_h, 0x59E3);
  }

  gfx->fillRect(mot_x, mot_y, 100, 100, COLOR_BLACK);
}



void drawDashboard()
{
  if (game_mode == true)
  {
    gfx->fillScreen(COLOR_WHITE); 

    game();
  }
  else
  {
    gfx->fillScreen(COLOR_WHITE);

    gfx->drawLine(231,0,231,272, 0x8410);
    gfx->drawLine(232,0,232,272, 0x8410);

    gfx->drawLine(450,0,450,272, 0x8410);
    gfx->drawLine(451,0,451,272, 0x8410);

    gfx->drawLine(0,228,450,228, 0x8410);
    gfx->drawLine(340,228,340,272, 0x8410);

    gfx->drawLine(70,228,70,272, 0x8410);
    gfx->drawLine(160,228,160,272, 0x8410);

    if (toggle_info)
    {
      drawINFO();
    }
    else
    {
      drawCart();
      drawPower();
    }
    
    drawCartSelector();
    drawRootMenu();
    drawTransmition();
    drawTahometr();

    if (coreOnline == false)
    {
      drawConnectScreen();
    }
    
    drawSpeedometr();
  }



  gfx->flush();
}

void drawWarning(String text)
{

  gfx->setFont(u8g2_font_streamline_all_t);
  gfx->setCursor(300, 170);
  gfx->setTextSize(4);
  gfx->setTextColor(RED);
  gfx->println("m");

  gfx->setFont(u8g2_font_crox4hb_tf);
  gfx->setCursor(260, 200);
  gfx->setTextSize(1);
  gfx->setTextColor(RED);
  gfx->println(text);

  gfx->flush();

  delay(1000);
}
