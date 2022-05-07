#include <RTCZero.h>

#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
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

void screen_display_param() {
    constexpr uint16_t start_x{100};
    constexpr uint16_t start_y{130};
    constexpr uint16_t radius_circle{13};
    constexpr int8_t offset_text_x{-8};
    constexpr int8_t offset_text_y{-12};

    Paint_DrawString_EN(start_x, 50, "Reveil de la semaine", &Font24, BLACK,
                        WHITE);

    Paint_DrawCircle(start_x, start_y, radius_circle, WHITE, DOT_PIXEL_3X3,
                     DRAW_FILL_FULL);
    Paint_DrawString_EN(offset_text_x + start_x, start_y + offset_text_y, "l",
                        &Font24, WHITE, BLACK);

    Paint_DrawCircle(start_x + 40, start_y, radius_circle, WHITE, DOT_PIXEL_3X3,
                     DRAW_FILL_FULL);
    Paint_DrawString_EN(offset_text_x + start_x + 40, start_y + offset_text_y,
                        "m", &Font24, WHITE, BLACK);

    Paint_DrawCircle(start_x + 40 * 2, start_y, radius_circle, WHITE,
                     DOT_PIXEL_3X3, DRAW_FILL_FULL);
    Paint_DrawString_EN(offset_text_x + start_x + 40 * 2,
                        start_y + offset_text_y, "m", &Font24, WHITE, BLACK);

    Paint_DrawCircle(start_x + 40 * 3, start_y, radius_circle, WHITE,
                     DOT_PIXEL_3X3, DRAW_FILL_FULL);
    Paint_DrawString_EN(offset_text_x + start_x + 40 * 3,
                        start_y + offset_text_y, "j", &Font24, WHITE, BLACK);

    Paint_DrawCircle(start_x + 40 * 4, start_y, radius_circle, WHITE,
                     DOT_PIXEL_3X3, DRAW_FILL_FULL);
    Paint_DrawString_EN(offset_text_x + start_x + 40 * 4,
                        start_y + offset_text_y, "v", &Font24, WHITE, BLACK);

    Paint_DrawCircle(start_x + 40 * 5, start_y, radius_circle, BLACK,
                     DOT_PIXEL_3X3, DRAW_FILL_FULL);
    Paint_DrawString_EN(offset_text_x + start_x + 40 * 5,
                        start_y + offset_text_y, "s", &Font24, BLACK, WHITE);

    Paint_DrawCircle(start_x + 40 * 6, start_y, radius_circle, BLACK,
                     DOT_PIXEL_3X3, DRAW_FILL_FULL);
    Paint_DrawString_EN(offset_text_x + start_x + 40 * 6,
                        start_y + offset_text_y, "d", &Font24, BLACK, WHITE);

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
}

void ui_update() {
    init_screen();
    switch (ui_state) {
        case menu_clock:
            Serial.println("menu_clock state");
            screen_update_clock();
            break;
        case menu_settings:
            Serial.println("menu_settings state");
            screen_display_param();
            break;
    }
    EPD_3IN7_Sleep();
}
