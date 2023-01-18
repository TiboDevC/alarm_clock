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

#define IDLE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

static StaticTask_t _idle_task_buffer;
static StackType_t  _idle_task_stack[IDLE_TASK_STACK_SIZE];

static void _idle_task(void *pvParameters)
{
	constexpr int period_ms     = 1000 / portTICK_PERIOD_MS;
	TickType_t    xLastWakeTime = xTaskGetTickCount();

	while (1) {
		xTaskDelayUntil(&xLastWakeTime, period_ms);
		if (serialEventRun) {
			serialEventRun();
		}
		Serial.print(".");
	}
}

void setup()
{
	/* Disable speaker first */
	pinMode(PIN_SWITCH_SPEAKER, OUTPUT);
	digitalWrite(PIN_SWITCH_SPEAKER, HIGH);

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

	/* Start IDLE task */
	xTaskCreateStatic(_idle_task,
	                  "IDLE task",
	                  IDLE_TASK_STACK_SIZE + 1,
	                  NULL,
	                  tskIDLE_PRIORITY,
	                  _idle_task_stack,
	                  &_idle_task_buffer);

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
