#include <assert.h>

#include <Arduino.h>

#include "alarm_clock_fsm.h"

#include "FreeRTOS.h"
#include "queue.h"

#include "debug.h"

static QueueHandle_t event_queue = NULL;
static struct fsm    fsm;

int alarm_clock_fsm_dispatch_event(enum alarm_clock_fsm_event_type event_type)
{
	struct fsm_event event = {.type = event_type};

	if (event_queue == NULL) {
		return -1;
	}

	if (xQueueSend(event_queue, (void *) &event, 10) != pdPASS) {
		assert(0);
		return -1;
	}

	return 0;
}

static char *alarm_clock_fsm_state_name_getter(const fsm_event_handler_t state)
{
	if ((void *) state == (void *) state_init) {
		return "state_init";
	} else if ((void *) state == (void *) state_clock_display) {
		return "state_clock_display";
	} else if ((void *) state == (void *) state_settings) {
		return "state_settings";
	} else if ((void *) state == (void *) state_alarm_ongoing) {
		return "state_alarm_ongoing";
	} else {
		return "STATE_UNKNOWN";
	}
}

static char *alarm_clock_fsm_event_name_getter(const struct fsm_event *event)
{
	switch ((event->type) & ALARM_CLOCK_FSM_EVT_MSK) {
	case FSM_EVENT_ENTRY:
		return "ENTRY";
	case FSM_EVENT_EXIT:
		return "EXIT";
	case FSM_EVENT_INIT:
		return "INIT";
	case FSM_EVENT_BUTTON:
		return "FSM_EVENT_BUTTON";
	case FSM_EVENT_ALARM_FIRES:
		return "FSM_EVENT_ALARM_FIRES";
	case FSM_EVENT_RTC_WAKE_UP:
		return "FSM_EVENT_RTC_WAKE_UP";
	default:
		return "UNKNOWN_EVT";
	}
}

static void _fsm_task(void *pvParameters)
{
	struct fsm_event fsm_event;

	while (1) {
		xQueueReceive(event_queue, &fsm_event, portMAX_DELAY);
		fsm_dispatch(&fsm, &fsm_event);
	}

	vQueueDelete(event_queue);
}


#define FSM_TASK_STACK_SIZE 600

static StaticTask_t _fsm_task_buffer;
static StackType_t  _fsm_stack[FSM_TASK_STACK_SIZE];
static TaskHandle_t _fsm_task_handler = NULL;

void alarm_clock_fsm(void)
{
	debug("[fsm] Starting FSM!\n");

	fsm.event_name_getter = NULL;
	fsm.event_name_getter = NULL;

	/* Create event queue and run event loop */
	event_queue = xQueueCreate(8, sizeof(struct fsm_event));
	if (event_queue == NULL) {
		debug("[fsm] Could not create event queue for ALARM_CLOCK_FSM\n");
		assert(0);
		return;
	}


	fsm_init(&fsm, "ALARM_CLOCK_FSM", (fsm_event_handler_t) &state_init, &fsm_init_event);

	/* Configure tracing on state transitions and events */
	fsm.state_name_getter = alarm_clock_fsm_state_name_getter;
	fsm.trace_transitions = 1;
	fsm.event_name_getter = alarm_clock_fsm_event_name_getter;
	fsm.trace_events      = 1;

	_fsm_task_handler = xTaskCreateStatic(_fsm_task,
	                                      "Screen refresh",
	                                      FSM_TASK_STACK_SIZE,
	                                      NULL,
	                                      tskIDLE_PRIORITY + 2,
	                                      _fsm_stack,
	                                      &_fsm_task_buffer);
	if (_fsm_task_handler == NULL) {
		debug("Could not create FSM task!\n");
	}
}
