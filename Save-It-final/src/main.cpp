#include <Arduino.h>
#include "fsm.h"

void loop() {
    Event ev = stateFunctions[fsm.current](); // calls the function of fsm.current and saves the enumerated next event returned by that function in ev
    if (ev != EV_NULL) fsm_dispatch(ev);  // if the event returned by the state function is not null, dispatch the event to determine the next state
}