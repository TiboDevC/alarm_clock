#include "alarm_clock_fsm.h"

#include "button/button.h"
#include "debug.h"
#include "flash_storage/alarm_flash_storage.h"
#include "rtc/rtc_tool.h"
#include "ui/screen.h"
#include "wifi/wifi_tool.h"

#define SCREEN_REFRESH_FREQ_MIN             5                  /* Refresh screen every x minutes */
#define SCREEN_WIFI_EPOCH_TIME_INTERVAl_SEC (60 * 60 * 24 * 8) /* Perform wifi sync every x sec max */

#define STATE_CLOCK_DISPLAY_INFO(x...) debug("[state_clock_display] " x)

static uint64_t _last_epoch_refresh_time;
static uint64_t _last_wifi_epoch_sync_time = 0;

static int is_alarm_ringing(void)
{
	const struct alarm_params_t alarm_0 = get_alarm_0();

	const enum rtc_days_t rtc_day    = rtc_get_weekday();
	const uint8_t         rtc_hour   = rtc_get_hours();
	const uint8_t         rtc_minute = rtc_get_minutes();

	if (!alarm_0.is_set) {
		return -1;
	}
	if (rtc_hour != alarm_0.alarm_hour) {
		return -1;
	}
	if (!(rtc_minute >= alarm_0.alarm_minute && rtc_minute <= alarm_0.alarm_minute + 3)) {
		return -1;
	}
	switch (rtc_day) {
	case rtc_monday:
		if (!alarm_0.alarm_days.days.monday) {
			return -1;
		}
		break;
	case rtc_tuesday:
		if (!alarm_0.alarm_days.days.tuesday) {
			return -1;
		}
		break;
	case rtc_wednesday:
		if (!alarm_0.alarm_days.days.wednesday) {
			return -1;
		}
		break;
	case rtc_thursday:
		if (!alarm_0.alarm_days.days.thursday) {
			return -1;
		}
		break;
	case rtc_friday:
		if (!alarm_0.alarm_days.days.friday) {
			return -1;
		}
		break;
	case rtc_saturday:
		if (!alarm_0.alarm_days.days.saturday) {
			return -1;
		}
		break;
	case rtc_sunday:
		if (!alarm_0.alarm_days.days.sunday) {
			return -1;
		}
		break;
	}
	if (alarm_0.rings_tomorrow) {
		return -1;
	}
	return 0;
}

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

static void _wifi_sync_epoch(void)
{
	int      need_wifi_sync = 0;
	uint64_t epoch;
	epoch = rtc_get_epoch();

	if (epoch - _last_wifi_epoch_sync_time > SCREEN_WIFI_EPOCH_TIME_INTERVAl_SEC) {
		need_wifi_sync = 1;
	} else {
		if (epoch - _last_wifi_epoch_sync_time < (60 * 60) && rtc_get_weekday() == rtc_sunday &&
		    rtc_get_hours() == 3) {
			/* Refresh every sunday at 3h */
			need_wifi_sync = 1;
		}
	}
	if (need_wifi_sync == 1) {
		_last_wifi_epoch_sync_time = epoch;
		wifi_update_rtc();
		ui_update();
	}
}

enum fsm_handler_rc state_clock_display(struct fsm *fsm, struct fsm_event const *event)
{
	enum alarm_clock_fsm_event_type event_alarm_clock = event->type & ALARM_CLOCK_FSM_EVT_MSK;
	uint64_t                        epoch;

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
		if (is_alarm_ringing() == 0) {
			return FSM_TRANSITION(&state_alarm_ongoing);
		}
		_wifi_sync_epoch();
		return FSM_HANDLED();
	case FSM_EVENT_BUTTON:
		return _handle_button(fsm, event->type);
	default:
		break;
	}

	return FSM_IGNORED();
}
