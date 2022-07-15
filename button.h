#ifndef ALARM_CLOCK_BUTTON_H
#define ALARM_CLOCK_BUTTON_H

#include <array>
#include <stdint.h>

#define NUM_BUTTON 12

enum button_action_t {
  SHORT_PRESS,
  LONG_PRESS
};

struct button_state_t {
  uint8_t pin_id{};
  enum button_action_t action{};
  uint8_t push_count{};
};

void init_buttons(void);
void button_get_state(std::array<button_state_t, NUM_BUTTON>& button_action, uint8_t& num_button_ack);

#endif //ALARM_CLOCK_BUTTON_H
