#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Event ev = stateFunctions[fsm.current](); // calls the function of fsm.current and saves the enumerated next event returned by that function in ev
  //if (ev != EV_NULL) fsm_dispatch(ev);  // if the event returned by the state function is not null, dispatch the event to determine the next state
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}