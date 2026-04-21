#pragma once
#include <Arduino.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Bluetooth Configuration
#define BT_BAUD         9600    // HC-05 default baud rate
#define BT_SERIAL       Serial2 // UART2 on ESP32

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Sensor Data: received from dummy over BT
typedef struct {
    bool     forceDetected;   // binary snap dome force sensor
    float    thermistor;      // thermistor reading for breath detection
} SensorData;

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Bluetooth Public Interface
void     bt_init();                  // initialize UART and HC-05
bool     bt_connected();             // check STATE pin for connection status
void     bt_send(uint8_t cmd);       // send command to dummy
bool     bt_receive(SensorData* data); // read and parse incoming sensor data