//defines all the gpio pins 
#pragma once
#include <Arduino.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Bluetooth Pins
#define BT_STATE_PIN    35      // HC-05 STATE pin — high when connected
#define BT_RX_PIN       27      // ESP32 UART RX pin connected to HC-05 TX
#define BT_TX_PIN       26      // ESP32 UART TX pin connected to HC-05 RX
#define BT_EN_PIN       14      // HC-05 enable pin for resetting module

// Audio Pins
#define AUDIO_RX_PIN    16      // ESP32 UART RX — connected to DFPlayer TX
#define AUDIO_TX_PIN    17      // ESP32 UART TX — connected to DFPlayer RX
#define AUDIO_BUSY_PIN  34      // DFPlayer BUSY pin — LOW when playing


// LED Pins
#define LCD_MOSI_PIN    23      // DIN — SPI MOSI, must be hardware MOSI pin on ESP32
#define LCD_CLK_PIN     18      // CLK — SPI clock, must be hardware SCK pin on ESP32
#define LCD_CS_PIN      5       // CS  — chip select, any GPIO, pull LOW to address display
#define LCD_DC_PIN      4       // DC  — data/command select, LOW=command HIGH=pixel data
#define LCD_RST_PIN     15      // RST — reset, pull LOW briefly on startup then HIGH
#define LCD_BL_PIN      2       // BL  — backlight, PWM capable pin for brightness control


// Charge and Shock Button Pins
#define PIN_CHARGE_BTN  33      // charge button on AED enclosure
#define PIN_SHOCK_BTN   25      // shock button on AED enclosure


// Start Game Button Pins
#define PIN_START_BTN   32      // start/restart game button — active LOW

// Defib Pads (to allow game start) Pins
#define PIN_PAD_LEFT    21      // left defib pad reed switch
#define PIN_PAD_RIGHT   13      // right defib pad reed switch

//Debounce delay for buttons (ms)
#define DEBOUNCE_MS  125

// inline debounce helper — returns true on valid button press
inline bool debounceButton(uint8_t pin, uint32_t* lastPressMs, bool* lastState) { // inline allows this function to be defined in the header without causing multiple definition errors
    bool currentState = digitalRead(pin) == LOW;  // true = pressed
    
    if (currentState && !(*lastState)) {  // just pressed — falling edge
        uint32_t now = millis();
        if (now - *lastPressMs >= DEBOUNCE_MS) {
            *lastPressMs = now;
            *lastState   = true;   // latch pressed
            return true;           // valid press detected
        }
    }
    
    if (!currentState) {
        *lastState = false;  // button released — ready for next press
    }
    
    return false;
}