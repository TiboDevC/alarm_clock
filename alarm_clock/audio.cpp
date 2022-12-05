/*
 * Copyright (c) 2015 by
 Arturo Guadalupi <a.guadalupi@arduino.cc>
 Angelo Scialabba <a.scialabba@arduino.cc>
 Claudio Indellicati <c.indellicati@arduino.cc> <bitron.it@gmail.com>

 * Audio library for Arduino Zero.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include <SD.h>
#include <SPI.h>

#include "audio.h"

#ifdef __cplusplus

extern "C" {
#endif /* __cplusplus */

#include "timer.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */


/*Global variables*/
bool              __StartFlag;
volatile uint32_t __SampleIndex;
uint32_t          __HeadIndex;

static uint8_t _volume_percent = 100;
#define MUSIC_BUFFER_SIZE (1024)
static uint8_t _music_buffer[MUSIC_BUFFER_SIZE];

void AudioZeroClass::begin(uint32_t sampleRate)
{
	__StartFlag   = false;
	__SampleIndex = 0; // in order to start from the beginning

	/*Modules configuration */
	dacConfigure();
	tcConfigure(sampleRate);
}

void AudioZeroClass::end()
{
	tcDisable();
	tcReset();
	analogWrite(A0, 0);
}

void AudioZeroClass::set_volume(uint8_t volume_percent)
{
	_volume_percent = volume_percent;
}

void AudioZeroClass::play(File myFile)
{
	if (!__StartFlag) {
		myFile.read(_music_buffer, MUSIC_BUFFER_SIZE);
		__HeadIndex = 0;

		/*once the buffer is filled for the first time the counter can be started*/
		tcStartCounter();
		__StartFlag = true;
	} else {
		uint32_t current__SampleIndex = __SampleIndex;

		if (current__SampleIndex > __HeadIndex) {
			myFile.read(&_music_buffer[__HeadIndex], current__SampleIndex - __HeadIndex);
			__HeadIndex = current__SampleIndex;
		} else if (current__SampleIndex < __HeadIndex) {
			myFile.read(&_music_buffer[__HeadIndex], MUSIC_BUFFER_SIZE - 1 - __HeadIndex);
			myFile.read(_music_buffer, current__SampleIndex);
			__HeadIndex = current__SampleIndex;
		}
	}
}


/**
 * Configures the DAC in event triggered mode.
 *
 * Configures the DAC to use the module's default configuration, with output
 * channel mode configured for event triggered conversions.
 */
void AudioZeroClass::dacConfigure()
{
	analogWriteResolution(10);
	analogWrite(A0, 0);
}

/**
 * Configures the TC to generate output events at the sample frequency.
 *
 * Configures the TC in Frequency Generation mode, with an event output once
 * each time the audio sample frequency period expires.
 */
void AudioZeroClass::tcConfigure(uint32_t sampleRate)
{
	const uint32_t freq_timer5 = (SystemCoreClock / TIMER_CLOCK_FREQ_DIV);
	while (GCLK->STATUS.bit.SYNCBUSY)
		;

	tcReset();

	// Configure interrupt request
	NVIC_DisableIRQ(TC5_IRQn);
	NVIC_ClearPendingIRQ(TC5_IRQn);
	NVIC_SetPriority(TC5_IRQn, 0);
	NVIC_EnableIRQ(TC5_IRQn);

	REG_TC5_INTFLAG |= TC_INTFLAG_OVF;  // Clear the interrupt flags
	REG_TC5_INTENSET = TC_INTENSET_OVF; // Enable TC5 interrupts

	REG_TC5_CTRLA |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_WAVEGEN_MFRQ | TC_CTRLA_MODE_COUNT16;
	while (TC5->COUNT16.STATUS.bit.SYNCBUSY)
		;

	REG_TC5_COUNT16_CC0 = (uint16_t) (freq_timer5 / sampleRate - 1);
	Serial.print("CC0: ");
	Serial.println(REG_TC5_COUNT16_CC0);
	while (TC5->COUNT16.STATUS.bit.SYNCBUSY)
		;
}


bool AudioZeroClass::tcIsSyncing()
{
	return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

void AudioZeroClass::tcStartCounter()
{
	// Enable TC

	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
	while (tcIsSyncing())
		;
}

void AudioZeroClass::tcReset()
{
	// Reset TCx
	TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
	while (tcIsSyncing())
		;
	while (TC5->COUNT16.CTRLA.bit.SWRST)
		;
}

void AudioZeroClass::tcDisable()
{
	// Disable TC5
	TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
	while (tcIsSyncing())
		;
}

AudioZeroClass AudioZero;

#ifdef __cplusplus
extern "C" {
#endif

void Audio_Handler(void)
{
	uint32_t sample;
	if (__SampleIndex < MUSIC_BUFFER_SIZE - 1) {
		sample = _music_buffer[__SampleIndex++] * 100 / _volume_percent;
		analogWrite(A0, sample);
	} else {
		__SampleIndex = 0;
	}
	// Clear the interrupt
	TC5->COUNT16.INTFLAG.bit.MC0 = 1;
}

void TC5_Handler(void) __attribute__((weak, alias("Audio_Handler")));

#ifdef __cplusplus
}
#endif