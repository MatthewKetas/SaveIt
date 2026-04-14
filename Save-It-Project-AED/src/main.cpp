#include <Arduino.h>
#include "fsm.h"

#include "bluetooth.h"
#include "lcd.h"
#include "audio.h"

void setup() {
  // Serial.begin(115200);  //for debug prints during development
  fsm_init();
  bt_init();
  lcd_init();
  audio_init();
}

void loop() {
  fsm_tick(); // handles BT check, timeout, state polling, LCD all inside
}
