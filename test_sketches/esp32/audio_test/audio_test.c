#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

#define AUDIO_RX_PIN    16  // ESP32 RX <- DFPlayer TX
#define AUDIO_TX_PIN    17  // ESP32 TX -> DFPlayer RX
#define AUDIO_BUSY_PIN  34  // LOW while playing

#define AUDIO_BAUD      9600
#define AUDIO_VOLUME    20

DFRobotDFPlayerMini dfPlayer;

void waitForTrack(uint8_t track) {
  Serial.print("Testing DFPlayer index ");
  Serial.println(track);

  dfPlayer.play(track);

  unsigned long startWait = millis();
  bool started = false;

  // Wait up to 2 seconds for BUSY to go LOW
  while (millis() - startWait < 2000) {
    if (digitalRead(AUDIO_BUSY_PIN) == LOW) {
      started = true;
      break;
    }
  }

  if (!started) {
    Serial.print("Index ");
    Serial.print(track);
    Serial.println(": did NOT start, or file is missing/bad");
    delay(1000);
    return;
  }

  Serial.print("Index ");
  Serial.print(track);
  Serial.println(": started playing");

  // Wait until BUSY goes HIGH again, or timeout after 15 seconds
  unsigned long playStart = millis();
  while (digitalRead(AUDIO_BUSY_PIN) == LOW && millis() - playStart < 15000) {
    delay(20);
  }

  if (digitalRead(AUDIO_BUSY_PIN) == HIGH) {
    Serial.print("Index ");
    Serial.print(track);
    Serial.println(": finished normally");
  } else {
    Serial.print("Index ");
    Serial.print(track);
    Serial.println(": timed out while playing");
  }

  delay(1000);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(AUDIO_BUSY_PIN, INPUT);

  Serial1.begin(AUDIO_BAUD, SERIAL_8N1, AUDIO_RX_PIN, AUDIO_TX_PIN);

  Serial.println("Starting DFPlayer index test...");

  if (!dfPlayer.begin(Serial1)) {
    Serial.println("DFPlayer init failed");
    while (true) delay(1000);
  }

  dfPlayer.volume(AUDIO_VOLUME);
  delay(500);

  int fileCount = dfPlayer.readFileCounts();
  Serial.print("DFPlayer says file count is: ");
  Serial.println(fileCount);

  // If fileCount is wrong or returns -1, still manually test 1-20.
  for (uint8_t i = 1; i <= 20; i++) {
    waitForTrack(i);
  }

  Serial.println("DFPlayer index test done.");
}

void loop() {
}
