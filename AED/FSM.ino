/*
AED Control Loop for ESP32
*/

#include <Arduino.h>

// Macro Definitions
#define STARTDELAY 0;

// ----------------------------------------------------------------------------------------------------------------------------------------------
// States, Events, and Transition Table: for traversing the states of the FSM using the current state and the event that is triggered
// ----------------------------------------------------------------------------------------------------------------------------------------------

typedef enum { //STATES
  ST_SYNC,                // connecting bluetooth/reconnecting and ensuring all periphial connections, ensuring leads are on dummy
  ST_START,               // when ready to start the game (has start up stuff)
  ST_DEFIB_IT,            // has charge it and shock it
  ST_BREATHE_IT,          // actions to push  air past the airflow sensor
  ST_PUMP_IT,             // actions to activate force sensor
  ST_GAME_OVER,           // shows score of the game and end game screen/info
  ST_NULL                 // null state for transition table, also used for invalid state
} State; //uses typedif to give the enum struct an alias of State, uses state when referencing an instance of the enum

typedef enum { //EVENTS, caused by action or inaction of user or by the system, for referencing changes in the transition table
  //update with full correct events as needed
    EV_SYNC_OK,
    EV_START_BTN,
    EV_PROMPT_DEFIB,
    EV_PROMPT_BREATHE,
    EV_PROMPT_PUMP,
    EV_FAIL,
    EV_RESTART,
    EV_NULL
} Event;

// array called to function pointers for the state functions, used to determine which function to call based on the next state
static Event (*stateFunctions[ST_NULL])(void) = { //The parantheses tell the compiler it's an array of functions with argument void and returning void, ex: void (*arr[6])(void) 
    // One function pointer for each state
    
};

// if (fsm.current == ST_START)     fsm_dispatch(EV_START_BTN);
// if (fsm.current == ST_GAME_OVER) fsm_dispatch(EV_RESTART);

static const State transTable[ST_NULL][EV_NULL] = { //use nulls to define the size due to zero indexing of the enums
    //                              SYNC_OK   START_BTN   PROMPT_DEFIB    PROMPT_BREATHE    PROMPT_PUMP   FAIL    RESTART 
    /* ST_SYNC                  */ {ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL},
    /* ST_START                 */ {ST_NULL, ST_NULL ,ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL},
    /* ST_DEFIB_IT              */ {ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL},
    /* ST_BREATHE_IT            */ {ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL},
    /* ST_PUMP_IT               */ {ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL},
    /* ST_GAME_OVER             */ {ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL, ST_NULL},
};



// ----------------------------------------------------------------------------------------------------------------------------------------------
// FSM Functions and Variables: for dispatching events and handling state transitions, also for keeping track of the current state and score
// ----------------------------------------------------------------------------------------------------------------------------------------------
static struct {
    State    current; // saves the current state of the FSM, used for referencing the current state and for traversing the transition table
    uint8_t  score;   // saves the current score of the game, used for referencing the score and for displaying it on the game over screen
    // uint32_t promptStartMs; // saves the timestamp when the prompt was started
} fsm;

void fsm_init() {
    fsm.current       = ST_SYNC;
    fsm.score         = 0;
    // fsm.promptStartMs = 0;
    // more stuff?
}

//only called in main loop, taking event as an argument from state function that was called last
void fsm_dispatch(Event ev) { // takes in current event and state and references the transition table to determine the next state
    State next = transTable[fsm.current][ev];   // references the transition table with the current state and event to determine the next state
    if (next == ST_NULL) return;             // if the next state is invalid, do not change states
    fsm.current = next;                         // sets the current state to the state it is about to change to
}

//main loop will be in a seperate file eventually 
void loop() {
    Event ev = stateFunctions[fsm.current](); // calls the function of fsm.current and saves the enumerated next event returned by that function in ev
    if (ev != EV_NULL) fsm_dispatch(ev);  // if the event returned by the state function is not null, dispatch the event to determine the next state
}

