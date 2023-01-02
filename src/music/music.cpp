#include <SD.h>
#include <SPI.h>

#include "audio.h"
#ifdef __cplusplus

extern "C" {
#endif /* __cplusplus */

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "music.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

int music_init(const uint8_t volume_percent)
{
	Serial.print("[music] Initializing...");

	if (!SD.begin(4)) {
		Serial.println(" failed!");
		return -1;
	}
	AudioZeroClass::begin(44100);
	AudioZeroClass::set_volume(volume_percent);
	Serial.println(" done!");
	return 0;
}

void music_deinit()
{
	AudioZeroClass::end();
}

/*
 * https://audio.online-convert.com/convert-to-wav
 * 8 bits
 * 24000Hz
 * Mono
 */

#define MUSIC_TASK_STACK_SIZE 300

static StaticTask_t _music_task_buffer;
static StackType_t  _music_stack[MUSIC_TASK_STACK_SIZE];
static TaskHandle_t _music_task_handler = NULL;

static SemaphoreHandle_t _sempahore_music = NULL;
static StaticSemaphore_t _semaphore_music_buffer;

static void _music_task(void *pvParameters)
{
	// open wave file from sdcard
	File myFile = SD.open("STR1.wav");

	if (myFile) {
		Serial.println("[music] Playing music");

		while (xSemaphoreTake(_sempahore_music, 0) == pdFALSE and myFile.available()) {
			AudioZeroClass::play(myFile);
		}
	}

	Serial.println("[music] Stop playing music");
	myFile.close();
	_music_task_handler = NULL;
	vTaskDelete(_music_task_handler);
}

void music_play()
{
	if (_sempahore_music == NULL) {
		_sempahore_music = xSemaphoreCreateBinaryStatic(&_semaphore_music_buffer);
	}
	xSemaphoreTake(_sempahore_music, 0);
	if (_music_task_handler == NULL) {
		Serial.println("[music] Start music task");
		_music_task_handler = xTaskCreateStatic(_music_task,
		                                        "Music task",
		                                        MUSIC_TASK_STACK_SIZE,
		                                        NULL,
		                                        tskIDLE_PRIORITY + 2,
		                                        _music_stack,
		                                        &_music_task_buffer);
	}
}

void music_stop()
{
	if (_sempahore_music != NULL) {
		xSemaphoreGive(_sempahore_music);
	}
}
