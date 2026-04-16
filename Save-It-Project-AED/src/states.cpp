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

    // show which condition is failing on the screen
    lcd_showSyncStatus(btOk, padsOk);

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
    static bool screenDrawn = false;
    if (!screenDrawn) {
        lcd_showStartScreen();
        audio_play(TRACK_START);
        screenDrawn = true;   // ← set true after drawing
    }

    // poll start button
    if (digitalRead(PIN_START_BTN) == LOW) { // on press, transition to next state  
        screenDrawn = false;  // reset for next entry
        return pickNextChallenge();  // dispatches random EV_PROMPT_*
    }

    lcd_updateEKG();
    return EV_NULL;
}
Event defibState(){
    // TODO: display "SHOCK IT!" on LCD
    // TODO: read shock sensor from dummy over BT
    // if (shockDetected()) {
    //     pickNextChallenge();
    // }
    static bool screenDrawn = false;
    if (!screenDrawn) {
        lcd_showDefibScreen(fsm_getScore());  // show prompt and score
        audio_play(TRACK_DEFIB);         // play "SHOCK IT!" audio
        screenDrawn = true;
    }

    // poll charge and shock buttons — must press charge first then shock
    if (digitalRead(PIN_CHARGE_BTN) == LOW &&
        digitalRead(PIN_SHOCK_BTN)  == LOW) {
        screenDrawn = false;
        lcd_setEKGState(EKG_SUCCESS);    // spike on success
        audio_play(TRACK_SUCCESS);
        return pickNextChallenge();
    }

    lcd_updateEKG();
    return EV_NULL;
}

Event blowState(){
       // TODO: display "BLOW IT!" on LCD
    // TODO: read thermistor from dummy over BT
    // if (breathDetected()) {
    //     pickNextChallenge();
    // }
    static bool screenDrawn = false;
    if (!screenDrawn) {
        lcd_showBlowScreen(fsm_getScore());
        audio_play(TRACK_BLOW);
        screenDrawn = true;
    }

    SensorData data;
    if (bt_receive(&data) && data.thermistor > BREATH_THRESHOLD) {
        screenDrawn = false;
        lcd_setEKGState(EKG_SUCCESS);
        audio_play(TRACK_SUCCESS);
        return pickNextChallenge();
    }

    lcd_updateEKG();
    return EV_NULL;
}
Event pumpState(){
       // TODO: display "PUMP IT!" on LCD
    // TODO: read force sensor from dummy over BT
    // if (forceDetected()) {
    //     pickNextChallenge();
    // }
    static bool screenDrawn = false;
    if (!screenDrawn) {
        lcd_showPumpScreen(fsm_getScore());
        audio_play(TRACK_PUMP);
        screenDrawn = true;
    }

    SensorData data;
    if (bt_receive(&data) && data.forceDetected) {
        screenDrawn = false;
        lcd_setEKGState(EKG_SUCCESS);
        audio_play(TRACK_SUCCESS);
        return pickNextChallenge();
    }

    lcd_updateEKG();
    return EV_NULL;
}
Event gameOverState(){
       // TODO: display "Game Over! Score: X" on LCD
    // TODO: poll start button for restart
    // if (digitalRead(PIN_START_BTN) == LOW) return EV_START_BTN;
    static bool screenDrawn = false;
    if (!screenDrawn) {
        lcd_showGameOverScreen(fsm_getScore());
        audio_play(TRACK_GAME_OVER);
        screenDrawn = true;
    }

    if (digitalRead(PIN_START_BTN) == LOW) {
        screenDrawn = false;
        return EV_START_BTN;
    }

    lcd_updateEKG();
    return EV_NULL;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Challenge Picker: randomly selects next challenge and dispatches EV_PROMPT_*
Event pickNextChallenge(){
    fsm_addScore();
    switch ((int)random(3)) {
        case 0: return EV_PROMPT_DEFIB;
        case 1: return EV_PROMPT_BLOW;
        case 2: return EV_PROMPT_PUMP;
        default: return EV_PROMPT_DEFIB;  // default, should never enter
    }
}