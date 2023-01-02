#include <Arduino.h>

#include <FreeRTOS.h>
#include <task.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "fsm/alarm_clock_fsm.h"
#include "music/music.h"

#ifdef __cplusplus
}
#endif

void setup()
{
	/* Disable speaker first */
	pinMode(PIN_SWITCH_SPEAKER, OUTPUT);
	digitalWrite(PIN_SWITCH_SPEAKER, HIGH);

	SerialUSB.begin(115200);

	delay(1000); // prevents usb driver crash on startup, do not omit this

	if (!Serial.available()) {
		/* Disable serial if not available */
		Serial.end();
	}

	Serial.println("");
	Serial.println("******************************");
	Serial.println("        Program start         ");
	Serial.println("******************************");
	Serial.flush();

	alarm_clock_fsm();
	vTaskStartScheduler();

	while (1) {
		Serial.print(".");
		Serial.println("Scheduler Failed! \n");
		Serial.flush();
		delay(1000);
	}
}

void loop()
{
	Serial.print(".");
	delay(1000);
}