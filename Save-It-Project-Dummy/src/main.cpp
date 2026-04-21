#include <Arduino.h>
#include "gpio.h"
#include "bluetooth.h"

void setup() {
  Serial.begin(9600);  // debug output

  pinMode(PIN_FORCE_SENSOR, INPUT_PULLUP);   // pull-up for snap dome
  pinMode(PIN_THERMISTOR,   INPUT);          // analog input, no pull-up
  pinMode(DEBUG_LED_PIN,    OUTPUT);         // onboard LED for debugging

  bt_init();  // initialize Bluetooth
}

void loop() {
    bool force = !digitalRead(PIN_FORCE_SENSOR); // snap dome pressed is active LOW
    uint16_t therm = analogRead(PIN_THERMISTOR); // thermistor reading (0-1023)
    bt_sendSensorData(force, therm);

    if (force) {
        // snap dome pressed — rapid blink
        digitalWrite(DEBUG_LED_PIN, HIGH);
        delay(50);
        digitalWrite(DEBUG_LED_PIN, LOW);
        delay(50);
        digitalWrite(DEBUG_LED_PIN, HIGH);
        delay(50);
        digitalWrite(DEBUG_LED_PIN, LOW);
        delay(350);  // total still ~500ms

    } else if (therm < 400) {
        // thermistor reading below threshold — slow blink
        digitalWrite(DEBUG_LED_PIN, HIGH);
        delay(250);
        digitalWrite(DEBUG_LED_PIN, LOW);
        delay(250);
    } else {
        // nothing detected — solid off
        digitalWrite(DEBUG_LED_PIN, LOW);
        delay(500);
    }
}
