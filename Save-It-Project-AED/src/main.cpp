#include <Arduino.h>
#include "fsm.h"

#include "gpio.h"
#include "bluetooth.h"
#include "lcd.h"
#include "audio.h"

void setup() {
  Serial.begin(9600);  //for debug prints during development

  // input pin modes with internal pull-ups
  pinMode(PIN_START_BTN,  INPUT_PULLUP);
  pinMode(PIN_PAD_LEFT,   INPUT_PULLUP);
  pinMode(PIN_PAD_RIGHT,  INPUT_PULLUP);
  pinMode(PIN_CHARGE_BTN, INPUT_PULLUP);
  pinMode(PIN_SHOCK_BTN,  INPUT_PULLUP);
  pinMode(AUDIO_BUSY_PIN, INPUT);
  pinMode(BT_STATE_PIN,   INPUT);

  //peripheral initializations
  bt_init();
  lcd_init();
  audio_init();
  fsm_init();
}

void loop() {
  fsm_tick(); // handles BT check, timeout, state polling, LCD all inside
}
