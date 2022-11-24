#include <array>
#include <limits.h>

#include "Arduino.h"

// Rtos core library
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "button.hpp"
#ifdef __cplusplus
extern "C" {
#endif

#include "timer.h"

#ifdef __cplusplus
}
#endif

#define BUTTON_INFO(x...) Serial.print("[button]" x)

constexpr int button_adc_tolerance  = 10; /* Number of ADC steps to accept button */
constexpr int button_adc_unselected = 10; /* Number of ADC steps to consider button not selected */

enum button_input_mode_t {
	B_ANALOG,
	B_IRQ,
};

struct button_ctx_t {
	constexpr button_ctx_t(button_id_t         id,
	                       uint8_t             pin_id,
	                       button_input_mode_t input_mode,
	                       int                 adc_threshold = 0)
	    : id(id)
	    , pin_id(pin_id)
	    , input_mode(input_mode)
	    , adc_threshold(adc_threshold)
	{
	}
	const button_id_t         id{};
	const uint8_t             pin_id{};
	const button_input_mode_t input_mode{};

	uint16_t adc_threshold{};

	uint8_t  state{};
	uint64_t last_state_time{};
	uint8_t  push_count{};
	uint8_t  on_use{};
};

static constexpr uint16_t long_press_ms{3000};
static constexpr uint16_t short_press_ms{300};

static constexpr uint8_t b_menu_0_pin{5};
static constexpr uint8_t b_menu_1_pin{1};
static constexpr uint8_t b_day_0_pin{A3};
static constexpr uint8_t b_day_1_pin{A4};
static constexpr uint8_t b_hour_inc_pin{0};
static constexpr uint8_t b_hour_dec_pin{17};
static constexpr uint8_t b_min_inc_pin{16};
static constexpr uint8_t b_min_dec_pin{6};

static std::array<button_ctx_t, B_LAST_BUTTON> button_states{{
    button_ctx_t(B_MENU_CLOCK, b_menu_0_pin, B_IRQ),
    button_ctx_t(B_MENU_SETTINGS, b_menu_1_pin, B_IRQ),
    button_ctx_t(B_DAY_MONDAY, b_day_0_pin, B_ANALOG, 935),
    button_ctx_t(B_DAY_TUESDAY, b_day_0_pin, B_ANALOG, 682),
    button_ctx_t(B_DAY_WEDNESDAY, b_day_0_pin, B_ANALOG, 511),
    button_ctx_t(B_DAY_THURSDAY, b_day_0_pin, B_ANALOG, 342),
    button_ctx_t(B_DAY_FRIDAY, b_day_1_pin, B_ANALOG, 937),
    button_ctx_t(B_DAY_SATURDAY, b_day_1_pin, B_ANALOG, 687),
    button_ctx_t(B_DAY_SUNDAY, b_day_1_pin, B_ANALOG, 343),
    button_ctx_t(B_HOUR_INC, b_hour_inc_pin, B_IRQ),
    button_ctx_t(B_HOUR_DEC, b_hour_dec_pin, B_IRQ),
    button_ctx_t(B_MIN_INC, b_min_inc_pin, B_IRQ),
    button_ctx_t(B_MIN_DEC, b_min_dec_pin, B_IRQ),
}};


static uint8_t num_button_ack_{};

static std::array<button_state_t, NUM_BUTTON> button_action_{};

static uint16_t next_timer_value(void)
{
	const uint64_t curr_time = millis();
	uint16_t       min_timer = SHRT_MAX;
	for (const auto &button : button_states) {
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

static void cb_button_timer(void)
{
	uint8_t        button_used = 0;
	const uint64_t curr_time   = millis();
	uint16_t       next_timer  = 0;

	for (auto &button : button_states) {
		if (button.on_use and button.input_mode == B_IRQ) {
			uint8_t timer_timeout = 0;

			if (button.state == LOW and curr_time - button.last_state_time > long_press_ms) {
				timer_timeout                             = 1;
				button_action_.at(num_button_ack_).action = LONG_PRESS;
				Serial.print("Long press ");
				Serial.println(button.pin_id);
			} else if (button.state == HIGH and
			           curr_time - button.last_state_time > short_press_ms) {
				timer_timeout                                 = 1;
				button_action_.at(num_button_ack_).action     = SHORT_PRESS;
				button_action_.at(num_button_ack_).push_count = button.push_count;
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

template <uint8_t pin_id> static void button_handler(void)
{
	const uint64_t curr_time = millis();
	uint8_t        state;
	uint16_t       next_timer;
	// Serial.print("Button ");
	// Serial.println(pin_id);

	for (auto &button : button_states) {
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

static void button_enable_interrupts(const uint8_t pin_id)
{
	switch (pin_id) {
	case b_menu_0_pin:
		attachInterrupt(digitalPinToInterrupt(b_menu_0_pin), button_handler<b_menu_0_pin>, CHANGE);
		break;
	case b_menu_1_pin:
		attachInterrupt(digitalPinToInterrupt(b_menu_1_pin), button_handler<b_menu_1_pin>, CHANGE);
		break;
	case b_hour_inc_pin:
		attachInterrupt(
		    digitalPinToInterrupt(b_hour_inc_pin), button_handler<b_hour_inc_pin>, CHANGE);
		break;
	case b_hour_dec_pin:
		attachInterrupt(
		    digitalPinToInterrupt(b_hour_dec_pin), button_handler<b_hour_dec_pin>, CHANGE);
		break;
	case b_min_inc_pin:
		attachInterrupt(digitalPinToInterrupt(b_min_inc_pin), button_handler<b_min_inc_pin>, CHANGE);
		break;
	case b_min_dec_pin:
		attachInterrupt(digitalPinToInterrupt(b_min_dec_pin), button_handler<b_min_dec_pin>, CHANGE);
		break;
	default:
		break;
	}
}

static void handle_timer5_poll_button(void)
{
	const uint64_t curr_time   = millis();
	const int      adc_b_day_0 = analogRead(b_day_0_pin);
	const int      adc_b_day_1 = analogRead(b_day_1_pin);
	int            adc_value, button_current_state;
	for (auto &button : button_states) {
		if (button.input_mode == B_ANALOG) {
			if (button.pin_id == b_day_0_pin) {
				adc_value = adc_b_day_0;
			} else {
				adc_value = adc_b_day_1;
			}

			if (adc_value < button_adc_unselected) {
				button_current_state = HIGH;
			}
			if ((adc_value + button_adc_tolerance) > button.adc_threshold and
			    (adc_value - button_adc_tolerance) < button.adc_threshold) {
				button_current_state = LOW;
			} else {
				button_current_state = HIGH;
			}

			if (LOW == button_current_state and HIGH == button.state) {
				/* Button just pushed */
				button.last_state_time = curr_time;
				button.push_count++;
			} else if (HIGH == button_current_state and HIGH == button.state) {
				if ((curr_time - button.last_state_time) > short_press_ms and
				    button.push_count > 0) {
					if ((curr_time - button.last_state_time) < long_press_ms) {
						/* Button released */
						Serial.print("Button ");
						Serial.print(button.id);
						Serial.print(", count: ");
						Serial.println(button.push_count);
					}

					button.push_count = 0;
				}
			} else if (LOW == button_current_state and LOW == button.state) {
				if ((curr_time - button.last_state_time) > long_press_ms and
				    button.push_count == 1) {
					/* Button released */
					Serial.print("Button ");
					Serial.print(button.id);
					Serial.println(", long press");
					button.push_count++;
				}
			}

			button.state = button_current_state;
		}
	}
}

/***************************************/

#define BUTTON_TASK_STACK_SIZE 200
#define BUTTON_POLLING_FREQ_MS 50 /* Poll buttons each x ms */

static StaticTask_t _button_task_buffer;
static StackType_t  _button_stack[BUTTON_TASK_STACK_SIZE];

static SemaphoreHandle_t _button_task_semaphore = nullptr;
static TaskHandle_t      _button_task           = nullptr;


static void a(void)
{
	BUTTON_INFO("init_buttons\n");

	if (_button_task_semaphore == nullptr) {
		_button_task_semaphore = xSemaphoreCreateBinary();
		if (_button_task_semaphore != nullptr) {
			xSemaphoreGive(_button_task_semaphore);
		} else {
			BUTTON_INFO("Cannot create semaphor!\n");
			return;
		}
	}

	for (auto &button : button_states) {
		pinMode(button.pin_id, INPUT_PULLUP);
		delay(200);                                /* Hard delay for pullup to be effective */
		button.state = digitalRead(button.pin_id); /* Read initial button state */
		if (button.input_mode == B_IRQ) {
			button_enable_interrupts(button.pin_id);
		}
	}

	/* Init debounce button timers */
	setup_gclk();
	setup_timer4();
	setup_timer5();
}


#ifdef __cplusplus
extern "C" {
#endif

void init_buttons(void)
{
	a();
}

#ifdef __cplusplus
}
#endif

static void _button_start_polling(void *pvParameters)
{
	constexpr int period_ms     = BUTTON_POLLING_FREQ_MS / portTICK_PERIOD_MS;
	TickType_t    xLastWakeTime = xTaskGetTickCount();

	while (xSemaphoreTake(_button_task_semaphore, 0) == pdFALSE) {
		handle_timer5_poll_button();
		vTaskDelayUntil(&xLastWakeTime, period_ms);
	}
	handle_timer5_poll_button();
	BUTTON_INFO("Delete button task\n");
	xSemaphoreGive(_button_task_semaphore);
	vTaskDelete(_button_task);
	_button_task = nullptr;
}

void button_start_polling(void)
{
	if (_button_task_semaphore != nullptr) {
		if (xSemaphoreTake(_button_task_semaphore, 0) == pdFALSE) {
			BUTTON_INFO("Task already running\n");
		}
	} else {
		/* Semaphore not created */
		BUTTON_INFO("Could not start polling\n");
		return;
	}

	if (_button_task == nullptr) {
		BUTTON_INFO("Create button task\n");
		_button_task = xTaskCreateStatic(_button_start_polling,
		                                 "Button polling",
		                                 BUTTON_TASK_STACK_SIZE,
		                                 NULL,
		                                 tskIDLE_PRIORITY + 3,
		                                 _button_stack,
		                                 &_button_task_buffer);
	}
}

void button_stop_polling(void)
{
	if (_button_task_semaphore != nullptr) {
		xSemaphoreGive(_button_task_semaphore);
	}
}

void button_get_state(std::array<button_state_t, NUM_BUTTON> &button_action, uint8_t &num_button_ack)
{
	noInterrupts();
	num_button_ack = num_button_ack_;
	if (num_button_ack_ > 0) {
		button_action = button_action_;
		for (unsigned int i = 0; i < num_button_ack_; i++) {
			const uint8_t pin_id = button_action_.at(i).pin_id;
			// Serial.print("Reactivate button: ");
			// Serial.println(pin_id);
			button_enable_interrupts(pin_id);
			for (auto &button : button_states) {
				if (button.pin_id == pin_id and button.input_mode == B_IRQ) {
					button.state = HIGH;
				}
			}
		}
		num_button_ack_ = 0;
	}
	interrupts();
}

/***************************************/