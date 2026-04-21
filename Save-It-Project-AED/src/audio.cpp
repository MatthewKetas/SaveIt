#include <Arduino.h>
#include "audio.h"
#include "gpio.h"


static DFRobotDFPlayerMini dfPlayer;  // DFPlayer object, private to audio.cpp

// ----------------------------------------------------------------------------------------------------------------------------------------------
// audio_init: initializes DFPlayer Mini over UART1, sets default volume
void audio_init() {
    AUDIO_SERIAL.begin(AUDIO_BAUD, SERIAL_8N1, AUDIO_RX_PIN, AUDIO_TX_PIN);
    
    if (!dfPlayer.begin(AUDIO_SERIAL)) {
        Serial.println("DFPlayer init failed — check wiring and SD card");
        dfPlayer.stop();
        delay(20);
    }

    dfPlayer.volume(AUDIO_VOLUME);  // set default volume
    Serial.println("DFPlayer initialized");
}

// ----------------------------------------------------------------------------------------------------------------------------------------------
// audio_play: plays a track by number, checks BUSY pin to avoid interrupting current track
void audio_play(uint8_t track) {
    if(audio_isBusy()){
        Serial.println("Unable to play track - audio player is busy.");
        return;
    }
    dfPlayer.play(track);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------
// audio_setVolume: sets volume 0-30
void audio_setVolume(uint8_t vol) {
    dfPlayer.volume(vol);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------
// audio_isBusy: returns true if a track is currently playing
// BUSY pin is LOW when playing, HIGH when idle
bool audio_isBusy() {
    return digitalRead(AUDIO_BUSY_PIN) == LOW;
}