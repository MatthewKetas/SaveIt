// fsm.h - for things that will be accessed outside of fsm.cpp 
#pragma once  // ensures that the header file is only included once in the compilation process, preventing redefinition errors
#include <Arduino.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------
// States and Events: shared types used across all files

typedef enum { //STATES
  ST_SYNC,                // connecting bluetooth/reconnecting and ensuring all periphial connections, ensuring leads are on dummy
  ST_START,               // when ready to start the game (has start up stuff)
  ST_DEFIB_IT,            // has charge it and shock it
  ST_BLOW_IT,          // actions to push  air past the airflow sensor
  ST_PUMP_IT,             // actions to activate force sensor
  ST_GAME_OVER,           // shows score of the game and end game screen/info
  ST_NULL                 // null state for transition table, also used for invalid state
} State; //uses typedif to give the enum struct an alias of State, uses state when referencing an instance of the enum

typedef enum { // EVENTS, caused by action or inaction of user or by the system, for referencing changes in the transition table
    EV_SYNC,              // lost connection or peripheral connection, or leads are off dummy, or just syncing up at the start of the game
    EV_SYNC_OK,           // connection is good and leads are on dummy, ready to start the game or regained connection
    EV_START_BTN,         // start button is pressed, used to transition between a few states like game over to star
    EV_PROMPT_DEFIB,      // prompt for defibrillation, used to transition from any state to defib
    EV_PROMPT_PUMP,       // prompt for pumping, used to transition from any state to pump
    EV_PROMPT_BLOW,    // prompt for breathing, used to transition from any state to BLOW
    EV_FAIL,              // fail event, used to transition to game over state
    EV_NULL               // null event for transition table, also used for invalid event
} Event;

// ----------------------------------------------------------------------------------------------------------------------------------------------
// FSM Public Interface: functions exposed to other files

void fsm_init();

void fsm_dispatch(Event ev);

//void fsm_tick();