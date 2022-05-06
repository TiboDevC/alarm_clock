#ifndef ALARM_CLOCK_SCREEN_H
#define ALARM_CLOCK_SCREEN_H

void init_screen();
void screen_display_param();
void screen_update_clock();

/***************************/
enum ui_state_t { menu_clock, menu_settings };

void ui_update();
void ui_set_state(const ui_state_t &state);

#endif//ALARM_CLOCK_SCREEN_H
