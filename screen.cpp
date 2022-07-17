#include <RTCZero.h>

#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "alarm_flash_storage.h"
#include "button.h"
#include "screen.h"

static RTCZero rtc;

static constexpr uint32_t image_size(const uint16_t x_size,
                                     const uint16_t y_size,
                                     const grey_scale_t grey_scale) {
    return (x_size * y_size) / ((uint32_t) grey_scale);
}

// Create a new image cache
static constexpr uint16_t image_x_size{8 * 35};
static_assert(image_x_size % 8 == 0, "");
static constexpr uint16_t image_y_size{480};
static constexpr grey_scale_t image_grey_scale{grey_scale_2};
static constexpr uint32_t image_buf_size{
        image_size(image_x_size, image_y_size, image_grey_scale)};
static constexpr uint16_t rotate{270};
static constexpr uint8_t initial_background_color{BLACK};

uint8_t BlackImage[image_buf_size];

void init_screen() {
    Serial.println("Init screen");
    EPD_3IN7_1Gray_Init();
    EPD_3IN7_1Gray_Clear();

    Paint_NewImage(BlackImage, image_x_size, image_y_size, rotate,
                   initial_background_color);

    Paint_SelectImage(BlackImage);
    Paint_SetScale(2);
    Paint_Clear(BLACK);
}

static void draw_days_alarm(const uint16_t start_x, const uint16_t start_y,
                            const uint8_t day_selected, const char *character,
                            const uint8_t bg_color_default) {
    constexpr uint16_t radius_circle{15};
    constexpr int8_t offset_text_x{-8};
    constexpr int8_t offset_text_y{-12};

    uint8_t bg_color;
    uint8_t ft_color;

    if (day_selected) {
        if (bg_color_default == BLACK) {
            bg_color = WHITE;
            ft_color = BLACK;
        } else {
            bg_color = BLACK;
            ft_color = WHITE;
        }
    } else {
        if (bg_color_default == BLACK) {
            bg_color = BLACK;
            ft_color = WHITE;
        } else {
            bg_color = WHITE;
            ft_color = BLACK;
        }
    }
    Paint_DrawCircle(start_x, start_y, radius_circle, bg_color, DOT_PIXEL_3X3,
                     DRAW_FILL_FULL);
    Paint_DrawString_EN(offset_text_x + start_x, start_y + offset_text_y,
                        character, &Font24, bg_color, ft_color);
}

void screen_display_param() {
    constexpr uint16_t start_x{150};
    constexpr uint16_t start_y{130};
    constexpr uint16_t start_y_1{230};

    const uint8_t bg_color = BLACK;
    const uint8_t ft_color = WHITE;

    alarm_params_t alarm_0;
    alarm_params_t alarm_1;

    alarm_0 = get_alarm_0();
    alarm_1 = get_alarm_1();

    Paint_Clear(bg_color);

    /* Alarm 0 */
    char clock_buf[30];
    const auto &days_0 = alarm_0.alarm_days.days;
    sprintf(clock_buf, "%2dh%02d", alarm_0.alarm_hour, alarm_0.alarm_minute);
    Paint_DrawString_EN(start_x, 80, clock_buf, &Font24, bg_color, ft_color);

    draw_days_alarm(start_x + 40 * 0, start_y, days_0.monday, "l", bg_color);
    draw_days_alarm(start_x + 40 * 1, start_y, days_0.tuesday, "m", bg_color);
    draw_days_alarm(start_x + 40 * 2, start_y, days_0.wednesday, "m", bg_color);
    draw_days_alarm(start_x + 40 * 3, start_y, days_0.thursday, "j", bg_color);
    draw_days_alarm(start_x + 40 * 4, start_y, days_0.friday, "v", bg_color);
    draw_days_alarm(start_x + 40 * 5, start_y, days_0.saturday, "s", bg_color);
    draw_days_alarm(start_x + 40 * 6, start_y, days_0.sunday, "d", bg_color);

    /* Alarm 1 */
    const auto &days_1 = alarm_1.alarm_days.days;

    sprintf(clock_buf, "%2dh%02d", alarm_1.alarm_hour, alarm_1.alarm_minute);
    Paint_DrawString_EN(start_x, 180, clock_buf, &Font24, bg_color, ft_color);

    draw_days_alarm(start_x + 40 * 0, start_y_1, days_1.monday, "l", bg_color);
    draw_days_alarm(start_x + 40 * 1, start_y_1, days_1.tuesday, "m", bg_color);
    draw_days_alarm(start_x + 40 * 2, start_y_1, days_1.wednesday, "m",
                    bg_color);
    draw_days_alarm(start_x + 40 * 3, start_y_1, days_1.thursday, "j",
                    bg_color);
    draw_days_alarm(start_x + 40 * 4, start_y_1, days_1.friday, "v", bg_color);
    draw_days_alarm(start_x + 40 * 5, start_y_1, days_1.saturday, "s",
                    bg_color);
    draw_days_alarm(start_x + 40 * 6, start_y_1, days_1.sunday, "d", bg_color);

    EPD_3IN7_1Gray_Display(BlackImage);
}

static int weekday(int year, int month, int day)
/* Calculate day of week in proleptic Gregorian calendar. Sunday == 0. */
{
    int adjustment, mm, yy;
    if (year < 2000)
        year += 2000;
    adjustment = (14 - month) / 12;
    mm         = month + 12 * adjustment - 2;
    yy         = year - adjustment;
    return (day + (13 * mm - 1) / 5 + yy + yy / 4 - yy / 100 + yy / 400) % 7;
}

void screen_update_clock() {
    const uint8_t day            = rtc.getDay();
    const uint8_t month          = rtc.getMonth();
    const uint8_t year           = rtc.getYear();
    const uint8_t hours          = rtc.getHours();
    const uint8_t minutes        = rtc.getMinutes();
    const uint8_t day_of_week    = weekday(year, month, day);
    const char *days_buf[]       = {"dimanche", "lundi",    "mardi", "mercredi",
                                    "jeudi",    "vendredi", "samedi"};
    const char *days_short_buf[] = {"dim.", "lun.",  "mar.", "mer.",
                                    "jeu.", "vend.", "same."};
    const char *months_buf[]     = {"",        "janvier",   "fevrier", "mars",
                                    "avril",   "mai",       "juin",    "juillet",
                                    "aout",    "septembre", "octobre", "novembre",
                                    "decembre"};

    char clock_buf[16];
    char date_buf[30];

    Paint_Clear(BLACK);
    sprintf(clock_buf, "%2d:%02d", hours, minutes);
    sprintf(date_buf, "%s %d %s", days_buf[day_of_week], day,
            months_buf[month]);
    Paint_DrawString_EN(10, 30, date_buf, &Font24, BLACK, WHITE);
    Paint_DrawString_EN(0, 100, clock_buf, &FontCustom, BLACK, WHITE);

    /* Alarm 0 */
    const alarm_params_t alaram_0 = get_alarm_0();

    if (alaram_0.is_set) {
        const uint32_t minute_of_day = hours * 60 + minutes;
        const uint32_t minute_of_day_alarm =
                alaram_0.alarm_hour * 60 + alaram_0.alarm_minute * 60;
        uint8_t is_alarm_valid = 0;

        sprintf(date_buf, "none");
        for (uint8_t day_it = day_of_week; day_it < day_of_week + 7; day_it++) {
            const uint8_t day_r = day_it % 7;
            const uint8_t alarm_day =
                    (alaram_0.alarm_days.value >> day_r) & 0x01;
            if (alarm_day == 1) {
                if (day_r == day_of_week) {
                    if (minute_of_day < minute_of_day_alarm) {
                        sprintf(date_buf, "auj.");
                        is_alarm_valid = 1;
                        break;
                    }
                } else {
                    sprintf(date_buf, "%s", days_short_buf[day_r]);
                    is_alarm_valid = 1;
                    break;
                }
            }
        }

        if (is_alarm_valid) {
            sprintf(date_buf, "%s %2dh%02d", date_buf, alaram_0.alarm_hour,
                    alaram_0.alarm_minute);
            Paint_DrawString_EN(320, 10, date_buf, &Font20, BLACK, WHITE);
        }
    }

    /* Alarm 1 */
    const alarm_params_t alaram_1 = get_alarm_1();
    if (alaram_1.is_set) {
        const uint32_t minute_of_day = hours * 60 + minutes;
        const uint32_t minute_of_day_alarm =
                alaram_1.alarm_hour * 60 + alaram_1.alarm_minute * 60;
        uint8_t is_alarm_valid = 0;

        sprintf(date_buf, "none");
        for (uint8_t day_it = day_of_week; day_it < day_of_week + 7; day_it++) {
            const uint8_t day_r = day_it % 7;
            const uint8_t alarm_day =
                    (alaram_1.alarm_days.value >> day_r) & 0x01;
            if (alarm_day == 1) {
                if (day_r == day_of_week) {
                    if (minute_of_day < minute_of_day_alarm) {
                        sprintf(date_buf, "auj.");
                        is_alarm_valid = 1;
                        break;
                    }
                } else {
                    sprintf(date_buf, "%s", days_short_buf[day_r]);
                    is_alarm_valid = 1;
                    break;
                }
            }
        }

        if (is_alarm_valid) {
            sprintf(date_buf, "%s %2dh%02d", date_buf, alaram_1.alarm_hour,
                    alaram_1.alarm_minute);
            Paint_DrawString_EN(320, 50, date_buf, &Font20, BLACK, WHITE);
        }
    }

    EPD_3IN7_1Gray_Display(BlackImage);
}

/**********************/

static ui_state_t ui_state{menu_clock};

void ui_set_state(const ui_state_t &state) {
    ui_state = state;
    if (ui_state == menu_settings) {
        init_screen();
    }
}

/*
 * Button map
 * 6                5  16
 *   x x 13 19 1 x x
 * 7                0  17
 */
void ui_button_event(void) {
    uint8_t num_button_ack;
    std::array<button_state_t, NUM_BUTTON> button_action;

    button_get_state(button_action, num_button_ack);

    uint8_t screen_need_update = 0;
    int16_t clock_time;

    for (uint8_t button_id = 0; button_id < num_button_ack; button_id++) {
        const uint8_t pin_id = button_action.at(button_id).pin_id;
        screen_need_update   = 1;
        if (button_action.at(button_id).action == LONG_PRESS) {
            switch (pin_id) {
                case 6:
                case 7:
                    if(ui_state == menu_clock)
                    {
                        ui_set_state(menu_settings);
                    }
                    else
                    {
                        ui_set_state(menu_clock);
                    }
                    break;
                default:
                    break;
            }
        }
        if (ui_state == menu_settings) {
            alarm_params_t alarm_0 = get_alarm_0();
            clock_time = alarm_0.alarm_minute + alarm_0.alarm_hour * 60;
            auto &days_0           = alarm_0.alarm_days.days;
            switch (pin_id) {
                case 13:
                    days_0.wednesday = ~days_0.wednesday;
                    break;
                case 19:
                    days_0.thursday = ~days_0.thursday;
                    break;
                case 1:
                    days_0.friday = ~days_0.friday;
                    break;
                case 16:
                    clock_time += 5 * button_action.at(button_id).push_count;
                    break;
                case 17:
                    clock_time -= 5 * button_action.at(button_id).push_count;
                    break;
                case 5:
                    clock_time += (1 * 60) * button_action.at(button_id).push_count;
                    break;
                case 0:
                    clock_time -= (1 * 60) * button_action.at(button_id).push_count;
                    break;
                default:
                    break;
            }
            clock_time %= 60 * 24; // Max 24 hours
            alarm_0.alarm_minute = clock_time % 60;
            alarm_0.alarm_hour = clock_time / 60;
            alarm_0.is_set = true;
            set_alarm_0(alarm_0);
        }
    }
    if (screen_need_update) {
        ui_update();
    }
}

void ui_update() {
    switch (ui_state) {
        case menu_clock:
            Serial.println("menu_clock state");
            init_screen();
            screen_update_clock();
            EPD_3IN7_Sleep();
            break;
        case menu_settings:
            Serial.println("menu_settings state");
            // EPD_3IN7_4Gray_quick_Clear();
            screen_display_param();
            break;
    }
}