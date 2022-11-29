#include "alarm_clock_fsm.h"

#include "debug.h"
#include "music.h"

#define STATE_CLOCK_DISPLAY_INFO(x...) debug("[state_alarm_ongoing] " x)

enum fsm_handler_rc state_alarm_ongoing(struct fsm *fsm, struct fsm_event const *event)
{
	enum alarm_clock_fsm_event_type event_alarm_clock = event->type & ALARM_CLOCK_FSM_EVT_MSK;

	switch (event_alarm_clock) {
	case FSM_EVENT_ENTRY:
		STATE_CLOCK_DISPLAY_INFO("Ring alarm!\n");
		if (music_init() == 0) {
			music_play();
		} else {
			STATE_CLOCK_DISPLAY_INFO("Alarm play failed\n");
		}
		return FSM_HANDLED();
	case FSM_EVENT_EXIT:
		return FSM_HANDLED();
	case FSM_EVENT_BUTTON:
		/* If a button is pushed, stop the alarm */
		music_stop();
		music_deinit();
		return FSM_TRANSITION(&state_clock_display);
	default:
		break;
	}

	return FSM_IGNORED();
}
