#include <Arduino.h>

#include "watchdog/watchdog.h"
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

#define IDLE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

static StaticTask_t _wdt_task_buffer;
static StackType_t  _wdt_task_stack[IDLE_TASK_STACK_SIZE];

static void _wdt_task(void *pvParameters)
{
	while (1) {
		vTaskDelay(pdMS_TO_TICKS(500));
		wdt_refresh();
		Serial.print(".");
	}
}

void setup()
{
	/* Disable speaker first */
	pinMode(PIN_SWITCH_SPEAKER, OUTPUT);
	digitalWrite(PIN_SWITCH_SPEAKER, LOW);

	pinMode(A0, OUTPUT);
	analogWrite(A0, 0);

	SerialUSB.begin(115200);

	delay(3000); // prevents usb driver crash on startup, do not omit this

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

	/* Start IDLE task */
	xTaskCreateStatic(_wdt_task,
	                  "IDLE task",
	                  IDLE_TASK_STACK_SIZE,
	                  NULL,
	                  tskIDLE_PRIORITY,
	                  _wdt_task_stack,
	                  &_wdt_task_buffer);

	Serial.println("end alarm_init, start scheduler");
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
	Serial.println("***************** ERROR");
	/* Should never reach this code */
	configASSERT(0)
}
