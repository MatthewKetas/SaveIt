// fsm.h - for things that will be accessed outside of fsm.cpp 
#pragma once
#include <Arduino.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------
// States and Events: shared types used across all files

typedef enum { //STATES
  ST_SYNC,                // connecting bluetooth/reconnecting and ensuring all periphial connections, ensuring leads are on dummy
  ST_START,               // when ready to start the game (has start up stuff)
  ST_DEFIB_IT,            // has charge it and shock it
  ST_BREATHE_IT,          // actions to push  air past the airflow sensor
  ST_PUMP_IT,             // actions to activate force sensor
  ST_GAME_OVER,           // shows score of the game and end game screen/info
  ST_NULL                 // null state for transition table, also used for invalid state
} State; //uses typedif to give the enum struct an alias of State, uses state when referencing an instance of the enum

typedef enum { // EVENTS, caused by action or inaction of user or by the system, for referencing changes in the transition table
    EV_SYNC_OK,
    EV_START_BTN,
    EV_PROMPT_DEFIB,
    EV_PROMPT_BREATHE,
    EV_PROMPT_PUMP,
    EV_FAIL,
    EV_RESTART,
    EV_NULL
} Event;

// ----------------------------------------------------------------------------------------------------------------------------------------------
// FSM Public Interface: functions exposed to other files

void fsm_init();

void fsm_dispatch(Event ev);

//void fsm_tick();