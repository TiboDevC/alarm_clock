#include <RTCZero.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "button.h"
#include "drv/screen/DEV_Config.h"
#include "drv/screen/EPD.h"
#include "flash_storage/alarm_flash_storage.h"
#include "rtc_tool.h"
#include "screen.h"
#include "ui/GUI_Paint.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#define SCREEN_INFO(x...) SerialUSB.print("[screen]" x)

static constexpr uint32_t
    image_size(const uint16_t x_size, const uint16_t y_size, const grey_scale_t grey_scale)
{
	return (x_size * y_size) / ((uint32_t) grey_scale);
}

// Create a new image cache
static constexpr uint16_t image_x_size{8 * 35};
static_assert(image_x_size % 8 == 0, "");
static constexpr uint16_t     image_y_size{480};
static constexpr grey_scale_t image_grey_scale{grey_scale_2};
static constexpr uint32_t     image_buf_size{image_size(image_x_size, image_y_size, image_grey_scale)};
static constexpr uint16_t     rotate{270};
static constexpr uint8_t      initial_background_color{BLACK};

static uint8_t _blackImage[image_buf_size];

void init_screen()
{
	SerialUSB.println("Init screen\n");

	EPD_3IN7_1Gray_Init();
	EPD_3IN7_1Gray_Clear();

	Paint_NewImage(_blackImage, image_x_size, image_y_size, rotate, initial_background_color);

	Paint_SelectImage(_blackImage);
	Paint_SetScale(2);
	Paint_Clear(BLACK);
}

static void draw_days_alarm(const uint16_t start_x,
                            const uint16_t start_y,
                            const uint8_t  day_selected,
                            const char    *character,
                            const uint8_t  bg_color_default)
{
	constexpr uint16_t radius_circle{15};
	constexpr int8_t   offset_text_x{-8};
	constexpr int8_t   offset_text_y{-12};

	uint8_t bg_color;
	uint8_t ft_color;

	if (day_selected) {
		if (bg_color_default == BLACK) {
			bg_color = WHITE;
			ft_color = BLACK;
		} else {
			bg_color = BLACK;
			ft_color = WHITE;
		}
	} else {
		if (bg_color_default == BLACK) {
			bg_color = BLACK;
			ft_color = WHITE;
		} else {
			bg_color = WHITE;
			ft_color = BLACK;
		}
	}
	Paint_DrawCircle(start_x, start_y, radius_circle, bg_color, DOT_PIXEL_3X3, DRAW_FILL_FULL);
	Paint_DrawString_EN(
	    offset_text_x + start_x, start_y + offset_text_y, character, &Font24, bg_color, ft_color);
}

static uint8_t _alarm_select{0};

void screen_display_param()
{
	constexpr uint16_t start_x{150};
	constexpr uint16_t start_y{130};
	constexpr uint16_t start_y_1{230};

	const uint8_t bg_color = BLACK;
	const uint8_t ft_color = WHITE;

	alarm_params_t alarm_0;
	alarm_params_t alarm_1;

	alarm_0 = get_alarm_0();
	alarm_1 = get_alarm_1();

	Paint_Clear(bg_color);

	/* Alarm 0 */
	char        clock_buf[30];
	const auto &days_0 = alarm_0.alarm_days.days;
	sprintf(clock_buf, "%2dh%02d", alarm_0.alarm_hour, alarm_0.alarm_minute);
	Paint_DrawString_EN(start_x, 80, clock_buf, &Font24, bg_color, ft_color);

	draw_days_alarm(start_x + 40 * 0, start_y, days_0.monday, "l", bg_color);
	draw_days_alarm(start_x + 40 * 1, start_y, days_0.tuesday, "m", bg_color);
	draw_days_alarm(start_x + 40 * 2, start_y, days_0.wednesday, "m", bg_color);
	draw_days_alarm(start_x + 40 * 3, start_y, days_0.thursday, "j", bg_color);
	draw_days_alarm(start_x + 40 * 4, start_y, days_0.friday, "v", bg_color);
	draw_days_alarm(start_x + 40 * 5, start_y, days_0.saturday, "s", bg_color);
	draw_days_alarm(start_x + 40 * 6, start_y, days_0.sunday, "d", bg_color);

	/* Alarm 1 */
	const auto &days_1 = alarm_1.alarm_days.days;

	sprintf(clock_buf, "%2dh%02d", alarm_1.alarm_hour, alarm_1.alarm_minute);
	Paint_DrawString_EN(start_x, 180, clock_buf, &Font24, bg_color, ft_color);

	draw_days_alarm(start_x + 40 * 0, start_y_1, days_1.monday, "l", bg_color);
	draw_days_alarm(start_x + 40 * 1, start_y_1, days_1.tuesday, "m", bg_color);
	draw_days_alarm(start_x + 40 * 2, start_y_1, days_1.wednesday, "m", bg_color);
	draw_days_alarm(start_x + 40 * 3, start_y_1, days_1.thursday, "j", bg_color);
	draw_days_alarm(start_x + 40 * 4, start_y_1, days_1.friday, "v", bg_color);
	draw_days_alarm(start_x + 40 * 5, start_y_1, days_1.saturday, "s", bg_color);
	draw_days_alarm(start_x + 40 * 6, start_y_1, days_1.sunday, "d", bg_color);

	if (_alarm_select == 0) {
		Paint_DrawCircle(100, start_y, 5, ft_color, DOT_PIXEL_3X3, DRAW_FILL_FULL);
	} else {
		Paint_DrawCircle(100, start_y_1, 5, ft_color, DOT_PIXEL_3X3, DRAW_FILL_FULL);
	}

	EPD_3IN7_1Gray_Display(_blackImage);
}

void screen_update_clock()
{
	const uint8_t day         = rtc_get_day();
	const uint8_t month       = rtc_get_month();
	const uint8_t hours       = rtc_get_hours();
	const uint8_t minutes     = rtc_get_minutes();
	const uint8_t day_of_week = rtc_get_weekday();
	const char   *days_buf[]  = {"dimanche", "lundi", "mardi", "mercredi", "jeudi", "vendredi", "samedi"};
	const char   *days_short_buf[] = {"dim.", "lun.", "mar.", "mer.", "jeu.", "vend.", "same."};
	const char   *months_buf[]     = {"",
	                                  "janvier",
	                                  "fevrier",
	                                  "mars",
	                                  "avril",
	                                  "mai",
	                                  "juin",
	                                  "juillet",
	                                  "aout",
	                                  "septembre",
	                                  "octobre",
	                                  "novembre",
	                                  "decembre"};

	char clock_buf[16];
	char date_buf[20];
	char date_buf2[30];

	Paint_Clear(BLACK);
	sprintf(clock_buf, "%2d:%02d", hours, minutes);
	sprintf(date_buf, "%s %d %s", days_buf[day_of_week], day, months_buf[month]);
	Paint_DrawString_EN(10, 30, date_buf, &Font24, BLACK, WHITE);
	Paint_DrawString_EN(0, 100, clock_buf, &FontCustom, BLACK, WHITE);

	/* Alarm 0 */
	const alarm_params_t alaram_0 = get_alarm_0();

	if (alaram_0.is_set) {
		const uint32_t minute_of_day       = hours * 60 + minutes;
		const uint32_t minute_of_day_alarm = alaram_0.alarm_hour * 60 + alaram_0.alarm_minute;
		uint8_t        is_alarm_valid      = 0;

		sprintf(date_buf, "none");
		for (uint8_t day_it = day_of_week; day_it < day_of_week + 7; day_it++) {
			const uint8_t day_r     = day_it % 7;
			const uint8_t alarm_day = (alaram_0.alarm_days.value >> day_r) & 0x01;
			if (alarm_day == 1) {
				if (day_r == day_of_week) {
					if (minute_of_day < minute_of_day_alarm) {
						sprintf(date_buf, "auj.");
						is_alarm_valid = 1;
						break;
					}
				} else {
					sprintf(date_buf, "%s", days_short_buf[day_r]);
					is_alarm_valid = 1;
					break;
				}
			}
		}

		if (is_alarm_valid) {
			sprintf(
			    date_buf2, "%s %2dh%02d", date_buf, alaram_0.alarm_hour, alaram_0.alarm_minute);
			Paint_DrawString_EN(320, 10, date_buf2, &Font20, BLACK, WHITE);
		}
	}

	/* Alarm 1 */
	const alarm_params_t alaram_1 = get_alarm_1();
	if (alaram_1.is_set) {
		const uint32_t minute_of_day       = hours * 60 + minutes;
		const uint32_t minute_of_day_alarm = alaram_1.alarm_hour * 60 + alaram_1.alarm_minute;
		uint8_t        is_alarm_valid      = 0;

		sprintf(date_buf, "none");
		for (uint8_t day_it = day_of_week; day_it < day_of_week + 7; day_it++) {
			const uint8_t day_r     = day_it % 7;
			const uint8_t alarm_day = (alaram_1.alarm_days.value >> day_r) & 0x01;
			if (alarm_day == 1) {
				if (day_r == day_of_week) {
					if (minute_of_day < minute_of_day_alarm) {
						sprintf(date_buf, "auj.");
						is_alarm_valid = 1;
						break;
					}
				} else {
					sprintf(date_buf, "%s", days_short_buf[day_r]);
					is_alarm_valid = 1;
					break;
				}
			}
		}

		if (is_alarm_valid) {
			sprintf(
			    date_buf2, "%s %2dh%02d", date_buf, alaram_1.alarm_hour, alaram_1.alarm_minute);
			Paint_DrawString_EN(320, 50, date_buf2, &Font20, BLACK, WHITE);
		}
	}

	EPD_3IN7_1Gray_Display(_blackImage);
}

/**********************/

static ui_state_t         ui_state{menu_clock};
static uint64_t           last_button_time{0};
static constexpr uint64_t timeout_setting_ms = 30 * 1000; // 30 seconds

void ui_set_state(const enum ui_state_t state)
{
	ui_state = state;
}

void ui_update()
{
	switch (ui_state) {
	case menu_clock:
		SerialUSB.println("menu_clock state");
		init_screen();
		screen_update_clock();
		EPD_3IN7_Sleep();
		break;
	case menu_settings:
		SerialUSB.println("menu_settings state");
		screen_display_param();
		break;
	}
}

/*
 * Button map
 * 6                5  16
 *   x x 13 19 1 x x
 * 7                0  17
 */
void ui_button_event(const struct button_evt_t *button_evt)
{
	const uint64_t curr_time = millis();

	int16_t clock_time;

	last_button_time = curr_time;

	if (ui_state == menu_settings) {
		alarm_params_t alarm;
		if (button_evt->action == SHORT_PRESS) {
			if (button_evt->button_id == B_MENU_CLOCK) {
				_alarm_select = 0;
			} else if (button_evt->button_id == B_MENU_SETTINGS) {
				_alarm_select = 1;
			}
		}

		if (_alarm_select == 0) {
			alarm = get_alarm_0();
		} else {
			alarm = get_alarm_1();
		}
		clock_time       = alarm.alarm_minute + (alarm.alarm_hour * 60);
		auto &alarm_days = alarm.alarm_days.days;
		switch (button_evt->button_id) {
		case B_DAY_MONDAY:
			alarm_days.monday = ~alarm_days.monday;
			break;
		case B_DAY_TUESDAY:
			alarm_days.tuesday = ~alarm_days.tuesday;
			break;
		case B_DAY_WEDNESDAY:
			alarm_days.wednesday = ~alarm_days.wednesday;
			break;
		case B_DAY_THURSDAY:
			alarm_days.thursday = ~alarm_days.thursday;
			break;
		case B_DAY_FRIDAY:
			alarm_days.friday = ~alarm_days.friday;
			break;
		case B_DAY_SATURDAY:
			alarm_days.saturday = ~alarm_days.saturday;
			break;
		case B_DAY_SUNDAY:
			alarm_days.sunday = ~alarm_days.sunday;
			break;
		case B_MIN_INC:
			clock_time += 5u * button_evt->push_count;
			break;
		case B_MIN_DEC:
			clock_time -= 5u * button_evt->push_count;
			break;
		case B_HOUR_INC:
			clock_time += (1u * 60u) * button_evt->push_count;
			break;
		case B_HOUR_DEC:
			clock_time -= (1u * 60u) * button_evt->push_count;
			break;
		default:
			break;
		}
		clock_time %= 60 * 24; /* Max 24 hours */
		alarm.alarm_minute = clock_time % 60;
		alarm.alarm_hour   = clock_time / 60;
		alarm.is_set       = true;
		if (_alarm_select == 0) {
			set_alarm_0(&alarm);
			SerialUSB.print("[screen] Setting alarm 0\n");
		} else {
			set_alarm_1(&alarm);
			SerialUSB.print("[screen] Setting alarm 1\n");
		}
		SerialUSB.print("Monday ");
		SerialUSB.println(alarm.alarm_days.days.monday);
		SerialUSB.print("Tuesday ");
		SerialUSB.println(alarm.alarm_days.days.tuesday);
		SerialUSB.print("Wednesday ");
		SerialUSB.println(alarm.alarm_days.days.wednesday);
		SerialUSB.print("Thursday ");
		SerialUSB.println(alarm.alarm_days.days.thursday);
		SerialUSB.print("Friday ");
		SerialUSB.println(alarm.alarm_days.days.friday);
		SerialUSB.print("Saturday ");
		SerialUSB.println(alarm.alarm_days.days.saturday);
		SerialUSB.print("Sunday ");
		SerialUSB.println(alarm.alarm_days.days.sunday);
		SerialUSB.print("At: ");
		SerialUSB.print(alarm.alarm_hour);
		SerialUSB.print("h");
		SerialUSB.println(alarm.alarm_minute);

	} else if (button_evt->action == SHORT_PRESS and ui_state == menu_clock) {
		/* Activate/deactivate alarm in clock screen */
		if (button_evt->button_id == B_MENU_CLOCK) {
			alarm_params_t alarm_0 = get_alarm_0();
			alarm_0.is_set         = !alarm_0.is_set;
			set_alarm_0(&alarm_0);
		} else if (button_evt->button_id == B_MENU_SETTINGS) {
			alarm_params_t alarm_1 = get_alarm_1();
			alarm_1.is_set         = !alarm_1.is_set;
			set_alarm_1(&alarm_1);
		}
	}
}

void display_hello_msg(void)
{
	Paint_Clear(BLACK);
	Paint_DrawString_EN(100, 100, "Bonjour Tibo !", &Font24, BLACK, WHITE);
	EPD_3IN7_1Gray_Display(_blackImage);
}

void display_error(int error)
{
	Paint_Clear(BLACK);
	if (error == 0) {
		Paint_DrawString_EN(100, 100, "No wifi chip detected!", &Font24, BLACK, WHITE);
	} else if (error == 1) {
		Paint_DrawString_EN(100, 100, "Fail connect to wifi!", &Font24, BLACK, WHITE);
	} else {
		Paint_DrawString_EN(100, 100, "Unknown error", &Font24, BLACK, WHITE);
	}

	EPD_3IN7_1Gray_Display(_blackImage);
}
