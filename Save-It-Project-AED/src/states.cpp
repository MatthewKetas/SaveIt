// States.cpp
#include <Arduino.h>
#include "states.h"
#include "fsm.h"
#include "gpio.h"
#include "bluetooth.h"
#include "lcd.h"
#include "audio.h"

Event syncState(){
    // Check if bluetooth and peripherals are connected and leads are on dummy, if not dispatch EV_SYNC to stay in this state, if so dispatch EV_SYNC_OK to transition to start state
    //display "Syncing..." on LCD screen
    // TODO: display "Connecting..." on LCD
    // TODO: check BT connected and peripherals ok
    // if (bt_connected() && peripheralsOk()) return EV_SYNC_OK;
    return EV_NULL;
}
Event startState(){
    // display "Press Start Button to Begin!" on LCD screen, wait for start button press to dispatch EV_START_BTN and transition to next state, show the start screen
    // set up peripherals and ensure everything is ready to start the game, reset score to 0, etc.
    // TODO: display "Press Start to Begin!" on LCD
    // TODO: poll start button
    // if (digitalRead(PIN_START_BTN) == LOW) {
    //     pickNextChallenge();
    // }
}
Event defibState(){
    // TODO: display "SHOCK IT!" on LCD
    // TODO: read shock sensor from dummy over BT
    // if (shockDetected()) {
    //     pickNextChallenge();
    // }
    return EV_NULL;
}
Event blowState(){
       // TODO: display "BLOW IT!" on LCD
    // TODO: read thermistor from dummy over BT
    // if (breathDetected()) {
    //     pickNextChallenge();
    // }
    return EV_NULL;
}
Event pumpState(){
       // TODO: display "PUMP IT!" on LCD
    // TODO: read force sensor from dummy over BT
    // if (forceDetected()) {
    //     pickNextChallenge();
    // }
    return EV_NULL;
}
Event gameOverState(){
       // TODO: display "Game Over! Score: X" on LCD
    // TODO: poll start button for restart
    // if (digitalRead(PIN_START_BTN) == LOW) return EV_START_BTN;
    return EV_NULL;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Challenge Picker: randomly selects next challenge and dispatches EV_PROMPT_*
Event pickNextChallenge(){
    switch ((int)random(3)) {
        case 0: return EV_PROMPT_DEFIB;
        case 1: return EV_PROMPT_BLOW;
        case 2: return EV_PROMPT_PUMP;
        default: return EV_PROMPT_DEFIB;  // default, should never enter
    }
}