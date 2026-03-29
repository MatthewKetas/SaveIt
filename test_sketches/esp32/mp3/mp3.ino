/*
  ST7789 2" LCD + DFPlayer Mini — ESP32-WROOM-32
  Animated sine wave on display, continuous audio playback

  Requires libraries (install via Library Manager):
    - Adafruit GFX Library
    - Adafruit ST7735 and ST7789 Library

  Edited for ECE 1885 by: Matthew Ketas
  Last Edited: 3/26/2026
  LLMs Used: Claude Sonnet 4.6 Extended

  ---- ESP32 Pin Mapping ----
  LCD (VSPI):
    GPIO 23 (VSPI MOSI) -> LCD DIN
    GPIO 18 (VSPI SCK)  -> LCD CLK
    GPIO  5             -> LCD CS
    GPIO  4             -> LCD DC
    GPIO 15             -> LCD RST
    GPIO  2             -> LCD BL  (also onboard LED)

  DFPlayer Mini (Serial2):
    GPIO 17 (TX2) -> DFPlayer RX (use 1K resistor in series)
    GPIO 16 (RX2) -> DFPlayer TX

  USB Serial (Serial):
    GPIO 1  (TX0) -> USB (debug, programming)
    GPIO 3  (RX0) -> USB (debug, programming)
  ---------------------------
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// ----- LCD Pin Definitions -----
#define TFT_CS    5
#define TFT_DC    4
#define TFT_RST  15
#define TFT_BL    2

// ----- Display Dimensions -----
#define SCREEN_W 240
#define SCREEN_H 320

// ----- Sine Wave Parameters -----
#define AMPLITUDE   60
#define WAVE_SPEED   3
#define LINE_COLOR  ST77XX_GREEN
#define BG_COLOR    ST77XX_BLACK
#define AXIS_COLOR  0x7BEF

// ----- DFPlayer Protocol -----
#define Start_Byte    0x7E
#define Version_Byte  0xFF
#define Command_Length 0x06
#define End_Byte      0xEF
#define Acknowledge   0x00

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

int prevY[SCREEN_W];
int offset = 0;

// ============ DFPlayer Functions ============

void execute_CMD(byte CMD, byte Par1, byte Par2) {
  word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);

  byte Command_line[10] = {
    Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
    Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte
  };

  for (byte k = 0; k < 10; k++) {
    Serial2.write(Command_line[k]);
  }
}

void setVolume(int volume) {
  execute_CMD(0x06, 0, volume);
  delay(500);
}

void checkDFPlayer() {
  // Listen for "track finished" messages from DFPlayer
  if (Serial2.available() >= 10) {
    byte response[10];
    for (int i = 0; i < 10; i++) {
      response[i] = Serial2.read();
    }

    // 0x3D = track finished (SD), 0x3C = track finished (USB)
    if (response[3] == 0x3D || response[3] == 0x3C) {
      delay(200);
      execute_CMD(0x01, 0, 1); // Play next track
      delay(500);
    }
  }
}

// ============ Setup ============

void setup() {
  Serial.begin(115200);       // USB debug
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // DFPlayer: RX=GPIO16, TX=GPIO17

  // --- Initialize DFPlayer ---
  delay(1000);
  execute_CMD(0x3F, 0, 0);   // Reset / init
  delay(500);
  setVolume(20);              // Volume 0-30
  delay(500);
  execute_CMD(0x11, 0, 1);   // Play track 1
  delay(500);
  Serial.println("DFPlayer started");

  // --- Initialize LCD ---
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.init(SCREEN_W, SCREEN_H, SPI_MODE0);
  tft.setRotation(1);        // Landscape: 320 x 240
  tft.fillScreen(BG_COLOR);
  tft.drawFastHLine(0, SCREEN_H / 2, SCREEN_W, AXIS_COLOR);

  for (int i = 0; i < SCREEN_W; i++) {
    prevY[i] = SCREEN_H / 2;
  }

  Serial.println("LCD initialized");
}

// ============ Main Loop ============

void loop() {
  // Check if a track finished and play next
  checkDFPlayer();

  // Animate sine wave
  int w = tft.width();
  int h = tft.height();
  int centerY = h / 2;

  for (int x = 0; x < w; x++) {
    float angle = (float)(x + offset) * 6.2832 / (float)w;
    int newY = centerY + (int)(AMPLITUDE * sin(angle));

    tft.drawPixel(x, prevY[x], BG_COLOR);

    if (prevY[x] == centerY) {
      tft.drawPixel(x, centerY, AXIS_COLOR);
    }

    tft.drawPixel(x, newY, LINE_COLOR);
    prevY[x] = newY;
  }

  tft.drawFastHLine(0, centerY, w, AXIS_COLOR);

  offset += WAVE_SPEED;
  if (offset >= w) {
    offset -= w;
  }
}