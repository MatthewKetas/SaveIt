// bluetooth.cpp
#include <Arduino.h>
#include "bluetooth.h"
#include "gpio.h"

void bt_init() {
    pinMode(BT_STATE_PIN, INPUT); // STATE pin for connection status
    pinMode(BT_EN_PIN, OUTPUT); // EN pin for resetting HC-05
    digitalWrite(BT_EN_PIN, LOW);  // keeps HC-05 in normal mode
    BT_SERIAL.begin(BT_BAUD, SERIAL_8N1, BT_RX_PIN, BT_TX_PIN); // initialize UART with specified RX and TX pins
}

bool bt_connected() {
    return digitalRead(BT_STATE_PIN) == HIGH; // HC-05 STATE pin is HIGH when connected
}

void bt_send(uint8_t cmd) {
    BT_SERIAL.write(cmd); // send command byte to dummy
}

bool bt_receive(SensorData* data) { // read and parse incoming sensor data, returns true if a valid packet was received and parsed, false otherwise
    if (!BT_SERIAL.available()) return false;

    String line = BT_SERIAL.readStringUntil('\n');  // read "0,512\n"
    line.trim();

    if (line.length() == 0) return false;

    int commaIndex = line.indexOf(',');
    if (commaIndex == -1) return false;  // no comma — invalid

    data->forceDetected = line.substring(0, commaIndex).toInt() == 1;
    data->thermistor    = line.substring(commaIndex + 1).toFloat();

    return true;
}