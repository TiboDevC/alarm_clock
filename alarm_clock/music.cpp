#include <AudioZero.h>
#include <SD.h>
#include <SPI.h>

#include "music.h"

void music_init() {
    Serial.print("Initializing SD card...");

    if (!SD.begin(4)) {
        Serial.println(" failed!");
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

void music_play() {
    // open wave file from sdcard
    File myFile = SD.open("STR1.wav");

    if (!myFile) {
        // if the file didn't open, print an error and stop
        Serial.println("error opening music file");

        while (true)
            ;
    }

    Serial.println("Playing");

    // until the file is not finished
    AudioZero.play(myFile);

    Serial.println("End of file. Thank you for listening!");
}
