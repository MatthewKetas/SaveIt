
// lcd.h
#pragma once
#include <Arduino.h>
#include <SPI.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Display Configuration
#define SCREEN_W        320     // landscape width
#define SCREEN_H        240     // landscape height
#define HEADER_H        80      // top 1/3 — prompt text and score
#define EKG_Y           80      // EKG graph starts here
#define EKG_H           160     // bottom 2/3 — EKG graph area
#define EKG_CENTER_Y    (EKG_Y + EKG_H / 2)  // center of EKG graph

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Colors (RGB565)
#define COLOR_BG        ST77XX_BLACK
#define COLOR_TEXT      ST77XX_WHITE
#define COLOR_SUCCESS   ST77XX_GREEN
#define COLOR_FAIL      ST77XX_RED
#define COLOR_HIGHLIGHT ST77XX_YELLOW

#define COLOR_PROMPT_DEFIB   ST77XX_YELLOW
#define COLOR_PROMPT_BLOW    ST77XX_BLUE  
#define COLOR_PROMPT_PUMP    ST77XX_MAGENTA

// ----------------------------------------------------------------------------------------------------------------------------------------------
// EKG Colors per state
#define EKG_COLOR_IDLE      ST77XX_WHITE
#define EKG_COLOR_ACTIVE    ST77XX_GREEN
#define EKG_COLOR_SUCCESS   ST77XX_YELLOW
#define EKG_COLOR_FAIL      ST77XX_RED

// ----------------------------------------------------------------------------------------------------------------------------------------------
// EKG States
typedef enum {
    EKG_IDLE,       // slow flat line with occasional small blip — waiting for input
    EKG_ACTIVE,     // steady heartbeat rhythm — challenge in progress
    EKG_SUCCESS,    // big spike — player succeeded
    EKG_FAIL        // flatline — player failed
} EKGState;

// ----------------------------------------------------------------------------------------------------------------------------------------------
// LCD Public Interface/functions
void lcd_init();                                // initialize display and backlight
void lcd_showSyncScreen();                      // "Connecting..."
void lcd_showStartScreen();                     // "Press Start to Begin!"
void lcd_showDefibScreen(uint8_t score);        // "SHOCK IT!" + EKG
void lcd_showBlowScreen(uint8_t score);         // "BLOW IT!" + EKG
void lcd_showPumpScreen(uint8_t score);         // "PUMP IT!" + EKG
void lcd_showGameOverScreen(uint8_t score);     // "Game Over! Score: X" + flatline
void lcd_showSyncStatus(bool btOk, bool padsOk);// helper for sync screen to show connection status
void lcd_updateEKG();                           // call every tick to scroll EKG
void lcd_setEKGState(EKGState state);           // change EKG behavior
void lcd_updateScore(uint8_t score);            // update score display in header
void lcd_drawHeader(const char* prompt, uint16_t promptColor, uint8_t score); // helper to draw header for challenge screens
