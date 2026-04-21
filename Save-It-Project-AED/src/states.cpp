// States.cpp
#include <Arduino.h>
#include "states.h"
#include "fsm.h"
#include "gpio.h"
#include "bluetooth.h"
#include "lcd.h"
#include "audio.h"

// screen drawn flags for each state to ensure we only draw once per state entry
static bool syncScreenDrawn    = false;
static bool startScreenDrawn   = false;
static bool defibScreenDrawn   = false;
static bool blowScreenDrawn    = false;
static bool pumpScreenDrawn    = false;
static bool gameOverScreenDrawn = false;
static bool defibChargePressed = false;

// variables for preventing false positives on breath detection
static float prevThermistor = 1023.0f; // initialize to max value so any real reading will be lower and register as a drop
static uint32_t lastBlowMs = 0;
#define BLOW_COOLDOWN_MS 1750  // 1.75 seconds cooldown after blow

// debounce timestamps and states — one per button
static uint32_t lastStartMs     = 0;  static bool lastStartState    = false;
static uint32_t lastGameOverMs  = 0;  static bool lastGameOverState = false;
static uint32_t lastChargeMs    = 0;  static bool lastChargeState   = false;
static uint32_t lastShockMs     = 0;  static bool lastShockState    = false;
static uint32_t lastForceMs     = 0;  // no latch needed for the snap dome

Event syncState(){
    if (!syncScreenDrawn) {
        lcd_showSyncScreen();
        syncScreenDrawn = true;
    }
    // check BT connection and reed switches every tick
    bool btOk   = bt_connected();
    // padsOk is true if both left and right pads are connected (active LOW)
    bool padsOk = (digitalRead(PIN_PAD_LEFT)  == LOW) && (digitalRead(PIN_PAD_RIGHT) == LOW);
    // if both conditions are good, transition to next state
    if (btOk && padsOk) {
        syncScreenDrawn = false;  // reset for next time we enter sync
        return EV_SYNC_OK;
    }

    // show which condition is failing on the screen
    lcd_showSyncStatus(btOk, padsOk);

    lcd_updateEKG();
    return EV_NULL;
}

Event startState(){
    if (!startScreenDrawn) {
        lcd_showStartScreen();
        audio_play(TRACK_START);
        startScreenDrawn = true;   // ← set true after drawing
    }

    // poll start button
    if (debounceButton(PIN_START_BTN, &lastStartMs, &lastStartState)) { // on press, transition to next state  
        startScreenDrawn = false;  // reset for next entry
        return pickNextChallenge(fsm_getState());  // dispatches random EV_PROMPT_*
    }

    lcd_updateEKG();
    return EV_NULL;
}
Event defibState(){
    if (!defibScreenDrawn) {
        lcd_drawHeader("CHARGE IT!", COLOR_PROMPT_DEFIB, fsm_getScore());
        audio_play(TRACK_CHARGE);
        defibScreenDrawn = true;
        defibChargePressed = false;
    }

    if (!defibChargePressed) {
        if (debounceButton(PIN_CHARGE_BTN, &lastChargeMs, &lastChargeState)) {
            defibChargePressed = true;
            lcd_drawHeader("SHOCK IT!", COLOR_PROMPT_DEFIB, fsm_getScore());
            audio_play(TRACK_SHOCK);
        }
        // pressing shock before charge = fail
        if (debounceButton(PIN_SHOCK_BTN, &lastShockMs, &lastShockState)) {
            defibScreenDrawn = false;
            defibChargePressed = false;
            return EV_FAIL;
        }
        // fail if force or breath detected during charge phase of defib challenge
        //TODO: sensing continued input from last state 
        // SensorData data;
        // if (bt_receive(&data)) {
        //     if (data.forceDetected || data.thermistor < BREATH_THRESHOLD) {
        //         defibScreenDrawn = false;
        //         defibChargePressed = false;
        //         return EV_FAIL;
        //     }
        // }
    } else {
        if (debounceButton(PIN_SHOCK_BTN, &lastShockMs, &lastShockState)) {
            defibScreenDrawn = false;
            defibChargePressed = false;
            fsm_addScore();
            lcd_setEKGState(EKG_SUCCESS);
            audio_play(TRACK_SUCCESS);
            return pickNextChallenge(fsm_getState());
        }
        // pressing charge again after already charged = fail
        if (debounceButton(PIN_CHARGE_BTN, &lastChargeMs, &lastChargeState)) {
            defibScreenDrawn = false;
            defibChargePressed = false;
            return EV_FAIL;
        }
        // fail if force or breath detected during defib challenge
        //TODO: sensing continued input from last state 
        // SensorData data;
        // if (bt_receive(&data)) {
        //     if (data.forceDetected || data.thermistor < BREATH_THRESHOLD) {
        //         defibScreenDrawn = false;
        //         defibChargePressed = false;
        //         return EV_FAIL;
        //     }
        // }
    }

    lcd_updateEKG();
    return EV_NULL;
}

Event blowState() {
    if (!blowScreenDrawn) {
        lcd_showBlowScreen(fsm_getScore());
        audio_play(TRACK_BLOW);
        blowScreenDrawn = true;
        prevThermistor = -1.0f;  // sentinel - baseline not set yet
    }

    SensorData data;
    if (bt_receive(&data)) {
        // wrong input — force sensor during blow = fail
        //TODO: fail because of speed still sensing it when in next state
        // if (data.forceDetected) { 
        //     blowScreenDrawn = false;
        //     return EV_FAIL;
        // }
        // correct input — breath detected
        if (prevThermistor < 0.0f) {
            prevThermistor = data.thermistor;  // first reading — set baseline, don't check
        } else if (prevThermistor - data.thermistor > BREATH_DROP_THRESHOLD) {
            prevThermistor = -1.0f;            // reset to sentinel so we don't register multiple drops from one breath
            lastBlowMs = millis();             // reset blow cooldown timer on successful breath detection
            blowScreenDrawn = false;
            lcd_setEKGState(EKG_SUCCESS);
            audio_play(TRACK_SUCCESS);
            fsm_addScore();
            return pickNextChallenge(fsm_getState());
        } 
    }

    // fail if charge or shock button pressed during blow challenge
    if (debounceButton(PIN_CHARGE_BTN, &lastChargeMs, &lastChargeState) ||
        debounceButton(PIN_SHOCK_BTN,  &lastShockMs,  &lastShockState)) {
        blowScreenDrawn = false;
        return EV_FAIL;
    }

    lcd_updateEKG();
    return EV_NULL;
}

Event pumpState() {
    if (!pumpScreenDrawn) {
        lcd_showPumpScreen(fsm_getScore());
        audio_play(TRACK_PUMP);
        pumpScreenDrawn = true;
    }
    
    SensorData data;
    if (bt_receive(&data)) {
        // wrong input — breath during pump = fail
        //TODO: fail because of speed still sensing it when in next state
        // if (data.thermistor < BREATH_THRESHOLD) {
        //     pumpScreenDrawn = false;
        //     return EV_FAIL;
        // }
        // correct input — force detected
        if (data.forceDetected) {
            uint32_t now = millis();
            if (now - lastForceMs >= DEBOUNCE_MS) {
                lastForceMs = now;
                pumpScreenDrawn = false;
                lcd_setEKGState(EKG_SUCCESS);
                audio_play(TRACK_SUCCESS);
                fsm_addScore();
                return pickNextChallenge(fsm_getState());
            }
        }
    }
    // fail if start, charge, or shock buttons pressed during pump challenge
    if (debounceButton(PIN_CHARGE_BTN, &lastChargeMs, &lastChargeState) ||
        debounceButton(PIN_SHOCK_BTN,  &lastShockMs,  &lastShockState)) {
        pumpScreenDrawn = false;
        return EV_FAIL;
    }

    lcd_updateEKG();
    return EV_NULL;
}

Event gameOverState(){
    if (!gameOverScreenDrawn) {
        lcd_showGameOverScreen(fsm_getScore());
        audio_play(TRACK_GAME_OVER);
        gameOverScreenDrawn = true;
    }

    if (debounceButton(PIN_START_BTN, &lastGameOverMs, &lastGameOverState)) {
        gameOverScreenDrawn = false;
        return EV_START_BTN;
    }

    lcd_updateEKG();
    return EV_NULL;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Challenge Picker: randomly selects next challenge and dispatches EV_PROMPT_*
Event pickNextChallenge(State currentState){
    Event returnEv;
    do {
        switch ((int)random(3)) {
            case 0: returnEv = EV_PROMPT_DEFIB; break;
            case 1: returnEv = EV_PROMPT_BLOW;  break;
            case 2: returnEv = EV_PROMPT_PUMP;  break;
            default: returnEv = EV_PROMPT_DEFIB;  // default, should never enter
        }
    } while (
        (returnEv == EV_PROMPT_BLOW && currentState == ST_BLOW_IT) ||  // ensure the breath challenge is not repeated twice so thermistor can heat back up for next time
        (returnEv == EV_PROMPT_BLOW && millis() - lastBlowMs < BLOW_COOLDOWN_MS) // ensure the breath challenge is not repeated too quickly so thermistor can heat back up for next time
    );
    return returnEv;
}

// -----------------------------------------------------------------------------------------------------------------------------------------------
void states_reset() {
    // reset screen drawn flags
    syncScreenDrawn     = false;
    startScreenDrawn    = false;
    defibScreenDrawn    = false;
    blowScreenDrawn     = false;
    pumpScreenDrawn     = false;
    gameOverScreenDrawn = false;
    defibChargePressed  = false;

    // reset thermistor variables
    prevThermistor      = 1023.0f; 
    lastBlowMs          = 0;

    // reset debounce states
    lastStartMs      = 0;  lastStartState    = false;
    lastGameOverMs   = 0;  lastGameOverState = false;
    lastChargeMs     = 0;  lastChargeState   = false;
    lastShockMs      = 0;  lastShockState    = false;
    lastForceMs      = 0;
}