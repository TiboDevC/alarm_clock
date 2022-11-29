#include <SD.h>
#include <SPI.h>

#include "audio.h"
#ifdef __cplusplus

extern "C" {
#endif /* __cplusplus */

#include "music.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

int music_init()
{
	SerialUSB.print("[music] Initializing...");

	if (!SD.begin(4)) {
		SerialUSB.println(" failed!");
		return -1;
	}
	AudioZero.begin(44100);
	SerialUSB.println(" done!");
	return 0;
}

/*
 * https://audio.online-convert.com/convert-to-wav
 * 8 bits
 * 24000Hz
 * Mono
 */

void music_play()
{
	// open wave file from sdcard
	File myFile = SD.open("STR1.wav");

	if (!myFile) {
		// if the file didn't open, print an error and stop
		SerialUSB.println("error opening music file");

		while (true)
			;
	}

	SerialUSB.println("Playing");

	// until the file is not finished
	AudioZero.play(myFile);

	SerialUSB.println("End of file. Thank you for listening!");
}
