#include <array>
#include <limits.h>

#include "Arduino.h"

#include "button.h"
#include "timer.hpp"

struct button_ctx_t {
  uint8_t pin_id;
  uint8_t state;
  uint64_t last_state_time;
  uint8_t push_count;
  uint8_t on_use;
};

static uint16_t long_press_ms{ 3000 };
static uint16_t short_press_ms{ 300 };

static constexpr uint8_t button_pin_0_int{ 0 };
static constexpr uint8_t button_pin_1_int{ 6 };
static constexpr uint8_t button_pin_2_int{ 17 };
static constexpr uint8_t button_pin_3_int{ 5 };
static constexpr uint8_t button_pin_4{ 19 };
static constexpr uint8_t button_pin_5{ 13 };
static constexpr uint8_t button_pin_6_int{ 16 };
static constexpr uint8_t button_pin_7_int{ 1 };
static constexpr uint8_t button_pin_8{ 12 };
static constexpr uint8_t button_pin_9{ 11 };
static constexpr uint8_t button_pin_10_int{ 7 };
static constexpr uint8_t button_pin_11_12{ 21 };

static std::array<button_ctx_t, NUM_BUTTON> button_states{
  { { button_pin_8, HIGH, millis(), 0, 0 },
    { button_pin_9, HIGH, millis(), 0, 0 },
    { button_pin_0_int, HIGH, millis(), 0, 0 },
    { button_pin_1_int, HIGH, millis(), 0, 0 },
    { button_pin_2_int, HIGH, millis(), 0, 0 },
    { button_pin_3_int, HIGH, millis(), 0, 0 },
    { button_pin_4, HIGH, millis(), 0, 0 },
    { button_pin_5, HIGH, millis(), 0, 0 },
    { button_pin_6_int, HIGH, millis(), 0, 0 },
    { button_pin_7_int, HIGH, millis(), 0, 0 },
    { button_pin_10_int, HIGH, millis(), 0, 0 },
    { button_pin_11_12, HIGH, millis(), 0, 0 } }
};


static uint8_t num_button_ack_{};

static std::array<button_state_t, NUM_BUTTON> button_action_{};

static uint16_t next_timer_value(void) {
  const uint64_t curr_time = millis();
  uint16_t min_timer = SHRT_MAX;
  for (const auto& button : button_states) {
    int16_t timer_button = 0;
    if (button.on_use) {
      if (button.state == LOW) {
        timer_button = long_press_ms - (curr_time - button.last_state_time);
        // Serial.print("Start long press timer: ");
      } else if (button.state == HIGH) {
        // Serial.print("Start short press timer: ");
        timer_button = short_press_ms - (curr_time - button.last_state_time);
      }
      // Serial.println(timer_button);
      if (timer_button <= 0) {
        // Serial.print("Abnormal_2: ");
        // Serial.println(timer_button);
      }
      if (timer_button < min_timer) {
        min_timer = timer_button;
      }
    }
  }
  return min_timer;
}

static void cb_button_timer(void) {
  uint8_t button_used = 0;
  const uint64_t curr_time = millis();
  uint16_t next_timer = 0;

  for (auto& button : button_states) {
    if (button.on_use) {
      uint8_t timer_timeout = 0;

      if (button.state == LOW and curr_time - button.last_state_time > long_press_ms) {
        timer_timeout = 1;
        button_action_.at(num_button_ack_).action = LONG_PRESS;
        // Serial.print("Long press ");
        // Serial.println(button.pin_id);
      } else if (button.state == HIGH and curr_time - button.last_state_time > short_press_ms) {

        timer_timeout = 1;
        button_action_.at(num_button_ack_).action = SHORT_PRESS;
        button_action_.at(num_button_ack_).push_count = button.push_count;
        // Serial.print("Short press ");
        // Serial.print(button.pin_id);
        // Serial.print(", count: ");
        // Serial.println(button.push_count);
        button.push_count = 0;
      }
      if (timer_timeout) {
        detachInterrupt(digitalPinToInterrupt(button.pin_id));
        button_action_.at(num_button_ack_).pin_id = button.pin_id;
        button.on_use = 0;
        num_button_ack_++;
      } else {
        button_used++;
      }
    }
  }
  next_timer = next_timer_value();
  if (next_timer != SHRT_MAX) {
    call_me_in_x_ms(next_timer, cb_button_timer);
  } else {
    // Serial.println("No more active button");
  }
}

template<uint8_t pin_id>
static void button_handler(void) {
  const uint64_t curr_time = millis();
  uint8_t state;
  uint16_t next_timer = 0;
  // Serial.print("Button ");
  // Serial.println(pin_id);

  for (auto& button : button_states) {
    if (button.pin_id == pin_id) {
      button.on_use = 1;
      state = digitalRead(button.pin_id);
      if (state == button.state and state == LOW) {
        // Serial.println("Abnormal_1");
        continue;
      } else if (state == button.state and state == HIGH) {
        // Serial.println("Abnormal_0");
        continue;
      } else if (state != button.state and state == LOW) {
        // launch long press timer
      } else if (state != button.state and state == HIGH) {
        button.push_count++;
      }
      button.state = state;
      button.last_state_time = curr_time;
      next_timer = next_timer_value();
      if (next_timer != SHRT_MAX) {
        call_me_in_x_ms(next_timer, cb_button_timer);
      }
      break;
    }
  }
}

/***************************************/

void init_buttons(void) {
  for (auto& button : button_states) {
    pinMode(button.pin_id, INPUT_PULLUP);
    delay(200);
    button.state = digitalRead(button.pin_id);
  }
  attachInterrupt(digitalPinToInterrupt(button_pin_8), button_handler<button_pin_8>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button_pin_9), button_handler<button_pin_9>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button_pin_0_int), button_handler<button_pin_0_int>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button_pin_1_int), button_handler<button_pin_1_int>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button_pin_2_int), button_handler<button_pin_2_int>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button_pin_3_int), button_handler<button_pin_3_int>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button_pin_4), button_handler<button_pin_4>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button_pin_5), button_handler<button_pin_5>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button_pin_6_int), button_handler<button_pin_6_int>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button_pin_7_int), button_handler<button_pin_7_int>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button_pin_10_int), button_handler<button_pin_10_int>, CHANGE);
  setup_timer4();
}

void button_get_state(std::array<button_state_t, NUM_BUTTON>& button_action, uint8_t& num_button_ack) {
  noInterrupts();
  num_button_ack = num_button_ack_;
  if (num_button_ack_ > 0) {
    button_action = button_action_;
    for (unsigned int i = 0; i < num_button_ack_; i++) {
      const uint8_t pin_id = button_action_.at(i).pin_id;
      // Serial.print("Reactivate button: ");
      // Serial.println(pin_id);
      switch (pin_id) {
        case button_pin_8:
          attachInterrupt(digitalPinToInterrupt(button_pin_8), button_handler<button_pin_8>, CHANGE);
          break;
        case button_pin_9:
          attachInterrupt(digitalPinToInterrupt(button_pin_9), button_handler<button_pin_9>, CHANGE);
          break;
        case button_pin_0_int:
          attachInterrupt(digitalPinToInterrupt(button_pin_0_int), button_handler<button_pin_0_int>, CHANGE);
          break;
        case button_pin_1_int:
          attachInterrupt(digitalPinToInterrupt(button_pin_1_int), button_handler<button_pin_1_int>, CHANGE);
          break;
        case button_pin_2_int:
          attachInterrupt(digitalPinToInterrupt(button_pin_2_int), button_handler<button_pin_2_int>, CHANGE);
          break;
        case button_pin_3_int:
          attachInterrupt(digitalPinToInterrupt(button_pin_3_int), button_handler<button_pin_3_int>, CHANGE);
          break;
        case button_pin_4:
          attachInterrupt(digitalPinToInterrupt(button_pin_4), button_handler<button_pin_4>, CHANGE);
          break;
        case button_pin_5:
          attachInterrupt(digitalPinToInterrupt(button_pin_5), button_handler<button_pin_5>, CHANGE);
          break;
        case button_pin_6_int:
          attachInterrupt(digitalPinToInterrupt(button_pin_6_int), button_handler<button_pin_6_int>, CHANGE);
          break;
        case button_pin_7_int:
          attachInterrupt(digitalPinToInterrupt(button_pin_7_int), button_handler<button_pin_7_int>, CHANGE);
          break;
        case button_pin_10_int:
          attachInterrupt(digitalPinToInterrupt(button_pin_10_int), button_handler<button_pin_10_int>, CHANGE);
          break;
      }
      for (auto& button : button_states) {
        if (button.pin_id == pin_id) {
          button.state = HIGH;
        }
      }
    }
    num_button_ack_ = 0;
  }
  interrupts();
}

/***************************************/