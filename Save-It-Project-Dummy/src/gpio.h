// gpio.h (dummy side)
#pragma once

//TODO: define the actual GPIO pins
// ── Input Pins ────────────────────────────────────────────────
#define PIN_FORCE_SENSOR    XX    // binary snap dome force sensor — digital read
#define PIN_THERMISTOR      XX    // thermistor for breath detection — analog read

// ── Bluetooth Pins ────────────────────────────────────────────
#define BT_RX_PIN           XX    // HC-05 TX → ESP32 RX
#define BT_TX_PIN           XX    // HC-05 RX → ESP32 TX