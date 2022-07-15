#include "NTP.h"
#include <RTCZero.h>
#include <WiFiNINA.h>
#include <stdlib.h>

#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "ZeroTimer.h"
#include "alarm_flash_storage.h"
#include "button.h"
#include "imagedata.h"
#include "music.h"
#include "screen.h"
#include "secret.h"

static RTCZero rtc;
static uint8_t alarm_match{0};
static WiFiUDP wifiUdp;
static NTP ntp(wifiUdp);

static uint64_t last_ntp_update{};

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

static int connect_to_wifi() {
    int status                = WL_IDLE_STATUS;
    const char ssid[]         = WIFI_SSID_NAME;// your network SSID (name)
    const char pass[]         = WIFI_PWD;
    const uint8_t max_num_try = 20;
    uint8_t num_try           = 0;

    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    do {
        delay(500);
        num_try++;
    } while (status != WL_CONNECTED and num_try < max_num_try);

    if (status == WL_CONNECTED) {
        printWiFiStatus();
        return 0;
    }
    return -1;
}

static void disconnect_wifi() {
    WiFi.disconnect();
}

static void init_ntp() {
    ntp.ruleDST(
            "CEST", Last, Sun, Mar, 2,
            120);// last sunday in march 2:00, timetone +120min (+1 GMT + 1h summertime offset)
    ntp.ruleSTD("CET", Last, Sun, Oct, 3,
                60);// last sunday in october 3:00, timezone +60min (+1 GMT)
    ntp.begin();
}

static void deinit_ntp() {
    ntp.stop();
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

void message() {
    Serial.println("hello");
}

void setup() {
    Serial.begin(115200);
    delay(7000);
    Serial.println("Staring");

    init_buttons();
    init_alarm_flash_storage();
    init_rtc();
    // music_init();
    // music_play();

    //    DEV_Module_Init();
    //    init_screen();
}

/* The main loop -------------------------------------------------------------*/
void loop() {
    delay(50);
    //    check_buttons();

    if (alarm_match) {
        alarm_match = 0;

        /* Update NTP time at 5am */
        if ((rtc.getHours() == 5 and rtc.getMinutes() == 0) or
            (rtc.getEpoch() - last_ntp_update > (3600 * 24))) {
            if (connect_to_wifi() == 0) {
                init_ntp();

                ntp.update();
                Serial.print("Epoch received: ");
                Serial.println(ntp.epoch());
                Serial.print("hours: ");
                Serial.println(ntp.hours());
                rtc.setEpoch(ntp.epoch());
                rtc.setHours(ntp.hours());
                last_ntp_update = rtc.getEpoch();
                Serial.println();
                Serial.println(ntp.formattedTime("%d. %B %Y"));// dd. Mmm yyyy
                Serial.println(ntp.formattedTime("%A %T"));    // Www hh:mm:ss
                deinit_ntp();
                disconnect_wifi();
                ui_update();
            } else {
                Serial.println("Fail updating epoch");
            }
        } else {
            /* Update screen every 5 minutes */
            if (rtc.getMinutes() % 5 == 0) {
                ui_update();
            }
        }


        rtc.setAlarmSeconds(0);
        rtc.attachInterrupt(alarmMatch);
        rtc.enableAlarm(rtc.MATCH_SS);

        //        rtc.standbyMode();
    }
}
