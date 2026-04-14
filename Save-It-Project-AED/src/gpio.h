//defines all the gpio pins 
#pragma once
#include <Arduino.h>

//TODO: define the actual GPIO pins for bluetooth (AED)
// ----------------------------------------------------------------------------------------------------------------------------------------------
// Bluetooth Pins
#define BT_STATE_PIN    XX      // HC-05 STATE pin — high when connected
#define BT_RX_PIN       XX      // ESP32 UART RX pin connected to HC-05 TX
#define BT_TX_PIN       XX      // ESP32 UART TX pin connected to HC-05 RX
#define BT_EN_PIN       XX      // HC-05 enable pin for resetting module

// Audio Pins
#define AUDIO_RX_PIN    XX      // ESP32 UART RX — connected to DFPlayer TX
#define AUDIO_TX_PIN    XX      // ESP32 UART TX — connected to DFPlayer RX
#define AUDIO_BUSY_PIN  XX      // DFPlayer BUSY pin — LOW when playing


// LED Pins
#define LCD_MOSI_PIN    XX      // DIN — SPI MOSI, must be hardware MOSI pin on ESP32
#define LCD_CLK_PIN     XX      // CLK — SPI clock, must be hardware SCK pin on ESP32
#define LCD_CS_PIN      XX      // CS  — chip select, any GPIO, pull LOW to address display
#define LCD_DC_PIN      XX      // DC  — data/command select, LOW=command HIGH=pixel data
#define LCD_RST_PIN     XX      // RST — reset, pull LOW briefly on startup then HIGH
#define LCD_BL_PIN      XX      // BL  — backlight, PWM capable pin for brightness control


// Charge and Shock Button Pins
#define PIN_CHARGE_BTN  XX      // charge button on AED enclosure
#define PIN_SHOCK_BTN   XX      // shock button on AED enclosure


// Start Game Button Pins
#define PIN_START_BTN   XX      // start/restart game button — active LOW

// Defib Pads (to allow game start) Pins
#define PIN_PAD_LEFT    XX      // left defib pad reed switch
#define PIN_PAD_RIGHT   XX      // right defib pad reed switch