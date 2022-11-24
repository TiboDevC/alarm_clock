#ifndef ALARM_CLOCK_TIMER_H
#define ALARM_CLOCK_TIMER_H

void setup_gclk(void);
void setup_timer4(void);
void call_me_in_x_ms(uint32_t ms, void (*cb)(void));

void setup_timer5(void);
void start_timer5(void (*cb)(void));
void stop_timer5(void);


#endif // ALARM_CLOCK_TIMER_H
