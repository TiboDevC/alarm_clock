#ifndef ALARM_CLOCK_FSM_H
#define ALARM_CLOCK_FSM_H

#include <FreeRTOS.h>
#include <semphr.h>

#include "fsm.h"

#define ALARM_CLOCK_FSM_EVT_MSK 0xFFFF /* 2 bytes for the event, rest for data */
#define ALARM_CLOCK_FSM_EVT_SHT 16

enum alarm_clock_fsm_event_type {
	FSM_EVENT_BUTTON = FSM_EVENT_APPLICATION,
	FSM_EVENT_ALARM_FIRES,
	FSM_EVENT_RTC_WAKE_UP,
};

int  alarm_clock_fsm_dispatch_event(enum alarm_clock_fsm_event_type event_type);
void alarm_clock_fsm(void);

/* State event handlers */
enum fsm_handler_rc state_init(struct fsm *fsm, struct fsm_event const *event);
enum fsm_handler_rc state_clock_display(struct fsm *fsm, struct fsm_event const *event);
enum fsm_handler_rc state_settings(struct fsm *fsm, struct fsm_event const *event);
enum fsm_handler_rc state_alarm_ongoing(struct fsm *fsm, struct fsm_event const *event);

#endif /* ALARM_CLOCK_FSM_H */
