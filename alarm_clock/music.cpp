#include <AudioZero.h>
#include <SD.h>
#include <SPI.h>

#include "music.h"

void music_init()
{
	SerialUSB.print("Initializing SD card...");

	if (!SD.begin(4)) {
		SerialUSB.println(" failed!");
		while (true)
			;
	}
	AudioZero.begin(44100);
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
