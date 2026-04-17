// bluetooth.cpp (dummy side)
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "bluetooth.h"
#include "gpio.h"

static SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

void bt_init() {
    BTSerial.begin(BT_BAUD);  // initialize SoftwareSerial for HC-05
}

void bt_sendSensorData(bool force, uint16_t therm) {
    BTSerial.write(0xFF);           // start byte
    BTSerial.write(force);          // force sensor — 0 or 1
    BTSerial.write((therm >> 8) & 0xFF);  // thermistor high byte
    BTSerial.write(therm & 0xFF);         // thermistor low byte
    BTSerial.write(0xFE);           // end byte
}