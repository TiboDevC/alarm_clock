#include <stdint.h>

#include <Arduino.h>

#include "debug.h"

#define TIMER_CLOCK_FREQ       48000000 /* Hz */
#define TIMER_CLOCK_FREQ_DIV   16
#define TIMER_TIMER4_FREQ_DIV  256
#define TIMER_TIMER5_FREQ_TCC0 100 /* Hz */

static const uint32_t freq_timer2_ = (TIMER_CLOCK_FREQ / TIMER_CLOCK_FREQ_DIV) / TIMER_TIMER4_FREQ_DIV;
static const uint32_t freq_timer5_ = (TIMER_CLOCK_FREQ / TIMER_CLOCK_FREQ_DIV);

void (*cb_timer4_)(void);
void (*cb_timer5_)(void);

/* 15. GCLK - Generic Clock Controller */
void setup_gclk(void)
{
	// Set up the generic clock (GCLK4) used to clock timers
	REG_GCLK_GENDIV =
	    GCLK_GENDIV_DIV(
		TIMER_CLOCK_FREQ_DIV) | // Divide the 48MHz clock source by divisor 1: 48MHz/1=48MHz
	    GCLK_GENDIV_ID(4);          // Select Generic Clock (GCLK) 4
	while (GCLK->STATUS.bit.SYNCBUSY)
		; // Wait for synchronization

	REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |         // Set the duty cycle to 50/50 HIGH/LOW
	                   GCLK_GENCTRL_GENEN |       // Enable GCLK4
	                   GCLK_GENCTRL_SRC_DFLL48M | // Set the 48MHz clock source
	                   GCLK_GENCTRL_ID(4);        // Select GCLK4
	while (GCLK->STATUS.bit.SYNCBUSY)
		; // Wait for synchronization

	// Feed GCLK4 to TC4 and TC5
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |     // Enable GCLK4 to TC4 and TC5
	                   GCLK_CLKCTRL_GEN_GCLK4 | // Select GCLK4
	                   GCLK_CLKCTRL_ID_TC4_TC5; // Feed the GCLK4 to TC4 and TC5
	while (GCLK->STATUS.bit.SYNCBUSY)
		; // Wait for synchronization
}


/*
 * One shot timer used to call a callback in x milliseconds
 */
static void _setup_timer4(uint16_t clk_div_)
{
	REG_TC4_CTRLA |= TC_CTRLA_SWRST; // Reset timer

	NVIC_SetPriority(TC4_IRQn, 0); // Set the Nested Vector Interrupt Controller
	// (NVIC) priority for TC4 to 0 (highest)
	NVIC_EnableIRQ(TC4_IRQn); // Connect TC4 to Nested Vector Interrupt Controller (NVIC)

	REG_TC4_CTRLBSET |= TC_CTRLBCLR_ONESHOT; // Select one-shot operation

	REG_TC4_INTFLAG |= TC_INTFLAG_OVF;  // Clear the interrupt flags
	REG_TC4_INTENSET = TC_INTENSET_OVF; // Enable TC4 interrupts

	uint16_t prescale = 0;
	// debug("clk_div_: ");
	// debug(clk_div_);
	switch (clk_div_) {
	case 1:
		prescale = TC_CTRLA_PRESCALER(0);
		break;
	case 2:
		prescale = TC_CTRLA_PRESCALER(1);
		break;
	case 4:
		prescale = TC_CTRLA_PRESCALER(2);
		break;
	case 8:
		prescale = TC_CTRLA_PRESCALER(3);
		break;
	case 16:
		prescale = TC_CTRLA_PRESCALER(4);
		break;
	case 64:
		prescale = TC_CTRLA_PRESCALER(5);
		break;
	default:
		prescale = TC_CTRLA_PRESCALER(6);
		break;
	}
	REG_TC4_CTRLA |= prescale | TC_CTRLA_WAVEGEN_MFRQ | TC_CTRLA_MODE_COUNT16;
	while (TC4->COUNT16.STATUS.bit.SYNCBUSY)
		; // Wait for synchronization
}

/*
 * Timer5 used to poll buttons at frequency TIMER_TIMER5_FREQ_TCC0
 * Configure the timer but do not start it
 */
static void _setup_timer5(uint32_t sample_rate)
{
	const uint16_t tcc0 = (freq_timer5_ / sample_rate) - 1;

	REG_TC5_CTRLA |= TC_CTRLA_SWRST; // Reset timer5

	NVIC_SetPriority(TC5_IRQn, 0); // Set the Nested Vector Interrupt Controller
	// (NVIC) priority for TC5 to 0 (highest)
	NVIC_EnableIRQ(TC5_IRQn); // Connect TC5 to Nested Vector Interrupt Controller (NVIC)

	REG_TC5_INTFLAG |= TC_INTFLAG_OVF;  // Clear the interrupt flags
	REG_TC5_INTENSET = TC_INTENSET_OVF; // Enable TC5 interrupts

	REG_TC5_CTRLA |=
	    TC_CTRLA_PRESCALER_DIV1_Val | TC_CTRLA_WAVEGEN_MFRQ | TC_CTRLA_MODE_COUNT16; // Timer in 16 bits mode
	while (TC5->COUNT16.STATUS.bit.SYNCBUSY)
		; // Wait for synchronization

	REG_TC5_COUNT16_CC0 = tcc0; // Set the TC5 CC0 register to match TIMER_TIMER5_FREQ_TCC0
	while (TC5->COUNT16.STATUS.bit.SYNCBUSY)
		; // Wait for synchronization
}
//----

void call_me_in_x_ms(uint32_t ms, void (*cb)(void))
{
	float    freq = 1000 / (float) (ms);
	uint16_t tcc0 = (float) (freq_timer2_) / freq;

	TC4->COUNT16.CTRLA.bit.ENABLE = 0;
	while (TC4->COUNT16.STATUS.bit.SYNCBUSY)
		; // Wait for synchronization

	TC4->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF; // Clear interrupt flag if any

	REG_TC4_COUNT16_CC0 = tcc0; // Set the TC4 CC0 register to some arbitary value
	while (TC4->COUNT16.STATUS.bit.SYNCBUSY)
		; // Wait for synchronization

	TC4->COUNT16.CTRLA.bit.ENABLE = 1;
	while (TC4->COUNT16.STATUS.bit.SYNCBUSY)
		; // Wait for synchronization

	cb_timer4_ = cb;
}

/* Configure timer4, do not start it */
void setup_timer4()
{
	debug("setup_timer4\n");
	_setup_timer4(TIMER_TIMER4_FREQ_DIV);
}


/* Configure timer5, do not start it */
void setup_timer5(const uint32_t sample_rate)
{
	debug("setup_timer5\n");
	_setup_timer5(sample_rate);
}

/* Register a cb for timer5 and start the timer, setup_timer5 must be called first */
void start_timer5(void (*cb)(void))
{
	cb_timer5_                    = cb;
	TC5->COUNT16.CTRLA.bit.ENABLE = 1;
	while (TC5->COUNT16.STATUS.bit.SYNCBUSY)
		; // Wait for synchronization
}

void stop_timer5(void)
{
	TC5->COUNT16.CTRLA.bit.ENABLE = 0;
	while (TC5->COUNT16.STATUS.bit.SYNCBUSY)
		; // Wait for synchronization
}
//---------------------------------------------------------------------------

void TC4_Handler()
{
	if (TC4->COUNT16.INTFLAG.bit.OVF && (TC4->COUNT16.INTENSET.bit.OVF)) {
		/*write your interrupt code here*/
		/*debug("int: ");
	    debug(counter++);
	    debug(", count: ");
	    debug(REG_TC4_COUNT16_COUNT);*/
		TC4->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF;

		if (cb_timer4_ != NULL) {
			(*cb_timer4_)();
		}
	}
}

void TC5_Handler()
{
	if (cb_timer5_ != NULL) {
		(*cb_timer5_)();
	}
	TC5->COUNT16.INTFLAG.reg = 0xFF;
}
