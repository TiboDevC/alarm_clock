#ifndef ALARM_CLOCK_SCREEN_H
#define ALARM_CLOCK_SCREEN_H

void init_screen();
void display_hello_msg();
void screen_display_param();
void screen_update_clock();
void ui_update();

/***************************/
enum ui_state_t { menu_clock, menu_settings };

void ui_set_state(enum ui_state_t state);
void ui_button_event(void);
void ui_start_task(void);

#endif // ALARM_CLOCK_SCREEN_H
