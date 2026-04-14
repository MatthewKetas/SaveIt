// bluetooth.cpp
#include <Arduino.h>
#include "bluetooth.h"
#include "gpio.h"

void bt_init() {
    pinMode(BT_STATE_PIN, INPUT); // STATE pin for connection status
    BT_SERIAL.begin(BT_BAUD, SERIAL_8N1, BT_RX_PIN, BT_TX_PIN); // initialize UART with specified RX and TX pins
    pinMode(EN_PIN, OUTPUT); // EN pin for resetting HC-05
}

bool bt_connected() {
    return digitalRead(BT_STATE_PIN) == HIGH; // HC-05 STATE pin is HIGH when connected
}

void bt_send(uint8_t cmd) {
    BT_SERIAL.write(cmd); // send command byte to dummy
}

bool bt_receive(SensorData* data) { // read and parse incoming sensor data, returns true if a valid packet was received and parsed, false otherwise
    // wait for start byte
    if (!BT_SERIAL.available()) return false;
    if (BT_SERIAL.read() != 0xFF) return false;

    // wait for remaining 3 bytes
    if (BT_SERIAL.available() < 3) return false; // wait for force byte and two thermistor bytes

    //TODO: Review this is all needed and corect inputs for bluetooth AED reciver, also consider if we want to do any error checking or validation on the data received from the dummy, such as checking for out of range values or invalid sensor readings, and how to handle such cases in the FSM and game logic
    data->forceDetected = BT_SERIAL.read(); // read force sensor byte, 0 or 1
    uint8_t thermHigh   = BT_SERIAL.read(); // read thermistor high byte
    uint8_t thermLow    = BT_SERIAL.read(); // read thermistor low byte

    // wait for end byte
    if (!BT_SERIAL.available()) return false;
    if (BT_SERIAL.read() != 0xFE) return false;

    // reconstruct float from two bytes
    data->thermistor = ((thermHigh << 8) | thermLow) / 100.0f; //changes the data result, which is a pointer so it will update the data in the calling function
    return true;
}