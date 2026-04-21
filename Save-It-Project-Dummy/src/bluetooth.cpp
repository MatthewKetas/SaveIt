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
    BTSerial.print(force);
    BTSerial.print(",");
    BTSerial.println(therm);  // sends "0,512\n"
}