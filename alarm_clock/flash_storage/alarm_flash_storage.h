#ifndef ALARM_CLOCK_FLASH_STORAGE_H
#define ALARM_CLOCK_FLASH_STORAGE_H

union days_u {
	struct days_t {
		uint8_t sunday    : 1;
		uint8_t monday    : 1;
		uint8_t tuesday   : 1;
		uint8_t wednesday : 1;
		uint8_t thursday  : 1;
		uint8_t friday    : 1;
		uint8_t saturday  : 1;
		uint8_t padding   : 1;
	} days;
	uint8_t value;
};

struct alarm_params_t {
	uint8_t      is_set;
	uint8_t      alarm_hour;     // when it should ring
	uint8_t      alarm_minute;   // when it should ring
	uint8_t      rings_tomorrow; // is the user disable alarm for the next day
	union days_u alarm_days;
};

void                  init_alarm_flash_storage();
void                  set_alarm_0(struct alarm_params_t *alarm);
struct alarm_params_t get_alarm_0();
void                  set_alarm_1(struct alarm_params_t *alarm);
struct alarm_params_t get_alarm_1();

#endif // ALARM_CLOCK_FLASH_STORAGE_H
