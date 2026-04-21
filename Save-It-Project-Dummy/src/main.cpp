#include <Arduino.h>
#include "gpio.h"
#include "bluetooth.h"

static const uint32_t SENSOR_SEND_INTERVAL_MS = 100;
static const uint8_t THERM_SAMPLES_PER_SEND = 4;

static uint16_t readThermistorAveraged() {
  uint32_t total = 0;

  for (uint8_t i = 0; i < THERM_SAMPLES_PER_SEND; i++) {
    total += analogRead(PIN_THERMISTOR);
  }

  return total / THERM_SAMPLES_PER_SEND;
}

void setup() {
  Serial.begin(9600);  // debug output

  pinMode(PIN_FORCE_SENSOR, INPUT_PULLUP);   // pull-up for snap dome
  pinMode(PIN_THERMISTOR,   INPUT);          // analog input, no pull-up
  pinMode(DEBUG_LED_PIN,    OUTPUT);         // onboard LED for debugging

  bt_init();  // initialize Bluetooth
}

void loop() {
    static uint32_t lastSendMs = 0;

    const uint32_t now = millis();
    const bool force = !digitalRead(PIN_FORCE_SENSOR); // snap dome pressed is active LOW
    const uint16_t therm = readThermistorAveraged();

    if (now - lastSendMs >= SENSOR_SEND_INTERVAL_MS) {
        lastSendMs = now;
        bt_sendSensorData(force, therm);
    }

    if (force) {
        // snap dome pressed — rapid blink
        digitalWrite(DEBUG_LED_PIN, ((now / 50) % 2) ? HIGH : LOW);
    } else if (therm < 400) {
        // thermistor reading below threshold — slow blink
        digitalWrite(DEBUG_LED_PIN, ((now / 250) % 2) ? HIGH : LOW);
    } else {
        // nothing detected — solid off
        digitalWrite(DEBUG_LED_PIN, LOW);
    }
}
