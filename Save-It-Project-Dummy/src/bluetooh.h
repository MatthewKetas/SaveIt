// bluetooth.h (dummy side)
#pragma once
#include <Arduino.h>

#define BT_BAUD     9600
#define BT_SERIAL   Serial2   // or whichever UART the dummy uses

void bt_init();
void bt_sendSensorData(bool force, uint16_t therm);