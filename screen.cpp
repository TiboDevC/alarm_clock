#include <RTCZero.h>

#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "alarm_flash_storage.h"
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
    EPD_3IN7_4Gray_Init();
    EPD_3IN7_4Gray_Clear();

    Paint_NewImage(BlackImage, image_x_size, image_y_size, rotate,
                   initial_background_color);

    Paint_SelectImage(BlackImage);
    Paint_SetScale(2);
    Paint_Clear(BLACK);
}

static void draw_days_alarm(const uint16_t start_x, const uint16_t start_y,
                            const uint8_t day_selected, const char *character) {
    constexpr uint16_t radius_circle{13};
    constexpr int8_t offset_text_x{-8};
    constexpr int8_t offset_text_y{-12};

    uint8_t bg_color;
    uint8_t ft_color;

    if (day_selected) {
        bg_color = WHITE;
        ft_color = BLACK;
    } else {
        bg_color = BLACK;
        ft_color = WHITE;
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

    alarm_params_t alarm_0;
    alarm_params_t alarm_1;

    alarm_0 = get_alarm_0();
    alarm_1 = get_alarm_1();

    /* Alarm 0 */
    char clock_buf[30];
    const auto &days_0 = alarm_0.alarm_days.days;
    sprintf(clock_buf, "%2dh%02d", alarm_0.alarm_hour, alarm_0.alarm_minute);
    Paint_DrawString_EN(start_x, 80, clock_buf, &Font24, BLACK, WHITE);

    draw_days_alarm(start_x + 40 * 0, start_y, days_0.monday, "l");
    draw_days_alarm(start_x + 40 * 1, start_y, days_0.tuesday, "m");
    draw_days_alarm(start_x + 40 * 2, start_y, days_0.wednesday, "m");
    draw_days_alarm(start_x + 40 * 3, start_y, days_0.wednesday, "j");
    draw_days_alarm(start_x + 40 * 4, start_y, days_0.wednesday, "v");
    draw_days_alarm(start_x + 40 * 5, start_y, days_0.wednesday, "s");
    draw_days_alarm(start_x + 40 * 6, start_y, days_0.wednesday, "d");

    /* Alarm 1 */
    const auto &days_1 = alarm_1.alarm_days.days;

    sprintf(clock_buf, "%2dh%02d", alarm_1.alarm_hour, alarm_1.alarm_minute);
    Paint_DrawString_EN(start_x, 180, clock_buf, &Font24, BLACK, WHITE);

    draw_days_alarm(start_x + 40 * 0, start_y_1, days_1.monday, "l");
    draw_days_alarm(start_x + 40 * 1, start_y_1, days_1.tuesday, "m");
    draw_days_alarm(start_x + 40 * 2, start_y_1, days_1.wednesday, "m");
    draw_days_alarm(start_x + 40 * 3, start_y_1, days_1.wednesday, "j");
    draw_days_alarm(start_x + 40 * 4, start_y_1, days_1.wednesday, "v");
    draw_days_alarm(start_x + 40 * 5, start_y_1, days_1.wednesday, "s");
    draw_days_alarm(start_x + 40 * 6, start_y_1, days_1.wednesday, "d");

    EPD_3IN7_1Gray_Display_Part(BlackImage, image_x_size, image_y_size,
                                image_buf_size, 0, 0);
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
    const uint8_t day         = rtc.getDay();
    const uint8_t month       = rtc.getMonth();
    const uint8_t year        = rtc.getYear();
    const uint8_t hours       = rtc.getHours();
    const uint8_t minutes     = rtc.getMinutes();
    const uint8_t day_of_week = weekday(year, month, day);
    const char *days_buf[]    = {"dimanche", "lundi",    "mardi", "mercredi",
                                 "jeudi",    "vendredi", "samedi"};
    const char *months_buf[]  = {"",        "janvier",   "fevrier", "mars",
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

    EPD_3IN7_1Gray_Display_Part(BlackImage, image_x_size, image_y_size,
                                image_buf_size, 0, 0);
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
 * 6                1
 *   8 9 10 0 x 4 x
 * 7                x
 */
void ui_button_event(const uint8_t button_id) {
    switch (button_id) {
        case 6:
            ui_set_state(menu_clock);
            break;
        case 7:
            ui_set_state(menu_settings);
            break;
        default:
            break;
    }
    if (ui_state == menu_settings) {
        alarm_params_t alarm_0 = get_alarm_0();
        auto &days_0           = alarm_0.alarm_days.days;
        switch (button_id) {
            case 8:
                days_0.monday = ~days_0.monday;
                break;
            case 9:
                days_0.tuesday = ~days_0.tuesday;
                break;
            case 10:
                days_0.wednesday = ~days_0.wednesday;
                break;
            case 0:
                days_0.thursday = ~days_0.thursday;
                break;
            case 4:
                days_0.saturday = ~days_0.saturday;
                break;
            case 1:
                alarm_0.alarm_minute += 5;
                if (alarm_0.alarm_minute > 55) {
                    alarm_0.alarm_hour++;
                    alarm_0.alarm_hour %= 23;
                    alarm_0.alarm_minute = 0;
                }
            default:
                break;
        }
        alarm_0.is_set = true;
        set_alarm_0(alarm_0);
    }
    ui_update();
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
