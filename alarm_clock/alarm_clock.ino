#include <Arduino.h>

#include <FreeRTOS.h>
#include <task.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "alarm_clock_fsm.h"
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

	if (!SerialUSB.available()) {
		/* Disable serial if not available */
		SerialUSB.end();
	}

	SerialUSB.println("");
	SerialUSB.println("******************************");
	SerialUSB.println("        Program start         ");
	SerialUSB.println("******************************");
	SerialUSB.flush();

	alarm_clock_fsm();
	vTaskStartScheduler();

	while (1) {
		SerialUSB.print(".");
		SerialUSB.println("Scheduler Failed! \n");
		SerialUSB.flush();
		delay(1000);
	}
}

void loop()
{
	SerialUSB.print(".");
	delay(1000);
}