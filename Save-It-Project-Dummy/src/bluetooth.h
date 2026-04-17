// bluetooth.h (dummy side)
#pragma once
#include <Arduino.h>
#include <SoftwareSerial.h>

#define BT_BAUD     9600

void bt_init();
void bt_sendSensorData(bool force, uint16_t therm);