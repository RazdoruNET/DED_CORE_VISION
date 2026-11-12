#ifndef APP_STATE_H
#define APP_STATE_H

#include <Arduino.h>
#include <Preferences.h>

// Экран, который сейчас на дисплее. Индекс совпадает с screen_index
// заводской прошивки: 0 — приборка, 1 — настройки, 2 — игра.
enum ScreenIndex : uint8_t {
  SCREEN_DASHBOARD = 0,
  SCREEN_OPTIONS = 1,
  SCREEN_GAME = 2,
};

// Пользовательские настройки экрана (namespace "vision"). Сетевые настройки
// живут отдельно, в net/Settings.
extern Preferences preferences;

// --- Тема -------------------------------------------------------------------
// B/W в настройках меняет местами фон и текст.
extern uint16_t COLOR_BLACK;
extern uint16_t COLOR_WHITE;

// --- Экран ------------------------------------------------------------------
extern int screen_index;

// Шаг подключения для стартового экрана: 0 поиск ядра, 1 подключение,
// 2 синхронизация, 3 готово.
extern int itin_step;

// --- Связь с ядром ----------------------------------------------------------
// coreOnline — кадр телеметрии не старше LINK_TIMEOUT_MS.
// coreStale — связь уже была, но кадры пропали: рисуем «DED BOX OFFLINE».
extern bool coreOnline;
extern bool coreStale;

// --- Телеметрия ядра --------------------------------------------------------
extern int cart;            // active_cart, 0…2
extern int cart_type;       // 0/1/2 → 7/12/22 точки карты
extern int rpm;             // сглаженные обороты для стрелки/чисел
extern int rpm_new;         // последнее значение из кадра
extern int max_rpm;
extern int max_cart_rpm;
extern int init_angle;      // угол на первом обороте карты
extern int power;           // throttle, %
extern int power_draw;      // сглаженное значение для полосы мощности
extern int micro;           // micros() ядра
extern float angles[22];
extern float rpms[22];
extern uint32_t cart_color;

// --- Приборка ---------------------------------------------------------------
extern int cart_old;
extern int rpm_old;
extern int max_rpm_old;
extern bool cart_1_click;
extern bool cart_2_click;
extern bool cart_3_click;

extern bool toggle_left;    // большая зона: передача / скорость
extern bool toggle_right;   // график карты / полоса мощности
extern bool toggle_info;    // экран INFO вместо графиков

extern bool lock_screen;
extern bool active_option;
extern bool active_option_click;
extern bool active_quiqshifter;
extern bool active_quiqshifter_click;
extern bool use_quiqshifter;
extern uint32_t last_quickshifter_click;
extern uint32_t last_option_click;
extern uint32_t unlock_sense;
extern int has_opt_point_test;
extern uint32_t next_senser_tap;

// --- КПП, квикшифтер, скорость, одометр -------------------------------------
extern int t_1, t_2, t_3, t_4, t_5, t_6, t_7, t_8, t_9;
extern int kpp;                 // -1 нет передачи, 0 нейтраль, 1…6 передача
extern bool init_kpp;
extern int last_kpp;
extern uint32_t quiqshifter;   // 0 — отсечка не активна
extern int sensetive_qshifter;
extern float dlinna;            // длина окружности колеса, м
extern int speed;
extern int speed_draw;
extern int last_speed_sense;
extern uint32_t speed_meter_time;
extern uint32_t start_engin;
extern int mh;
extern int odometr;

// --- Экран настроек ---------------------------------------------------------
extern String optins_name[5];
extern int optins_var[5];
extern int optins_action[5];
extern bool opt_click_plus;
extern bool opt_click_minus;
extern bool opt_click_wifi;
extern bool opt_click_info;
extern bool opt_click_bw;
extern bool opt_click_light_plus;
extern bool opt_click_light_minus;

// --- Свайпы ----------------------------------------------------------------
extern bool swipe;
extern uint32_t swipe_start;
extern uint32_t swipe_time;
extern int swipe_x;
extern int swipe_y;
extern int swipe_x_old;
extern int swipe_y_old;
extern int swipe_x_start;
extern int swipe_y_start;
extern int swipe_sensetive;
extern int swipe_sleep_time;
extern int swipe_left;
extern int swipe_right;
extern int swipe_up;
extern int swipe_down;
extern uint32_t swipe_sleep;

// --- Игра -------------------------------------------------------------------
extern bool game_mode;
extern int game_score_1;
extern int game_score_1_tik;
extern float game_speed;
extern int alert_tik;
extern int alert_x;
extern String alert_value;

// Локальная полоса игрока: 0 — левая, 1 — средняя, 2 — правая.
// Карта ядра при этом не переключается.
extern int game_lane;
extern int game_best;
// 0 — приглашение, 1 — заезд, 2 — авария.
extern uint8_t game_state;
extern float game_offset;
extern float game_spawn;
extern float game_time;

// Пока millis() меньше этого значения, loop() не перерисовывает приборку:
// портал показывает тестовую заливку дисплея.
extern unsigned long displayTestUntil;

#endif // APP_STATE_H
