#ifndef ALARM_CLOCK_SAMD21_WATCHDOG_H
#define ALARM_CLOCK_SAMD21_WATCHDOG_H

#ifdef __cplusplus
extern "C" {
#endif

void wdt_init(void);
void wdt_refresh(void);

#ifdef __cplusplus
}
#endif

#endif /* ALARM_CLOCK_SAMD21_WATCHDOG_H */
