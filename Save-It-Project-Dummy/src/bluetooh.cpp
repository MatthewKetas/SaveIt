// bluetooth.cpp (dummy side)
#include <Arduino.h>
#include "bluetooth.h"
#include "gpio.h"

void bt_init() {
    BT_SERIAL.begin(BT_BAUD, SERIAL_8N1, BT_RX_PIN, BT_TX_PIN);  // HC-05 on hardware UART
}

void bt_sendSensorData(bool force, uint16_t therm) {
    BT_SERIAL.write(0xFF);           // start byte
    BT_SERIAL.write(force);          // force sensor — 0 or 1
    BT_SERIAL.write((therm >> 8) & 0xFF);  // thermistor high byte
    BT_SERIAL.write(therm & 0xFF);         // thermistor low byte
    BT_SERIAL.write(0xFE);           // end byte
}