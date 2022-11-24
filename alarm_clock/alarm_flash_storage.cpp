#include "Arduino.h"
#include <FlashStorage_SAMD.h>

#include "alarm_flash_storage.h"

FlashStorage(alarm_0_flash, alarm_params_t);
FlashStorage(alarm_1_flash, alarm_params_t);

static alarm_params_t alarm_0_ram{};
static alarm_params_t alarm_1_ram{};

void init_alarm_flash_storage()
{
	Serial.println("init_alarm_flash_storage");
	alarm_0_flash.read(alarm_0_ram);
	alarm_1_flash.read(alarm_1_ram);
}

void set_alarm_0(alarm_params_t &alarm)
{
	alarm_0_ram = alarm;
	alarm_0_flash.write(alarm);
}

alarm_params_t get_alarm_0()
{
	return alarm_0_ram;
}

void set_alarm_1(alarm_params_t &alarm)
{
	alarm_1_ram = alarm;
	alarm_1_flash.write(alarm);
}

alarm_params_t get_alarm_1()
{
	return alarm_1_ram;
}
