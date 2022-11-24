#include "Arduino.h"
#include <WiFiNINA.h>

#include "NTP.h"
#include "secret.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rtc_tool.h"
#include "wifi_tool.h"

#ifdef __cplusplus
}
#endif

static WiFiUDP wifiUdp;
static NTP     ntp(wifiUdp);

static uint64_t last_ntp_update{};

static void printWiFiStatus()
{
	/* print the SSID of the network you're attached to: */
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());
	/* print your WiFi shield's IP address: */
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);
	/* print the received signal strength: */
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}

static int connect_to_wifi()
{
	int           status      = WL_IDLE_STATUS;
	const char    ssid[]      = WIFI_SSID_NAME; /* your network SSID (name) */
	const char    pass[]      = WIFI_PWD;
	const uint8_t max_num_try = 5;
	uint8_t       num_try     = 0;

	WiFi.end();

	Serial.print("Attempting to connect to SSID: ");
	Serial.println(ssid);
	do {
		status = WiFi.begin(ssid, pass);
		delay(1000);
		Serial.println(WiFi.status());
		num_try++;
	} while (status != WL_CONNECTED and num_try < max_num_try);

	if (status == WL_CONNECTED) {
		printWiFiStatus();
		return 0;
	}
	return -1;
}

static void disconnect_wifi()
{
	WiFi.disconnect();
}

static void init_ntp()
{
	ntp.ruleDST("CEST",
	            Last,
	            Sun,
	            Mar,
	            2,
	            120); /* last sunday in march 2:00, timetone +120min (+1 GMT + 1h summertime offset) */
	ntp.ruleSTD("CET", Last, Sun, Oct, 3, 60); /* last sunday in october 3:00, timezone +60min (+1 GMT) */
	ntp.begin();
}

static void deinit_ntp()
{
	ntp.stop();
}


int wifi_update_rtc(void)
{
	if (connect_to_wifi() == 0) {
		init_ntp();

		ntp.update();
		Serial.print("Epoch received: ");
		Serial.println(static_cast<long unsigned int>(ntp.epoch()));
		Serial.print("hours: ");
		Serial.println(ntp.hours());
		rtc_set_epoch(ntp.epoch());
		rtc_set_hours(ntp.hours());
		last_ntp_update = rtc_get_epoch();
		Serial.println();
		Serial.println(ntp.formattedTime("%d. %B %Y")); /* dd. Mmm yyyy */
		Serial.println(ntp.formattedTime("%A %T"));     /* Www hh:mm:ss */
		deinit_ntp();
		disconnect_wifi();
		return 0;
	} else {
		Serial.println("Fail updating epoch");
	}
	return -1;
}
