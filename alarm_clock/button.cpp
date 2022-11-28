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
    const bool polling;// True: polling, False: interrupt
};

static uint16_t long_press_ms{3000};
static uint16_t short_press_ms{300};

static constexpr uint8_t button_pin_0_int{0};
static constexpr uint8_t button_pin_1_int{6};
static constexpr uint8_t button_pin_2_int{17};
static constexpr uint8_t button_pin_3_int{5};
static constexpr uint8_t button_pin_4{19};
static constexpr uint8_t button_pin_5{13};
static constexpr uint8_t button_pin_6_int{16};
static constexpr uint8_t button_pin_7_int{1};
static constexpr uint8_t button_pin_8{12};
static constexpr uint8_t button_pin_9{11};
static constexpr uint8_t button_pin_10_int{7};
static constexpr uint8_t button_pin_11_12{21};

static std::array<button_ctx_t, NUM_BUTTON> button_states{
        {{button_pin_8, HIGH, millis(), 0, 0, true},
         {button_pin_9, HIGH, millis(), 0, 0, true},
         {button_pin_0_int, HIGH, millis(), 0, 0, false},
         {button_pin_1_int, HIGH, millis(), 0, 0, false},
         {button_pin_2_int, HIGH, millis(), 0, 0, false},
         {button_pin_3_int, HIGH, millis(), 0, 0, false},
         {button_pin_4, HIGH, millis(), 0, 0, true},
         {button_pin_5, HIGH, millis(), 0, 0, true},
         {button_pin_6_int, HIGH, millis(), 0, 0, false},
         {button_pin_7_int, HIGH, millis(), 0, 0, false},
         {button_pin_10_int, HIGH, millis(), 0, 0, false},
         {button_pin_11_12, HIGH, millis(), 0, 0, true}}};


static uint8_t num_button_ack_{};

static std::array<button_state_t, NUM_BUTTON> button_action_{};

static uint16_t next_timer_value(void) {
    const uint64_t curr_time = millis();
    uint16_t min_timer       = SHRT_MAX;
    for (const auto &button: button_states) {
        int16_t timer_button = 0;
        if (button.on_use) {
            if (button.state == LOW) {
                timer_button =
                        long_press_ms - (curr_time - button.last_state_time);
                // Serial.print("Start long press timer: ");
            } else if (button.state == HIGH) {
                // Serial.print("Start short press timer: ");
                timer_button =
                        short_press_ms - (curr_time - button.last_state_time);
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
    uint8_t button_used      = 0;
    const uint64_t curr_time = millis();
    uint16_t next_timer      = 0;

    for (auto &button: button_states) {
        if (button.on_use and not button.polling) {
            uint8_t timer_timeout = 0;

            if (button.state == LOW and
                curr_time - button.last_state_time > long_press_ms) {
                timer_timeout                             = 1;
                button_action_.at(num_button_ack_).action = LONG_PRESS;
                Serial.print("Long press ");
                Serial.println(button.pin_id);
            } else if (button.state == HIGH and
                       curr_time - button.last_state_time > short_press_ms) {

                timer_timeout                             = 1;
                button_action_.at(num_button_ack_).action = SHORT_PRESS;
                button_action_.at(num_button_ack_).push_count =
                        button.push_count;
                Serial.print("Short press ");
                Serial.print(button.pin_id);
                Serial.print(", count: ");
                Serial.println(button.push_count);
                button.push_count = 0;
            }
            if (timer_timeout) {
                detachInterrupt(digitalPinToInterrupt(button.pin_id));
                button_action_.at(num_button_ack_).pin_id = button.pin_id;
                button.on_use                             = 0;
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

    for (auto &button: button_states) {
        if (button.pin_id == pin_id) {
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
            button.on_use          = 1;
            button.state           = state;
            button.last_state_time = curr_time;
            next_timer             = next_timer_value();
            if (next_timer != SHRT_MAX) {
                call_me_in_x_ms(next_timer, cb_button_timer);
            }
            break;
        }
    }
}

static void button_enable_interrupts(const uint8_t pin_id) {
    switch (pin_id) {
        case button_pin_0_int:
            attachInterrupt(digitalPinToInterrupt(button_pin_0_int),
                            button_handler<button_pin_0_int>, CHANGE);
            break;
        case button_pin_1_int:
            attachInterrupt(digitalPinToInterrupt(button_pin_1_int),
                            button_handler<button_pin_1_int>, CHANGE);
            break;
        case button_pin_2_int:
            attachInterrupt(digitalPinToInterrupt(button_pin_2_int),
                            button_handler<button_pin_2_int>, CHANGE);
            break;
        case button_pin_3_int:
            attachInterrupt(digitalPinToInterrupt(button_pin_3_int),
                            button_handler<button_pin_3_int>, CHANGE);
            break;
        case button_pin_6_int:
            attachInterrupt(digitalPinToInterrupt(button_pin_6_int),
                            button_handler<button_pin_6_int>, CHANGE);
            break;
        case button_pin_7_int:
            attachInterrupt(digitalPinToInterrupt(button_pin_7_int),
                            button_handler<button_pin_7_int>, CHANGE);
            break;
        case button_pin_10_int:
            attachInterrupt(digitalPinToInterrupt(button_pin_10_int),
                            button_handler<button_pin_10_int>, CHANGE);
            break;
        default:
            break;
    }
}

static void handle_timer5_poll_button(void) {
    const uint64_t curr_time = millis();
    for (auto &button: button_states) {
        uint8_t button_timeout = 0;
        if (button.polling) {
            // button_pin_11_12 is analog input
            if (button.pin_id != button_pin_11_12) {
                const uint8_t state = digitalRead(button.pin_id);
                auto &button_action = button_action_.at(num_button_ack_);
                if (state != button.state) {
                    // If previous state is not long press
                    if (not(button.state == LOW and
                            button.last_state_time == 0)) {
                        button.last_state_time = curr_time;
                        if (state == LOW) {
                            button.push_count++;
                        }
                    }
                    button.state = state;
                } else if (button.last_state_time != 0) {
                    if (state == LOW) {
                        if (curr_time - button.last_state_time >
                            long_press_ms) {
                            // Long press
                            button_timeout       = 1;
                            button_action.action = LONG_PRESS;
                            Serial.print("p_Long press ");
                            Serial.println(button.pin_id);
                        }
                    } else if (state == HIGH) {
                        if (curr_time - button.last_state_time >
                            short_press_ms) {
                            // Short press
                            button_timeout       = 1;
                            button_action.action = SHORT_PRESS;
                            Serial.print("p_Short press ");
                            Serial.print(button.pin_id);
                            Serial.print(", count: ");
                            Serial.println(button.push_count);
                        }
                    }
                    if (button_timeout) {
                        button_action.push_count = button.push_count;
                        button_action.pin_id     = button.pin_id;
                        num_button_ack_++;
                        button.push_count      = 0;
                        button.last_state_time = 0;
                    }
                }
            }
        }
    }
}

/***************************************/

void init_buttons(void) {
    Serial.println("init_buttons");
    for (auto &button: button_states) {
        if (button.pin_id == 11 or button.pin_id == 12) {
            // I2C has hardware pull-up
            pinMode(button.pin_id, INPUT);
        } else {
            pinMode(button.pin_id, INPUT_PULLUP);
        }
        delay(200);
        button.state = digitalRead(button.pin_id);
        button_enable_interrupts(button.pin_id);
    }
    setup_gclk();
    setup_timer4();
    setup_timer5();
}

void button_start_polling(void) {
    start_timer5(handle_timer5_poll_button);
}

void button_stop_polling(void) {
    stop_timer5();
}

void button_get_state(std::array<button_state_t, NUM_BUTTON> &button_action,
                      uint8_t &num_button_ack) {
    noInterrupts();
    num_button_ack = num_button_ack_;
    if (num_button_ack_ > 0) {
        button_action = button_action_;
        for (unsigned int i = 0; i < num_button_ack_; i++) {
            const uint8_t pin_id = button_action_.at(i).pin_id;
            // Serial.print("Reactivate button: ");
            // Serial.println(pin_id);
            button_enable_interrupts(pin_id);
            for (auto &button: button_states) {
                if (button.pin_id == pin_id and not button.polling) {
                    button.state = HIGH;
                }
            }
        }
        num_button_ack_ = 0;
    }
    interrupts();
}

/***************************************/