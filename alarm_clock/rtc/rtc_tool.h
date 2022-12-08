enum rtc_days_t {
	rtc_sunday = 0,
	rtc_monday,
	rtc_tuesday,
	rtc_wednesday,
	rtc_thursday,
	rtc_friday,
	rtc_saturday,
};

void            init_rtc(void);
void            rtc_start_interrupt(void);
void            rtc_set_epoch(uint64_t epoch);
void            rtc_set_hours(uint8_t hours);
uint64_t        rtc_get_epoch(void);
uint8_t         rtc_get_day(void);
uint8_t         rtc_get_month(void);
uint8_t         rtc_get_year(void);
uint8_t         rtc_get_hours(void);
uint8_t         rtc_get_minutes(void);
uint8_t         rtc_get_seconds(void);
enum rtc_days_t rtc_get_weekday();
