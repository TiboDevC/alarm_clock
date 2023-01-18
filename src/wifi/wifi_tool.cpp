#include "Arduino.h"
#include <WiFiNINA.h>

#include "NTP.h"
#include "wifi_credentials.h"

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
#warning "wifi_credentials.h must be created and define WIFI_SSID_NAME and WIFI_PWD"
#endif /* WIFI_SSID_NAME */

static WiFiUDP wifiUdp;
static NTP     ntp(wifiUdp);

static void print_connection_status()
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

static void _print_wifi_status(uint8_t status)
{
	Serial.print("Wifi status: ");
	switch (status) {
	case WL_NO_MODULE:
		Serial.println("WL_NO_MODULE");
		break;
	case WL_IDLE_STATUS:
		Serial.println("WL_IDLE_STATUS");
		break;
	case WL_NO_SSID_AVAIL:
		Serial.println("WL_NO_SSID_AVAIL");
		break;
	case WL_SCAN_COMPLETED:
		Serial.println("WL_SCAN_COMPLETED");
		break;
	case WL_CONNECTED:
		Serial.println("WL_CONNECTED");
		break;
	case WL_CONNECT_FAILED:
		Serial.println("WL_CONNECT_FAILED");
		break;
	case WL_CONNECTION_LOST:
		Serial.println("WL_CONNECTION_LOST");
		break;
	case WL_DISCONNECTED:
		Serial.println("WL_DISCONNECTED");
		break;
	case WL_AP_LISTENING:
		Serial.println("WL_AP_LISTENING");
		break;
	case WL_AP_CONNECTED:
		Serial.println("WL_AP_CONNECTED");
		break;
	case WL_AP_FAILED:
		Serial.println("WL_AP_FAILED");
		break;
	default:
		Serial.print("Unknown: ");
		Serial.println(status);
	}
}

static int connect_to_wifi()
{
	int           status      = WL_IDLE_STATUS;
	const char    ssid[]      = WIFI_SSID_NAME; /* your network SSID (name) */
	const char    pass[]      = WIFI_PWD;
	const uint8_t max_num_try = 2;
	uint8_t       num_try     = 0;

	Serial.print("Attempting to connect to SSID: ");
	Serial.println(ssid);
	do {
		Serial.print("Attempt: ");
		Serial.println(num_try);
		WiFi.begin(ssid, pass);
		delay(10000);
		status = WiFi.status();
		_print_wifi_status(status);
		num_try++;
	} while (status != WL_CONNECTED and status != WL_NO_SHIELD and num_try < max_num_try);

	digitalWrite(LED_BUILTIN, LOW);
	if (status == WL_CONNECTED) {
		print_connection_status();
		return 0;
	} else if (status == WL_NO_SHIELD) {
		Serial.println("No wifi chip detected!");
		display_error(0);
	} else {
		display_error(1);
	}
	Serial.println("Could not connect to Wifi");
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
		Serial.print("Epoch received: ");
		Serial.println(static_cast<long unsigned int>(ntp.epoch()));
		Serial.print("hours: ");
		Serial.println(ntp.hours());
		rtc_set_epoch(ntp.epoch());
		rtc_set_hours(ntp.hours());
		Serial.println();
		Serial.println(ntp.formattedTime("%d. %B %Y")); /* dd. Mmm yyyy */
		Serial.println(ntp.formattedTime("%A %T"));     /* Www hh:mm:ss */
		deinit_ntp();
		return 0;
	} else {
		Serial.println("Fail updating epoch");
	}
	disconnect_wifi();
	return -1;
}
