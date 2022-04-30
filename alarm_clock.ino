#include <stdlib.h>
#include <RTCZero.h>

#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "imagedata.h"
#include "screen.h"

void init_rtc()
{
    RTCZero rtc;

    const uint8_t hours{12};
    const uint8_t minutes{45};
    const uint8_t seconds{10};

    const uint8_t day{30};
    const uint8_t month{04};
    const uint8_t year{22};

    rtc.begin();

    // Set the time
    rtc.setHours(hours);
    rtc.setMinutes(minutes);
    rtc.setSeconds(seconds);


    // Set the date
    rtc.setDay(day);
    rtc.setMonth(month);
    rtc.setYear(year);
}

void setup() {
  Serial.begin(115200);
  delay(5000);

  init_rtc();

  Serial.println("EPD_3IN7_test Demo\r\n");
  DEV_Module_Init();

  Serial.println("e-Paper Init and Clear...\r\n");
  DEV_Delay_ms(500);
  init_screen();


  Serial.println("Paint_NewImage\r\n");

// Paint_SetScale(4);
// Paint_Clear(WHITE);

#if 0  // show image for array    
    Serial.println("show image for array\r\n");
    EPD_3IN7_4Gray_Display(gImage_3in7);
    DEV_Delay_ms(2000);
#endif

#if 0  // Drawing on the image, partial display
    //1.Select Image
    Serial.println("SelectImage:BlackImage\r\n");
    Paint_SelectImage(BlackImage);
    Paint_SetScale(4);
    Paint_Clear(WHITE);
    // DEV_Delay_ms(5000);


    // 2.Drawing on the image
    Serial.println("Drawing:BlackImage");
    Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Serial.println("Paint_DrawLine");
    Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Serial.println("Paint_DrawRectangle");
    Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Serial.println("Paint_DrawCircle");
    Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Serial.println("Paint_DrawLine");
    Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Serial.println("Paint_DrawString_EN");
    Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
    Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
    Serial.println("Paint_DrawNum");
    Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
    Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);
    Serial.println("Paint_DrawString_EN");
    Paint_DrawString_EN(10, 150, "GRAY1 with black background", &Font24, BLACK, GRAY1);
    Paint_DrawString_EN(10, 175, "GRAY2 with white background", &Font24, WHITE, GRAY2);
    Paint_DrawString_EN(10, 200, "GRAY3 with white background", &Font24, WHITE, GRAY3);
    Paint_DrawString_EN(10, 225, "GRAY4 with white background", &Font24, WHITE, GRAY4);
    Serial.println("EPD_Display");
    EPD_3IN7_4Gray_Display(BlackImage);
    DEV_Delay_ms(4000);
#endif

#if 1                     // partial update, just 1 Gray mode

//  screen_display_param();
//
//    DEV_Delay_ms(5000);
    screen_update_clock();


    Paint_Clear(BLACK);
    DEV_Delay_ms(5000);
  /*
  Serial.println("show time, partial update, just 1 Gary mode\r\n");
  PAINT_TIME sPaint_time;
  sPaint_time.Hour = 12;
  sPaint_time.Min = 34;
  sPaint_time.Sec = 56;
  UBYTE num = 5;
  for (;;) {
    sPaint_time.Sec = sPaint_time.Sec + 1;
    if (sPaint_time.Sec == 60) {
      sPaint_time.Min = sPaint_time.Min + 1;
      sPaint_time.Sec = 0;
      if (sPaint_time.Min == 60) {
        sPaint_time.Hour = sPaint_time.Hour + 1;
        sPaint_time.Min = 0;
        if (sPaint_time.Hour == 24) {
          sPaint_time.Hour = 0;
          sPaint_time.Min = 0;
          sPaint_time.Sec = 0;
        }
      }
    }
    Serial.println("Paint_ClearWindows");
    Paint_ClearWindows(0, 0, EPD_3IN7_HEIGHT, EPD_3IN7_WIDTH, WHITE);
    Serial.println("Paint_DrawTime");
    Paint_DrawTime(0, 0, &sPaint_time, &Font20, BLACK, WHITE);

    num = num - 1;
    if (num == 0) {
      break;
    }

    Serial.println("Part refresh...\r\n");
    // EPD_3IN7_1Gray_Display(BlackImage);
    EPD_3IN7_1Gray_Display_Part(BlackImage, image_x_size, image_y_size, image_buf_size, 0, 50);
    DEV_Delay_ms(500);
  }
   */

#endif
  // EPD_3IN7_4Gray_Init();
  Serial.println("Clear...\r\n");
  // EPD_3IN7_4Gray_Clear();

  // Sleep & close 5V
  Serial.println("Goto Sleep...\r\n");
  // EPD_3IN7_Sleep();

  // free(BlackImage);
  // BlackImage = NULL;

  Serial.println("close 5V, Module enters 0 power consumption ...\r\n");
}

/* The main loop -------------------------------------------------------------*/
void loop() {
  //
}
