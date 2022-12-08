#include "Arduino.h"
#include <WiFiNINA.h>

#include "NTP.h"
#include "secret.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rtc/rtc_tool.h"
#include "ui/screen.h"
#include "wifi_tool.h"

#ifdef __cplusplus
}
#endif

#if !defined(WIFI_SSID_NAME) || !defined(WIFI_PWD)
#warning "secret.h must be created and define WIFI_SSID_NAME and WIFI_PWD"
#endif /* WIFI_SSID_NAME */

static WiFiUDP wifiUdp;
static NTP     ntp(wifiUdp);

static uint64_t last_ntp_update{};

static void printWiFiStatus()
{
	/* print the SSID of the network you're attached to: */
	SerialUSB.print("SSID: ");
	SerialUSB.println(WiFi.SSID());
	/* print your WiFi shield's IP address: */
	IPAddress ip = WiFi.localIP();
	SerialUSB.print("IP Address: ");
	SerialUSB.println(ip);
	/* print the received signal strength: */
	long rssi = WiFi.RSSI();
	SerialUSB.print("signal strength (RSSI):");
	SerialUSB.print(rssi);
	SerialUSB.println(" dBm");
}

static int connect_to_wifi()
{
	int           status      = WL_IDLE_STATUS;
	const char    ssid[]      = WIFI_SSID_NAME; /* your network SSID (name) */
	const char    pass[]      = WIFI_PWD;
	const uint8_t max_num_try = 5;
	uint8_t       num_try     = 0;

	SerialUSB.print("Attempting to connect to SSID: ");
	SerialUSB.println(ssid);
	do {
		SerialUSB.print("Attempt: ");
		SerialUSB.println(num_try);
		WiFi.begin(ssid, pass);
		delay(1000);
		status = WiFi.status();
		SerialUSB.println(status);
		num_try++;
	} while (status != WL_CONNECTED and status != WL_NO_SHIELD and num_try < max_num_try);

	if (status == WL_CONNECTED) {
		printWiFiStatus();
		return 0;
	} else if (status == WL_NO_SHIELD) {
		SerialUSB.println("No wifi chip detected!");
		display_error(0);
	} else {
		display_error(1);
	}
	SerialUSB.println("Could not connect to Wifi");
	return -1;
}

static void disconnect_wifi()
{
	WiFi.disconnect();
	WiFi.end();
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
		SerialUSB.print("Epoch received: ");
		SerialUSB.println(static_cast<long unsigned int>(ntp.epoch()));
		SerialUSB.print("hours: ");
		SerialUSB.println(ntp.hours());
		rtc_set_epoch(ntp.epoch());
		rtc_set_hours(ntp.hours());
		last_ntp_update = rtc_get_epoch();
		SerialUSB.println();
		SerialUSB.println(ntp.formattedTime("%d. %B %Y")); /* dd. Mmm yyyy */
		SerialUSB.println(ntp.formattedTime("%A %T"));     /* Www hh:mm:ss */
		deinit_ntp();
		return 0;
	} else {
		SerialUSB.println("Fail updating epoch");
	}
	disconnect_wifi();
	return -1;
}
