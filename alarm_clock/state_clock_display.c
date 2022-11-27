#include "alarm_clock_fsm.h"

#include "debug.h"
#include "screen.h"
#include "rtc_tool.h"

#define SCREEN_REFRESH_FREQ_MIN 2 /* Refresh screen every x minutes */

static uint64_t _last_epoch_refresh_time;

enum fsm_handler_rc state_clock_display(struct fsm *fsm, struct fsm_event const *event)
{
	enum alarm_clock_fsm_event_type event_alarm_clock = event->type & ALARM_CLOCK_FSM_EVT_MSK;

	switch (event_alarm_clock) {
	case FSM_EVENT_ENTRY:
		/* Enter in clock screen, update screen status */
		ui_set_state(menu_clock);
		return FSM_HANDLED();
	case FSM_EVENT_RTC_WAKE_UP:
		if ((rtc_get_minutes() % SCREEN_REFRESH_FREQ_MIN == 0 && rtc_get_seconds() < 20) ||
		    ((_last_epoch_refresh_time - rtc_get_epoch()) > SCREEN_REFRESH_FREQ_MIN * 60)) {
			_last_epoch_refresh_time =  rtc_get_epoch();
			debug("Update screen\n");
			ui_update();
		} else {
			debug("Do not update screen\n");
		}
		return FSM_HANDLED();
	default:
		break;
	}

	return FSM_IGNORED();
}
