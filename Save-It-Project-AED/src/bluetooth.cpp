// bluetooth.cpp
#include <Arduino.h>
#include "bluetooth.h"
#include "gpio.h"

void bt_init() {
    pinMode(BT_STATE_PIN, INPUT); // STATE pin for connection status
    pinMode(BT_EN_PIN, OUTPUT); // EN pin for resetting HC-05
    digitalWrite(BT_EN_PIN, LOW);  // keeps HC-05 in normal mode
    BT_SERIAL.begin(BT_BAUD, SERIAL_8N1, BT_RX_PIN, BT_TX_PIN); // initialize UART with specified RX and TX pins

    Serial.println("Bluetooth UART started");
    Serial.print("BT RX pin: ");
    Serial.println(BT_RX_PIN);
    Serial.print("BT TX pin: ");
    Serial.println(BT_TX_PIN);
    Serial.print("BT baud: ");
    Serial.println(BT_BAUD);
}

bool bt_connected() {
    return digitalRead(BT_STATE_PIN) == HIGH; // HC-05 STATE pin is HIGH when connected
}

void bt_send(uint8_t cmd) {
    BT_SERIAL.write(cmd); // send command byte to dummy
}

bool bt_receive(SensorData* data) { // read and parse incoming sensor data, returns true if a valid packet was received and parsed, false otherwise
    static uint32_t lastNoDataPrintMs = 0;

    if (!BT_SERIAL.available()) {
        uint32_t now = millis();
        if (now - lastNoDataPrintMs >= 1000) {
            lastNoDataPrintMs = now;
            Serial.println("BT RX: no data");
        }
        return false;
    }

    Serial.print("BT RX available bytes: ");
    Serial.println(BT_SERIAL.available());

    String line = BT_SERIAL.readStringUntil('\n');  // read "0,512\n"
    Serial.print("BT RX raw line: ");
    Serial.println(line);

    line.trim();
    Serial.print("BT RX trimmed line: ");
    Serial.println(line);

    if (line.length() == 0) {
        Serial.println("BT RX ignored: empty line");
        return false;
    }

    int commaIndex = line.indexOf(',');
    if (commaIndex == -1) {
        Serial.println("BT RX ignored: no comma found");
        return false;  // no comma — invalid
    }

    data->forceDetected = line.substring(0, commaIndex).toInt() == 1;
    data->thermistor    = line.substring(commaIndex + 1).toFloat();

    Serial.print("BT parsed forceDetected: ");
    Serial.println(data->forceDetected);
    Serial.print("BT parsed thermistor: ");
    Serial.println(data->thermistor);

    return true;
}
