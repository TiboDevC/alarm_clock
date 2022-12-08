#include "alarm_clock_fsm.h"

#include "button.h"
#include "debug.h"
#include "drv/screen/EPD_3in7.h"
#include "rtc_tool.h"
#include "ui/screen.h"

#define SETTING_SCREEN_TIMEOUT_SEC 60

static uint64_t _settings_entry_time;

static enum fsm_handler_rc _handle_button(struct fsm *fsm, uint32_t evt)
{
	struct button_evt_t evt_button;
	evt_button.button_id  = (evt >> BUTTON_ID_SHIFT) & BUTTON_ID_MSK;
	evt_button.action     = (evt >> BUTTON_ACTION_SHIFT) & BUTTON_ACTION_MSK;
	evt_button.push_count = (evt >> BUTTON_COUNT_SHIFT) & BUTTON_COUNT_MSK;

	if (evt_button.action == LONG_PRESS && evt_button.button_id == B_MENU_CLOCK) {
		return FSM_TRANSITION(&state_clock_display);
	}
	ui_button_event(&evt_button);
	ui_update();
	return FSM_HANDLED();
}

enum fsm_handler_rc state_settings(struct fsm *fsm, struct fsm_event const *event)
{
	enum alarm_clock_fsm_event_type event_alarm_clock = event->type & ALARM_CLOCK_FSM_EVT_MSK;

	switch (event_alarm_clock) {
	case FSM_EVENT_ENTRY:
		/* Enter in clock screen, update screen status */
		ui_set_state(menu_settings);
		init_screen();
		ui_update();
		button_start_polling();
		_settings_entry_time = rtc_get_epoch();
		return FSM_HANDLED();
	case FSM_EVENT_EXIT:
		button_stop_polling();
		EPD_3IN7_Sleep();
		return FSM_HANDLED();
	case FSM_EVENT_BUTTON:
		_settings_entry_time = rtc_get_epoch();
		return _handle_button(fsm, event->type);
	case FSM_EVENT_RTC_WAKE_UP:
		if (rtc_get_epoch() - _settings_entry_time > SETTING_SCREEN_TIMEOUT_SEC) {
			debug("[state_settings] Timeout, return to clock display\n");
			return FSM_TRANSITION(&state_clock_display);
		}
		return FSM_HANDLED();
	default:
		break;
	}

	return FSM_IGNORED();
}
