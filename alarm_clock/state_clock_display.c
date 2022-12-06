#include "alarm_clock_fsm.h"

#include "button.h"
#include "debug.h"
#include "rtc_tool.h"
#include "screen.h"

#define SCREEN_REFRESH_FREQ_MIN 5 /* Refresh screen every x minutes */

#define STATE_CLOCK_DISPLAY_INFO(x...) debug("[state_clock_display] " x)

static uint64_t _last_epoch_refresh_time;

static enum fsm_handler_rc _handle_button(struct fsm *fsm, uint32_t evt)
{
	struct button_evt_t evt_button;
	evt_button.button_id  = (evt >> BUTTON_ID_SHIFT) & BUTTON_ID_MSK;
	evt_button.action     = (evt >> BUTTON_ACTION_SHIFT) & BUTTON_ACTION_MSK;
	evt_button.push_count = (evt >> BUTTON_COUNT_SHIFT) & BUTTON_COUNT_MSK;

	STATE_CLOCK_DISPLAY_INFO("Receive button: ");
	debug_num(evt_button.button_id);
	debug(", action: ");
	debug_num(evt_button.action);
	debug(", push_count: ");
	debug_num(evt_button.push_count);
	debug("\n");

	if (evt_button.action == LONG_PRESS) {
		if (evt_button.button_id == B_MENU_SETTINGS) {
			return FSM_TRANSITION(&state_settings);
		}
	} else if (evt_button.action == SHORT_PRESS) {
		/* Dispatch event to screen */
		ui_button_event(&evt_button);
	}
	return FSM_HANDLED();
}

enum fsm_handler_rc state_clock_display(struct fsm *fsm, struct fsm_event const *event)
{
	enum alarm_clock_fsm_event_type event_alarm_clock = event->type & ALARM_CLOCK_FSM_EVT_MSK;
	uint64_t epoch;

	switch (event_alarm_clock) {
	case FSM_EVENT_ENTRY:
		/* Enter in clock screen, update screen status */
		ui_set_state(menu_clock);
		ui_update();
		return FSM_HANDLED();
	case FSM_EVENT_RTC_WAKE_UP:
		epoch = rtc_get_epoch();
		if (rtc_get_minutes() % SCREEN_REFRESH_FREQ_MIN == 0 ||
		    (epoch - _last_epoch_refresh_time > SCREEN_REFRESH_FREQ_MIN * 60)) {
			STATE_CLOCK_DISPLAY_INFO("Update screen\n");
			_last_epoch_refresh_time = epoch;
			ui_update();
		} else {
			STATE_CLOCK_DISPLAY_INFO("Do not update screen yet\n");
		}
		return FSM_HANDLED();
	case FSM_EVENT_BUTTON:
		return _handle_button(fsm, event->type);
	default:
		break;
	}

	return FSM_IGNORED();
}
