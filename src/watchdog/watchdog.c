#include <Arduino.h>

void wdt_init(void)
{
	/* Disable WDT */
	REG_WDT_CTRL = 0;
	while (WDT->STATUS.bit.SYNCBUSY)
		;

	/* Disable IRQ */
	NVIC_DisableIRQ(WDT_IRQn);
	NVIC_ClearPendingIRQ(WDT_IRQn);

	/* Configure WDT to run of the low power clock */
	GCLK->CLKCTRL.reg = (uint32_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID_WDT);
	while (GCLK->STATUS.bit.SYNCBUSY)
		;

	/* Set the WDT reset timeout to 16 seconds */
	REG_WDT_CONFIG = WDT_CONFIG_WINDOW_16K_Val;
	while (WDT->STATUS.bit.SYNCBUSY)
		;

	/* Start WDT: no early interrupt + normal mode (not window mode) */
	REG_WDT_CTRL = WDT_CTRL_ENABLE;
	while (WDT->STATUS.bit.SYNCBUSY)
		;
}

void wdt_refresh(void)
{
	if (!WDT->STATUS.bit.SYNCBUSY) /* Check if the WDT registers are synchronized */
	{
		REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;
	}
}
