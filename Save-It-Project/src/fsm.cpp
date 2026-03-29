//fsm.cpp
#include "fsm.h"
//#include "states.h"  // forward declares the six state functions

// ----------------------------------------------------------------------------------------------------------------------------------------------
// FSM Variables: private to fsm.cpp, only accessible through fsm_init, fsm_dispatch, fsm_tick
static struct { //saves static fsm struct for FSM variables (current state, score, )
    State    current; // saves the current state of the FSM, used for referencing the current state and for traversing the transition table
    uint8_t  score;   // saves the current score of the game, used for referencing the score and for displaying it on the game over screen
    // uint32_t promptStartMs; // saves the timestamp when the prompt was started
} fsm;


// ----------------------------------------------------------------------------------------------------------------------------------------------
// Transition Table: for traversing the states of the FSM using the current state and the event that is triggered

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
// State Function Pointer Array
// array called to function pointers for the state functions, used to determine which function to call based on the next state
static Event (*stateFunctions[ST_NULL])(void) = { //The parantheses tell the compiler it's an array of functions with argument void and returning void, ex: void (*arr[6])(void) 
    // One function pointer for each state
    
};



// ----------------------------------------------------------------------------------------------------------------------------------------------
// FSM Functions

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