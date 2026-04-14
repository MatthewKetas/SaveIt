// bluetooth.cpp (dummy side)
#include "bluetooth.h"
#include "gpio.h"

void bt_init() {
    Serial.begin(BT_BAUD);  // HC-05 on hardware UART
}

void bt_sendSensorData(bool force, uint16_t therm) {
    Serial.write(0xFF);           // start byte
    Serial.write(force);          // force sensor — 0 or 1
    Serial.write((therm >> 8) & 0xFF);  // thermistor high byte
    Serial.write(therm & 0xFF);         // thermistor low byte
    Serial.write(0xFE);           // end byte
}