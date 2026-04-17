#pragma once
#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Audio Configuration
#define AUDIO_SERIAL    Serial1     // UART1 for DFPlayer Mini
#define AUDIO_BAUD      9600        // DFPlayer default baud rate
#define AUDIO_VOLUME    20          // default volume 0-30

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Track Definitions — loaded onto microSD in order
#define TRACK_START         1       // game start audio
#define TRACK_CHARGE        2       // defibrillate prompt
#define TRACK_SHOCK         3       // shock prompt
#define TRACK_BLOW          3       // blow prompt
#define TRACK_PUMP          4       // pump prompt
#define TRACK_SUCCESS       5       // successful action
#define TRACK_GAME_OVER     6       // game over audio

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Audio Public Interface
void audio_init();                  // initialize DFPlayer over UART
void audio_play(uint8_t track);     // play track by number
void audio_setVolume(uint8_t vol);  // set volume 0-30
bool audio_isBusy();                // true if track is currently playing
