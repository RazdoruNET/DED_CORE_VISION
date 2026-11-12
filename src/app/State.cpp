#include "app/State.h"

#include <Arduino_GFX_Library.h>

#include "config/CONFIG.h"
#include "config/PINS.h"

Preferences preferences;

uint16_t COLOR_BLACK = BLACK;
uint16_t COLOR_WHITE = WHITE;

int screen_index = SCREEN_DASHBOARD;
int itin_step = 0;

bool coreOnline = false;
bool coreStale = false;

int cart = 0;
int cart_type = 0;
int rpm = 12000;
int rpm_new = 12000;
int max_rpm = 12000;
int max_cart_rpm = 12000;
int init_angle = 0;
int power = 0;
int power_draw = 100;
int micro = 0;
float angles[22] = {0};
float rpms[22] = {0};
uint32_t cart_color = 0x2480;

int cart_old = 0;
int rpm_old = 0;
int max_rpm_old = 0;
bool cart_1_click = false;
bool cart_2_click = false;
bool cart_3_click = false;

bool toggle_left = false;
bool toggle_right = false;
bool toggle_info = false;

bool lock_screen = false;
bool active_option = false;
bool active_option_click = false;
bool active_quiqshifter = false;
bool active_quiqshifter_click = false;
bool use_quiqshifter = false;
uint32_t last_quickshifter_click = 0;
uint32_t last_option_click = 0;
uint32_t unlock_sense = 0;
int has_opt_point_test = 0;
uint32_t next_senser_tap = 0;

int t_1 = PIN_GEAR_N;
int t_2 = PIN_GEAR_1;
int t_3 = PIN_GEAR_2;
int t_4 = PIN_GEAR_3;
int t_5 = PIN_GEAR_4;
int t_6 = PIN_GEAR_5;
int t_7 = PIN_QS_RELAY;
int t_8 = PIN_SPEED;
int t_9 = PIN_GEAR_6;

int kpp = 0;
bool init_kpp = false;
int last_kpp = -1;
uint32_t quiqshifter = 0;
int sensetive_qshifter = KPP_SENSITIVE;
float dlinna = 0;
int speed = 0;
int speed_draw = 0;
int last_speed_sense = HIGH;
uint32_t speed_meter_time = 0;
uint32_t start_engin = 0;
int mh = 0;
int odometr = 0;

String optins_name[5] = {
    "< БЛОКИРОВКА ЭКРАНА",
    "< РАДИУС КОЛЕСА",
    "< ЯРКОСТЬ ПОДСВЕТКИ",
    "< ИГРА",
    "",
};

int optins_var[5] = {-1, 19, 90, 0, -1};
int optins_action[5] = {4, 2, 3, 6, 5};

bool opt_click_plus = false;
bool opt_click_minus = false;
bool opt_click_wifi = false;
bool opt_click_info = false;
bool opt_click_bw = false;
bool opt_click_light_plus = false;
bool opt_click_light_minus = false;

bool swipe = false;
uint32_t swipe_start = 0;
uint32_t swipe_time = 0;
int swipe_x = 0;
int swipe_y = 0;
int swipe_x_old = 0;
int swipe_y_old = 0;
int swipe_x_start = 0;
int swipe_y_start = 0;
int swipe_sensetive = 25;
int swipe_sleep_time = 250000;
int swipe_left = 0;
int swipe_right = 0;
int swipe_up = 0;
int swipe_down = 0;
uint32_t swipe_sleep = 0;

bool game_mode = false;
int game_score_1 = 0;
int game_score_1_tik = 0;
float game_speed = 0.15f;
int game_speed_step = 500;
int alert_tik = 0;
int alert_x = 0;
String alert_value = "";

int game_lane = 1;
int game_best = 0;
uint8_t game_state = 0;
float game_offset = 0;
float game_spawn = 0.8f;
float game_time = 0;

unsigned long displayTestUntil = 0;
