/*
  ST7789 2" LCD + DFPlayer Mini — ESP32-WROOM-32
  Animated sine wave on display, plays track 0001 every 3 seconds

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

// ----- Playback Timer -----
#define PLAY_INTERVAL 3000  // Play track every 3 seconds

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Two buffers: current wave and previous wave Y values
// After setRotation(1), width=320, height=240
int curY[320];
int oldY[320];
int offset = 0;
unsigned long lastPlayTime = 0;

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

void playTrack(int track) {
  // Command 0x03 = play specific track by number
  execute_CMD(0x03, highByte(track), lowByte(track));
}

// ============ Setup ============

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  // --- Initialize DFPlayer ---
  delay(1000);
  execute_CMD(0x3F, 0, 0);   // Reset / init
  delay(500);
  setVolume(20);              // Volume 0-30
  delay(500);
  playTrack(1);               // Play track 0001 immediately
  lastPlayTime = millis();
  Serial.println("DFPlayer started — playing track 0001");

  // --- Initialize LCD ---
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.init(SCREEN_W, SCREEN_H, SPI_MODE0);
  tft.setRotation(1);        // Landscape: 320 x 240
  tft.fillScreen(BG_COLOR);

  // Pre-calculate initial wave into both buffers
  int w = tft.width();
  int h = tft.height();
  int centerY = h / 2;
  for (int x = 0; x < w; x++) {
    float angle = (float)(x + offset) * 6.2832 / (float)w;
    curY[x] = centerY + (int)(AMPLITUDE * sin(angle));
    oldY[x] = curY[x];
  }

  // Draw initial wave as connected lines
  for (int x = 0; x < w - 1; x++) {
    tft.drawLine(x, curY[x], x + 1, curY[x + 1], LINE_COLOR);
  }

  Serial.println("LCD initialized");
}

// ============ Main Loop ============

void loop() {
  // Play track 0001 every 3 seconds
  if (millis() - lastPlayTime >= PLAY_INTERVAL) {
    playTrack(1);
    lastPlayTime = millis();
    Serial.println("Playing track 0001");
  }

  // Animate sine wave
  int w = tft.width();
  int h = tft.height();
  int centerY = h / 2;

  // Calculate new wave positions
  for (int x = 0; x < w; x++) {
    float angle = (float)(x + offset) * 6.2832 / (float)w;
    curY[x] = centerY + (int)(AMPLITUDE * sin(angle));
  }

  // Erase old wave and draw new wave as connected line segments
  for (int x = 0; x < w - 1; x++) {
    // Erase old segment in black
    tft.drawLine(x, oldY[x], x + 1, oldY[x + 1], BG_COLOR);
    // Draw new segment in green
    tft.drawLine(x, curY[x], x + 1, curY[x + 1], LINE_COLOR);
  }

  // Copy current wave to old buffer for next frame
  memcpy(oldY, curY, sizeof(curY));

  // Shift the wave for next frame
  offset += WAVE_SPEED;
  if (offset >= w) {
    offset -= w;
  }
}