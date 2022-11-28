#ifndef ALARM_CLOCK_SCREEN_H
#define ALARM_CLOCK_SCREEN_H

void init_screen();
void display_hello_ms();
void screen_display_param();
void screen_update_clock();

/***************************/
enum ui_state_t { menu_clock, menu_settings };

void ui_button_event(void);
void ui_update();
void ui_set_state(const ui_state_t &state);
ui_state_t ui_get_state(void);

#endif//ALARM_CLOCK_SCREEN_H
