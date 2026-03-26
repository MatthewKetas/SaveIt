#include "SoftwareSerial.h"

// RX, TX - adjust these pins for your ATmega wiring
SoftwareSerial mySerial(10, 11);

#define Start_Byte  0x7E
#define Version_Byte 0xFF
#define Command_Length 0x06
#define End_Byte    0xEF
#define Acknowledge 0x00

void setup() {
  mySerial.begin(9600);
  delay(1000);          // Give the DFPlayer time to boot

  execute_CMD(0x3F, 0, 0); // Send reset / init
  delay(500);

  setVolume(20);           // Volume 0-30
  delay(500);

  execute_CMD(0x11, 0, 1); // Start playing track 1
  delay(500);
}

void loop() {
  // Listen for messages from DFPlayer
  if (mySerial.available() >= 10) {
    byte response[10];
    for (int i = 0; i < 10; i++) {
      response[i] = mySerial.read();
    }

    // 0x3D = track finished (SD card source)
    // 0x3C = track finished (USB source)
    if (response[3] == 0x3D || response[3] == 0x3C) {
      delay(200);
      execute_CMD(0x01, 0, 1); // Play next track
      delay(500);
      // When the last track finishes, DFPlayer wraps
      // back to track 1 automatically with 0x01
    }
  }
}

void setVolume(int volume) {
  execute_CMD(0x06, 0, volume);
  delay(500);
}

void execute_CMD(byte CMD, byte Par1, byte Par2) {
  word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);

  byte Command_line[10] = {
    Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
    Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte
  };

  for (byte k = 0; k < 10; k++) {
    mySerial.write(Command_line[k]);
  }
}