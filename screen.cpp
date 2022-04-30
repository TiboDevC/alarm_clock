#include "screen.h"
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"

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
    DEV_Delay_ms(1000);

    /*EPD_3IN7_1Gray_Init();
    EPD_3IN7_1Gray_Clear();
    DEV_Delay_ms(1000);*/

    Paint_NewImage(BlackImage, image_x_size, image_y_size, rotate,
                   initial_background_color);

    Paint_SelectImage(BlackImage);
    Paint_SetScale(2);
    Paint_Clear(BLACK);

    Serial.print("Allocate ");
    Serial.println(sizeof(BlackImage));
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
    DEV_Delay_ms(2000);
}
