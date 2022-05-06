#include "NTP.h"
#include <RTCZero.h>
#include <WiFiNINA.h>
#include <stdlib.h>

#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "alarm_flash_storage.h"
#include "imagedata.h"
#include "screen.h"
#include "secret.h"

#define NUM_BUTTON 11
#define BUTTON_0 0
#define BUTTON_1 6
#define BUTTON_2 11
#define BUTTON_3 12
#define BUTTON_4 13
#define BUTTON_5 14
#define BUTTON_6 16
#define BUTTON_7 17
#define BUTTON_8 18
#define BUTTON_9 19
#define BUTTON_10 20

#define BUTTON_PRESSED_EVENT 1
#define BUTTON_PRESSED_EVENT_CLR 0

static uint8_t buttons_state[NUM_BUTTON];
static constexpr uint8_t buttons_pin[NUM_BUTTON]{
        BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5,
        BUTTON_6, BUTTON_7, BUTTON_8, BUTTON_9, BUTTON_10};
static uint8_t buttons_event[NUM_BUTTON];

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

static void check_buttons() {
    for (uint8_t button = 0; button < NUM_BUTTON; button++) {
        const uint8_t button_state = digitalRead(buttons_pin[button]);
        if (buttons_state[button] != button_state and button_state == LOW) {
            buttons_event[button] = BUTTON_PRESSED_EVENT;
            Serial.print("Button ");
            Serial.print(button);
            Serial.println(" pressed");
        }
        buttons_state[button] = button_state;
    }
}

static void init_buttons() {
    for (uint8_t button = 0; button < NUM_BUTTON; button++) {
        pinMode(buttons_pin[button], INPUT_PULLDOWN);
        delay(500);
        Serial.println(digitalRead(buttons_pin[button]));
    }

    memset(buttons_state, LOW, sizeof(buttons_state));
}

void setup() {
    Serial.begin(115200);
    delay(5000);
    Serial.println("Staring");

    init_buttons();
    init_alarm_flash_storage();
    init_rtc();

    DEV_Module_Init();

    init_screen();

    //  screen_display_param();

    screen_update_clock();
}

/* The main loop -------------------------------------------------------------*/
void loop() {
    delay(50);
    check_buttons();
    if (buttons_event[7] == BUTTON_PRESSED_EVENT) {
        buttons_event[7] = BUTTON_PRESSED_EVENT_CLR;
        ui_set_state(menu_clock);
        ui_update();
    }
    if (buttons_event[6] == BUTTON_PRESSED_EVENT) {
        buttons_event[6] = BUTTON_PRESSED_EVENT_CLR;
        ui_set_state(menu_settings);
        ui_update();
    }

    if (alarm_match) {
        alarm_match = 0;

        /* Update NTP time at 5am */
        if ((rtc.getHours() == 5 and rtc.getMinutes() == 0) or
            (rtc.getEpoch() - last_ntp_update > (3600 * 24))) {
            if (connect_to_wifi() != 0) {
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
    }
}
