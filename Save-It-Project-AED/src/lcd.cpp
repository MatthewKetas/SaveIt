// lcd.cpp
#include <Arduino.h>
#include "lcd.h"
#include "gpio.h"

static Adafruit_ST7789 tft = Adafruit_ST7789(LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN); // hardware SPI instance for ST7789 display

// ----------------------------------------------------------------------------------------------------------------------------------------------
// EKG Buffer — stores last SCREEN_W y-values for scrolling effect
static int16_t  ekgBuf[SCREEN_W];          // y-value for each column
static EKGState ekgState    = EKG_IDLE;    // current EKG behavior
static uint16_t ekgColor    = EKG_COLOR_IDLE;
static int16_t  ekgPhase    = 0;           // phase counter for waveform generation
static int8_t   spikeSample = 0;           // which sample of the spike we're on

// spike shape - has offset values to simulate a sort of heart rhythm
static const int8_t spikeShape[] = {
    0, 0, 0,           // flat lead in
    -3, -3,            // small P wave up
    0, 0,              // back to baseline
    -8, -15,           // Q dip down
    -25, -35, -40,     // sharp R peak up
    -20, -5,           // back down
    15, 25, 20,        // S wave down
    10, 5, 2,          // T wave recovery
    0, 0, 0            // flat tail
};
static const uint8_t SPIKE_LEN   = sizeof(spikeShape);

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Helper funcitons (Internal to lcd)
static void lcd_clear() {  // clear entire screen and reset text settings
    tft.fillScreen(COLOR_BG);
    tft.setTextWrap(false);
}

static void lcd_printCentered(const char* text, uint8_t sz, uint16_t color, int16_t y) { // prints text centered at given y coordinate
    tft.setTextSize(sz);
    tft.setTextColor(color);
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
    tft.setCursor((tft.width() - w) / 2, y);
    tft.print(text);
}

static void lcd_drawScore(uint8_t score) { // draws the score in the top right corner of the header
    tft.fillRect(SCREEN_W - 90, 0, 90, 25, COLOR_BG); // clear previous score
    tft.setTextSize(2);
    tft.setTextColor(COLOR_HIGHLIGHT);
    tft.setCursor(SCREEN_W - 85, 5);
    tft.print("Score:");
    tft.print(score);
}

// generates next EKG y-value based on current state
static int16_t ekgNextSample() {
    int16_t centerY = EKG_CENTER_Y; // baseline for all states

    switch (ekgState) {
        case EKG_IDLE: { // for when we're waiting for the player to start, a slow gentle beat to show we're alive and waiting for input
            // slow gentle beat every 100 ish samples
            ekgPhase++;
            if (ekgPhase % 100 == 0) return centerY - 10;  // small blip
            if (ekgPhase % 100 == 1) return centerY + 5;
            return centerY;
        }

        case EKG_ACTIVE: { // steady heartbeat rhythm 
            // steady heartbeat — PQRST every 60 samples
            ekgPhase++;
            int16_t pos = ekgPhase % 120;
            if (pos < (int16_t)SPIKE_LEN) return centerY + spikeShape[pos];
            return centerY;
        }

        case EKG_SUCCESS: { // one big spike to celebrate success, then back to active rhythm
            // one big spike then back to active
            if (spikeSample < SPIKE_LEN) {
                int16_t val = centerY + (int16_t)(spikeShape[spikeSample] * 1.5f);
                spikeSample++;
                return val;
            }
            ekgState    = EKG_ACTIVE;   // return to active after spike
            ekgColor    = EKG_COLOR_ACTIVE;
            spikeSample = 0;
            return centerY;
        }

        case EKG_FAIL: // flatline for failure
            return centerY;             // flatline
    }
    return centerY;
}

void lcd_drawHeader(const char* prompt, uint16_t promptColor, uint8_t score) {
    tft.fillRect(0, 0, SCREEN_W, HEADER_H, COLOR_BG);  // clear header
    
    // "SaveIt!" top left
    tft.setTextSize(2);
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(5, 8);
    tft.print("SaveIt!");
    
    // prompt centered
    lcd_printCentered(prompt, 3, promptColor, 35);
    
    // score top right
    lcd_drawScore(score);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------
// lcd_init
void lcd_init() {
    pinMode(LCD_BL_PIN, OUTPUT);
    digitalWrite(LCD_BL_PIN, HIGH);
    tft.init(SCREEN_H, SCREEN_W, SPI_MODE0); // initialize display (its in portrait mode by default, so we swap width and height here and rotate below
    tft.setRotation(1);
    tft.fillScreen(COLOR_BG);

    // initialize EKG buffer to center
    for (int i = 0; i < SCREEN_W; i++) ekgBuf[i] = EKG_CENTER_Y;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------
// lcd_setEKGState: change EKG behavior, called from state functions
void lcd_setEKGState(EKGState state) {
    ekgState    = state;
    spikeSample = 0;
    ekgPhase    = 0;
    switch (state) {
        case EKG_IDLE:    ekgColor = EKG_COLOR_IDLE;    break;
        case EKG_ACTIVE:  ekgColor = EKG_COLOR_ACTIVE;  break;
        case EKG_SUCCESS: ekgColor = EKG_COLOR_SUCCESS; break;
        case EKG_FAIL:    ekgColor = EKG_COLOR_FAIL;    break;
    }
}

// ----------------------------------------------------------------------------------------------------------------------------------------------
// lcd_updateEKG: scrolls EKG left by 1 pixel, called every tick
void lcd_updateEKG() {
    // erase leftmost column
    tft.drawFastVLine(0, EKG_Y, EKG_H, COLOR_BG);

    // shift buffer left
    for (int i = 0; i < SCREEN_W - 1; i++) {
        int16_t oldY = ekgBuf[i];
        int16_t newY = ekgBuf[i + 1];
        tft.drawPixel(i, oldY, COLOR_BG);           // erase old
        tft.drawPixel(i, newY, ekgColor);           // draw shifted
        ekgBuf[i] = newY;
    }

    // generate and draw new sample on right edge
    int16_t newY = ekgNextSample();
    newY = constrain(newY, EKG_Y, EKG_Y + EKG_H - 1);  // keep in bounds
    tft.drawPixel(SCREEN_W - 1, newY, ekgColor);
    ekgBuf[SCREEN_W - 1] = newY;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------
// lcd_updateScore: redraws score in header without clearing whole screen
void lcd_updateScore(uint8_t score) {
    lcd_drawScore(score);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------
// Screen functions — draw header only, EKG runs via lcd_updateEKG() every tick
void lcd_showSyncScreen() {
    lcd_clear();
    lcd_printCentered("SaveIt!", 3, COLOR_TEXT, 10);
    lcd_printCentered("Connecting...", 2, COLOR_HIGHLIGHT, 45);
    lcd_setEKGState(EKG_IDLE);
}

void lcd_showStartScreen() {
    lcd_clear();
    lcd_printCentered("SaveIt!", 3, COLOR_TEXT, 10);
    lcd_printCentered("Press Start to Begin!", 1, COLOR_TEXT, 50);
    lcd_setEKGState(EKG_IDLE);
}

void lcd_showDefibScreen(uint8_t score) {
    lcd_drawHeader("DEFIB-IT!", COLOR_PROMPT_DEFIB, score);
    lcd_setEKGState(EKG_ACTIVE);
}

void lcd_showBlowScreen(uint8_t score) {
    lcd_drawHeader("BREATHE IT!", COLOR_PROMPT_BLOW, score);
    lcd_setEKGState(EKG_ACTIVE);
}

void lcd_showPumpScreen(uint8_t score) {
    lcd_drawHeader("COMPRESS IT!", COLOR_PROMPT_PUMP, score);
    lcd_setEKGState(EKG_ACTIVE);
}

void lcd_showGameOverScreen(uint8_t score) {
    lcd_clear();
    lcd_printCentered("GAME OVER", 3, COLOR_FAIL, 10);
    char scoreStr[16];
    snprintf(scoreStr, sizeof(scoreStr), "Score: %d", score);
    lcd_printCentered(scoreStr, 2, COLOR_TEXT, 50);
    lcd_printCentered("Press Start to play again", 1, COLOR_TEXT, 70);
    lcd_setEKGState(EKG_FAIL);
}

void lcd_showSyncStatus(bool btOk, bool padsOk) {
    tft.fillRect(0, 40, SCREEN_W, 40, COLOR_BG);  // clear status area only
    if (!btOk)   lcd_printCentered("No BT connection",    1, COLOR_FAIL, 45);
    if (!padsOk) lcd_printCentered("Place pads on dummy", 1, COLOR_FAIL, 60);
}
