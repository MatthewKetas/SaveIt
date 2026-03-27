/*
  ST7789 2" LCD — Animated Sine Wave on ATmega1284
  240x320 IPS display, SPI interface, ST7789VW driver

  Requires libraries (install via Library Manager):
    - Adafruit GFX Library
    - Adafruit ST7735 and ST7789 Library

  Edited for ECE 1885 by: Matthew Ketas
  Last Edited: 3/26/2026
  LLMs Used: Claude Sonnet 4.6 Extended

  ---- ATmega1284 Pin Mapping (MightyCore Standard) ----
  Physical Pin 6  (PB5/MOSI) -> LCD DIN   (Arduino D5)
  Physical Pin 8  (PB7/SCK)  -> LCD CLK   (Arduino D7)
  Physical Pin 26 (PC4)      -> LCD CS    (Arduino D20)
  Physical Pin 25 (PC3)      -> LCD DC    (Arduino D19)
  Physical Pin 24 (PC2)      -> LCD RST   (Arduino D18)
  Physical Pin 20 (PD6)      -> LCD BL    (Arduino D14)
  -------------------------------------------------------
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Pin definitions — MightyCore standard pinout
#define TFT_CS   20   // PC4, physical pin 26
#define TFT_DC   19   // PC3, physical pin 25
#define TFT_RST  18   // PC2, physical pin 24
#define TFT_BL   14   // PD6, physical pin 20

// Display dimensions
#define SCREEN_W 240
#define SCREEN_H 320

// Sine wave parameters
#define AMPLITUDE  60        // Wave height in pixels (peak from center)
#define WAVE_SPEED 3         // Pixels to shift per frame (higher = faster)
#define LINE_COLOR   ST77XX_GREEN
#define BG_COLOR     ST77XX_BLACK
#define AXIS_COLOR   0x7BEF      // Dark grey in 565 RGB format

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Store previous Y values so we can erase the old wave efficiently
int prevY[SCREEN_W];
int offset = 0;

void setup() {
  // Turn on backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  // Initialize display — 240x320, SPI mode
  tft.init(SCREEN_W, SCREEN_H, SPI_MODE0);
  tft.setRotation(1); // Landscape: 320 wide x 240 tall
  tft.fillScreen(BG_COLOR);

  // Draw center axis line
  tft.drawFastHLine(0, SCREEN_H / 2, SCREEN_W, AXIS_COLOR);

  // Initialize previous Y array to center
  for (int i = 0; i < SCREEN_W; i++) {
    prevY[i] = SCREEN_H / 2;
  }
}

void loop() {
  // Note: after setRotation(1), "width" is 320 and "height" is 240
  int w = tft.width();
  int h = tft.height();
  int centerY = h / 2;

  for (int x = 0; x < w; x++) {
    // Calculate new sine Y value
    // 6.2832 = 2*PI, spread one full cycle across the screen width
    float angle = (float)(x + offset) * 6.2832 / (float)w;
    int newY = centerY + (int)(AMPLITUDE * sin(angle));

    // Erase old pixel
    tft.drawPixel(x, prevY[x], BG_COLOR);

    // Redraw axis if we erased it
    if (prevY[x] == centerY) {
      tft.drawPixel(x, centerY, AXIS_COLOR);
    }

    // Draw new pixel
    tft.drawPixel(x, newY, LINE_COLOR);

    // Store for next frame
    prevY[x] = newY;
  }

  // Redraw axis line (in case sine crossed it)
  tft.drawFastHLine(0, centerY, w, AXIS_COLOR);

  // Shift the wave for next frame
  offset += WAVE_SPEED;
  if (offset >= w) {
    offset -= w;
  }
}