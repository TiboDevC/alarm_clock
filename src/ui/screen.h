#ifndef ALARM_CLOCK_SCREEN_H
#define ALARM_CLOCK_SCREEN_H

void init_screen();
void display_hello_msg();
void screen_display_param();
void screen_update_clock();
void ui_update();
void display_error(int error);

/***************************/
enum ui_state_t { menu_clock, menu_settings };

void ui_set_state(enum ui_state_t state);
void ui_button_event(const struct button_evt_t *button_evt);

#endif // ALARM_CLOCK_SCREEN_H
