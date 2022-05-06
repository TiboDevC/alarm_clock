#ifndef ALARM_CLOCK_FLASH_STORAGE_H
#define ALARM_CLOCK_FLASH_STORAGE_H

union days_u {
    struct days_t {
        uint8_t monday : 1;
        uint8_t tuesday : 1;
        uint8_t wednesday : 1;
        uint8_t thursday : 1;
        uint8_t friday : 1;
        uint8_t saturday : 1;
        uint8_t sunday : 1;
        uint8_t padding : 1;
    };
    uint8_t value;
};

struct alarm_params_t {
    bool is_set{false};
    uint64_t alarm_time{};    // when it should ring
    bool rings_tomorrow{true};// is the user disable alarm for the next day
    days_u alarm_days{};

    friend bool operator!=(const alarm_params_t &a0, const alarm_params_t &a1) {
        if (a0.is_set != a1.is_set) {
            return true;
        }
        if (a0.alarm_time != a1.alarm_time) {
            return true;
        }
        if (a0.rings_tomorrow != a1.rings_tomorrow) {
            return true;
        }
        if (a0.alarm_days.value != a1.alarm_days.value) {
            return true;
        }
        return false;
    }
};

void init_alarm_flash_storage();
void set_alarm_0(const alarm_params_t &alarm);
alarm_params_t get_alarm_0();
void set_alarm_1(const alarm_params_t &alarm);
alarm_params_t get_alarm_1();

#endif//ALARM_CLOCK_FLASH_STORAGE_H
