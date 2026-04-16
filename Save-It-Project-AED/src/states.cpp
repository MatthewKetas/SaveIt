// States.cpp
#include <Arduino.h>
#include "states.h"
#include "fsm.h"
#include "gpio.h"
#include "bluetooth.h"
#include "lcd.h"
#include "audio.h"

Event syncState(){
    static bool screenDrawn = false;
    if (!screenDrawn) {
        lcd_showSyncScreen();
        screenDrawn = true;
    }
    // check BT connection and reed switches every tick
    bool btOk   = bt_connected();
    bool padsOk = (digitalRead(PIN_PAD_LEFT)  == LOW) && (digitalRead(PIN_PAD_RIGHT) == LOW);

    if (btOk && padsOk) {
        screenDrawn = false;  // reset for next time we enter sync
        return EV_SYNC_OK;
    }

    // show which condition is failing
    if (!btOk)   lcd_printCentered("No BT connection", 1, COLOR_FAIL, 45);  // update status
    if (!padsOk) lcd_printCentered("Place pads on dummy", 1, COLOR_FAIL, 60);

    lcd_updateEKG();
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