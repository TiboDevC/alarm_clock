#include "alarm_clock_fsm.h"

#include "screen.h"

enum fsm_handler_rc state_settings(struct fsm *fsm, struct fsm_event const *event)
{
	enum alarm_clock_fsm_event_type event_alarm_clock = event->type & ALARM_CLOCK_FSM_EVT_MSK;

	switch (event_alarm_clock) {
	case FSM_EVENT_ENTRY:
		/* Enter in clock screen, update screen status */
		ui_set_state(menu_settings);
		return FSM_HANDLED();
	default:
		break;
	}

	return FSM_IGNORED();
}
