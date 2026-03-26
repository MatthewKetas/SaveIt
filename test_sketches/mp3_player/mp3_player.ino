/*
  DFPlayer Mini — Continuous Playback on ATmega1284

  ---- Pin Mapping ----
  Physical Pin 16 (PD2) = RX1 -> DFPlayer TX
  Physical Pin 17 (PD3) = TX1 -> DFPlayer RX (use 1K resistor in series!)
  
  Uses hardware UART1 (Serial1) — no SoftwareSerial needed.

  NOTE: UART0 (Serial, pins 14/15) is free for Bluetooth or debug.
        If you need debug AND Bluetooth, you'll need SoftwareSerial
        on different pins for one of the three devices.
  -------------------------
*/

#define Start_Byte    0x7E
#define Version_Byte  0xFF
#define Command_Length 0x06
#define End_Byte      0xEF
#define Acknowledge   0x00

void setup() {
  Serial1.begin(9600);    // UART1 (physical pins 16/17) -> DFPlayer
  delay(1000);            // Give the DFPlayer time to boot

  execute_CMD(0x3F, 0, 0); // Send reset / init
  delay(500);

  setVolume(20);            // Volume 0-30
  delay(500);

  execute_CMD(0x11, 0, 1);  // Start playing track 1
  delay(500);
}

void loop() {
  // Listen for messages from DFPlayer
  if (Serial1.available() >= 10) {
    byte response[10];
    for (int i = 0; i < 10; i++) {
      response[i] = Serial1.read();
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
    Serial1.write(Command_line[k]);
  }
}