// gpio.h (dummy side)
#pragma once
#include <Arduino.h>

// ── Input Pins ────────────────────────────────────────────────
#define PIN_FORCE_SENSOR    12    // binary snap dome force sensor — digital read
#define PIN_THERMISTOR      A5    // thermistor for breath detection — analog read 

// ── Bluetooth Pins ────────────────────────────────────────────
#define BT_RX_PIN           5     // HC-05 TX 
#define BT_TX_PIN           4     // HC-05 RX 

// ── Debug LED Pin ─────────────────────────────────────────────
#define DEBUG_LED_PIN       14     // onboard LED for debugging — active HIGH
