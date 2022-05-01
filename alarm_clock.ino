#include "NTP.h"
#include <RTCZero.h>
#include <WiFiNINA.h>
#include <stdlib.h>

#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "imagedata.h"
#include "screen.h"
#include "secret.h"

static RTCZero rtc;
static uint8_t alarm_match{0};
static WiFiUDP wifiUdp;
static NTP ntp(wifiUdp);

static void printWiFiStatus() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}

static void connect_to_wifi() {
    int status        = WL_IDLE_STATUS;
    const char ssid[] = WIFI_SSID_NAME;// your network SSID (name)
    const char pass[] = WIFI_PWD;

    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        do {
            delay(500);
        } while (status != WL_CONNECTED);
    }
    // you're connected now, so print out the status:
    printWiFiStatus();
}

static void wifi_init_ntp() {
    ntp.ruleDST(
            "CEST", Last, Sun, Mar, 2,
            120);// last sunday in march 2:00, timetone +120min (+1 GMT + 1h summertime offset)
    ntp.ruleSTD("CET", Last, Sun, Oct, 3,
                60);// last sunday in october 3:00, timezone +60min (+1 GMT)
    ntp.begin();
}

static void alarmMatch() {
    alarm_match = 1;
    rtc.enableAlarm(rtc.MATCH_SS);
}

void init_rtc() {
    const uint8_t hours{12};
    const uint8_t minutes{34};
    const uint8_t seconds{00};

    const uint8_t day{30};
    const uint8_t month{10};
    const uint8_t year{07};

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
    connect_to_wifi();
    wifi_init_ntp();

    DEV_Module_Init();

    init_screen();

    //  screen_display_param();

    screen_update_clock();
}

/* The main loop -------------------------------------------------------------*/
void loop() {
    if (alarm_match) {
        alarm_match = 0;

        ntp.update();
        Serial.print("Epoch received: ");
        Serial.println(ntp.epoch());
        Serial.print("hours: ");
        Serial.println(ntp.hours());
        rtc.setEpoch(ntp.epoch());
        rtc.setHours(ntp.hours());
        Serial.println();
        Serial.println(ntp.formattedTime("%d. %B %Y"));// dd. Mmm yyyy
        Serial.println(ntp.formattedTime("%A %T"));    // Www hh:mm:ss

        init_screen();
        screen_update_clock();

        EPD_3IN7_Sleep();

        rtc.setAlarmSeconds(0);
        rtc.attachInterrupt(alarmMatch);
        rtc.enableAlarm(rtc.MATCH_SS);
    }
}
