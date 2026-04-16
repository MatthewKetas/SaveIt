#include <Arduino.h>
#include "gpio.h"
#include "bluetooth.h"

void setup() {
  Serial.begin(9600);  // debug output

  pinMode(PIN_FORCE_SENSOR, INPUT_PULLUP);   // pull-up for snap dome
  pinMode(PIN_THERMISTOR,   INPUT);          // analog input, no pull-up
  
  bt_init();  // initialize Bluetooth
}

void loop() {
    bool     force = digitalRead(PIN_FORCE_SENSOR);
    uint16_t therm = analogRead(PIN_THERMISTOR);
    bt_sendSensorData(force, therm);
    delay(50);  // send every 50ms
}
