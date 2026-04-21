// states.h
#pragma once
#include "fsm.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------
// State Function Prototypes: for the six state functions, defined in states.cpp, used in the state function pointer array in fsm.cpp
Event syncState();
Event startState();
Event defibState();
Event blowState();
Event pumpState();
Event gameOverState();

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Challenge Picker: randomly selects next challenge and dispatches EV_PROMPT_*
Event pickNextChallenge(State currentState); // takes in the current state and randomly picks one of the three challenges to dispatch, used for transitioning from start and after successfully completing a challenge

// -----------------------------------------------------------------------------------------------------------------------------------------------
void states_reset();  // resets all static state for clean restart