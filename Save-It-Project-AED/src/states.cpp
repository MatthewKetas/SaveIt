// States.cpp
#include <Arduino.h>
#include "states.h"
#include "fsm.h"
#include "gpio.h"
#include "bluetooth.h"
#include "lcd.h"
#include "audio.h"

// debounce timestamps and states — one per button
static uint32_t lastStartMs     = 0;  static bool lastStartState    = false;
static uint32_t lastGameOverMs  = 0;  static bool lastGameOverState = false;
static uint32_t lastChargeMs    = 0;  static bool lastChargeState   = false;
static uint32_t lastShockMs     = 0;  static bool lastShockState    = false;
static uint32_t lastForceMs     = 0;  // no latch needed for the snap dome

Event syncState(){
    static bool screenDrawn = false;
    if (!screenDrawn) {
        lcd_showSyncScreen();
        screenDrawn = true;
    }
    // check BT connection and reed switches every tick
    bool btOk   = bt_connected();
    // padsOk is true if both left and right pads are connected (active LOW)
    bool padsOk = (digitalRead(PIN_PAD_LEFT)  == LOW) && (digitalRead(PIN_PAD_RIGHT) == LOW);
    // if both conditions are good, transition to next state
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
    if (debounceButton(PIN_START_BTN, &lastStartMs, &lastStartState)) { // on press, transition to next state  
        screenDrawn = false;  // reset for next entry
        return pickNextChallenge();  // dispatches random EV_PROMPT_*
    }

    lcd_updateEKG();
    return EV_NULL;
}
Event defibState(){
    static bool screenDrawn   = false;
    static bool chargePressed = false;

    if (!screenDrawn) {
        lcd_drawHeader("CHARGE IT!", COLOR_PROMPT_DEFIB, fsm_getScore());
        audio_play(TRACK_CHARGE);
        screenDrawn   = true;
        chargePressed = false;
    }

    if (!chargePressed) {
        if (debounceButton(PIN_CHARGE_BTN, &lastChargeMs, &lastChargeState)) {
            chargePressed = true;
            lcd_drawHeader("SHOCK IT!", COLOR_PROMPT_DEFIB, fsm_getScore());
            audio_play(TRACK_SHOCK);
        }
        // pressing shock before charge = fail
        if (debounceButton(PIN_SHOCK_BTN, &lastShockMs, &lastShockState)) {
            screenDrawn   = false;
            chargePressed = false;
            return EV_FAIL;
        }
    } else {
        if (debounceButton(PIN_SHOCK_BTN, &lastShockMs, &lastShockState)) {
            screenDrawn   = false;
            chargePressed = false;
            fsm_addScore();
            lcd_setEKGState(EKG_SUCCESS);
            audio_play(TRACK_SUCCESS);
            return pickNextChallenge();
        }
        // pressing charge again after already charged = fail
        if (debounceButton(PIN_CHARGE_BTN, &lastChargeMs, &lastChargeState)) {
            screenDrawn   = false;
            chargePressed = false;
            return EV_FAIL;
        }
    }

    lcd_updateEKG();
    return EV_NULL;
}

Event blowState() {
    static bool screenDrawn = false;
    if (!screenDrawn) {
        lcd_showBlowScreen(fsm_getScore());
        audio_play(TRACK_BLOW);
        screenDrawn = true;
    }

    SensorData data;
    if (BT_SERIAL.available()) {
        lcd_debugPrint("data arriving");
    } else {
    lcd_debugPrint("no data");
    }   
    if (bt_receive(&data)) {
        // debug — show raw values on screen
        char buf[32];
        snprintf(buf, sizeof(buf), "T:%.1f F:%d", data.thermistor, data.forceDetected);
        lcd_debugPrint(buf);

        // actual game logic
        if (data.thermistor < BREATH_THRESHOLD) {
            screenDrawn = false;
            lcd_setEKGState(EKG_SUCCESS);
            audio_play(TRACK_SUCCESS);
            fsm_addScore();
            return pickNextChallenge();
        }
    }

    lcd_updateEKG();
    return EV_NULL;
}

Event pumpState() {
    static bool screenDrawn = false;
    if (!screenDrawn) {
        lcd_showPumpScreen(fsm_getScore());
        audio_play(TRACK_PUMP);
        screenDrawn = true;
    }

    SensorData data;
    if (bt_receive(&data)) {
        // debug — show raw values on screen
        char buf[32];
        snprintf(buf, sizeof(buf), "T:%.1f F:%d", data.thermistor, data.forceDetected);
        lcd_debugPrint(buf);

        // actual game logic
        if (data.forceDetected) {
            uint32_t now = millis();
            if (now - lastForceMs >= DEBOUNCE_MS) {
                lastForceMs = now;
                screenDrawn = false;
                lcd_setEKGState(EKG_SUCCESS);
                audio_play(TRACK_SUCCESS);
                fsm_addScore();
                return pickNextChallenge();
            }
        }
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

    if (debounceButton(PIN_START_BTN, &lastGameOverMs, &lastGameOverState)) {
        screenDrawn = false;
        return EV_START_BTN;
    }

    lcd_updateEKG();
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