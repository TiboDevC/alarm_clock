#include <RTCZero.h>
#include <stdlib.h>

#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "imagedata.h"
#include "screen.h"

static void alarmMatch() {
    RTCZero rtc;

    init_screen();
    screen_update_clock();

    EPD_3IN7_Sleep();

    rtc.setAlarmSeconds(0);
    rtc.attachInterrupt(alarmMatch);
    rtc.enableAlarm(rtc.MATCH_SS);
}

void init_rtc() {
    RTCZero rtc;

    const uint8_t hours{23};
    const uint8_t minutes{56};
    const uint8_t seconds{00};

    const uint8_t day{1};
    const uint8_t month{05};
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

    rtc.setAlarmSeconds(0);
    rtc.attachInterrupt(alarmMatch);
    rtc.enableAlarm(rtc.MATCH_SS);
}

void setup() {
    Serial.begin(115200);
    delay(5000);

    init_rtc();

    DEV_Module_Init();

    init_screen();


    //  screen_display_param();

    screen_update_clock();
}

/* The main loop -------------------------------------------------------------*/
void loop() {}
