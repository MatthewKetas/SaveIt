#include <Arduino.h>
#include "bluetooth.h"

void setup() {
  bt_init();  // initialize Bluetooth
}

void loop() {
    bool     force = digitalRead(PIN_FORCE_SENSOR);
    uint16_t therm = analogRead(PIN_THERMISTOR);
    bt_sendSensorData(force, therm);
    delay(50);  // send every 50ms
}
