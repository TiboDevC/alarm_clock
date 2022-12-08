#include <RTCZero.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "alarm_clock_fsm.h"
#include "rtc_tool.h"

#ifdef __cplusplus
}
#endif


static RTCZero rtc;

void _rtc_alarm_cb()
{
	SerialUSB.println("Alarm match!");
	alarm_clock_fsm_dispatch_event(FSM_EVENT_RTC_WAKE_UP);
}

void init_rtc(void)
{
	SerialUSB.println("Init rtc");
	const uint8_t hours{12};
	const uint8_t minutes{34};
	const uint8_t seconds{00};

	const uint8_t day{30};
	const uint8_t month{10};
	const uint8_t year{07};

	rtc.begin();

	/* Set the time */
	rtc.setHours(hours);
	rtc.setMinutes(minutes);
	rtc.setSeconds(seconds);

	/* Set the date */
	rtc.setDay(day);
	rtc.setMonth(month);
	rtc.setYear(year);
}

void rtc_start_interrupt(void)
{
	/* Wake up device every seconds */
	rtc.setAlarmSeconds(0);
	rtc.attachInterrupt(_rtc_alarm_cb);
	rtc.enableAlarm(rtc.MATCH_SS);
}


void rtc_set_epoch(uint64_t epoch)
{
	rtc.setEpoch(epoch);
}

void rtc_set_hours(uint8_t hours)
{
	rtc.setHours(hours);
}

uint64_t rtc_get_epoch()
{
	return rtc.getEpoch();
}

uint8_t rtc_get_day()
{
	return rtc.getDay();
}

uint8_t rtc_get_month()
{
	return rtc.getMonth();
}

uint8_t rtc_get_year()
{
	return rtc.getYear();
}

uint8_t rtc_get_hours()
{
	return rtc.getHours();
}

uint8_t rtc_get_minutes()
{
	return rtc.getMinutes();
}

uint8_t rtc_get_seconds()
{
	return rtc.getSeconds();
}

enum rtc_days_t rtc_get_weekday()
/* Calculate day of week in proleptic Gregorian calendar. Sunday == 0. */
{
	int adjustment, mm, yy;
	adjustment = (14 - rtc.getMonth()) / 12;
	mm         = rtc.getMonth() + 12 * adjustment - 2;
	yy         = rtc.getYear() - adjustment;
	return static_cast<rtc_days_t>(
	    (rtc.getDay() + (13 * mm - 1) / 5 + yy + yy / 4 - yy / 100 + yy / 400) % 7);
}
