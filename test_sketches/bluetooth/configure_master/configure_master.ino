/*
  Configure MASTER HC-05 via Arduino Nano

  ---- WIRING ----
  Nano Pin 2  -> HC-05 TX
  Nano Pin 3  -> HC-05 RX (use voltage divider: 1K + 2K)
  Nano 5V     -> HC-05 VCC
  Nano GND    -> HC-05 GND
  -----------------

  STEPS:
  1. Run the SLAVE config sketch FIRST and copy the slave address
  2. Paste the slave address into SLAVE_ADDR below (commas not colons)
  3. Wire up this HC-05 but do NOT power it yet
  4. Flash this sketch to the Nano
  5. Open Serial Monitor at 9600 baud, line ending "Both NL & CR"
  6. Hold the HC-05 button, THEN plug in power (or reset Nano)
     — HC-05 LED should blink slowly (~2 sec) = AT mode
  7. Watch the Serial Monitor for results
*/

#include <SoftwareSerial.h>

SoftwareSerial BTSerial(2, 3); // RX, TX

// =====================================================
//  PASTE YOUR SLAVE ADDRESS HERE (use commas, not colons)
//  Example: if slave returned +ADDR:1234:56:789ABC
//  then enter "1234,56,789ABC"
// =====================================================
const char* SLAVE_ADDR = "98d3,c1,fea800";

// Helper: send an AT command and print the response
void sendCommand(const char* cmd, unsigned long waitTime = 1000) {
  Serial.print(">> Sending: ");
  Serial.println(cmd);

  BTSerial.print(cmd);
  BTSerial.print("\r\n");

  unsigned long start = millis();
  Serial.print("<< Response: ");

  while (millis() - start < waitTime) {
    if (BTSerial.available()) {
      Serial.write(BTSerial.read());
    }
  }
  Serial.println();
  Serial.println("---------------------------");
}

void setup() {
  Serial.begin(9600);
  BTSerial.begin(38400); // HC-05 AT mode baud rate
  delay(1000);

  Serial.println("===============================");
  Serial.println("  MASTER HC-05 CONFIGURATION");
  Serial.println("===============================");
  Serial.println();

  // 1. Basic test
  sendCommand("AT");

  // 2. Set role to master
  sendCommand("AT+ROLE=1");

  // 3. Set baud rate for normal communication
  sendCommand("AT+UART=9600,0,0");

  // 4. Set a recognizable name (optional but helpful)
  sendCommand("AT+NAME=HC05_MASTER");

  // 5. Set password (must match slave)
  sendCommand("AT+PSWD=1234");

  // 6. Set connection mode to fixed address only
  sendCommand("AT+CMODE=0");

  // 7. Bind to the slave's address
  char bindCmd[40];
  snprintf(bindCmd, sizeof(bindCmd), "AT+BIND=%s", SLAVE_ADDR);
  sendCommand(bindCmd);

  // 8. Get this module's own address for reference
  Serial.println("--- Master's own address ---");
  sendCommand("AT+ADDR?", 2000);

  // 9. Verify all settings
  Serial.println("--- Verifying settings ---");
  sendCommand("AT+ROLE?");
  sendCommand("AT+UART?");
  sendCommand("AT+NAME?");
  sendCommand("AT+PSWD?");
  sendCommand("AT+CMODE?");
  sendCommand("AT+BIND?");

  Serial.println("===============================");
  Serial.println("  MASTER CONFIGURATION COMPLETE");
  Serial.println("===============================");
  Serial.println();
  Serial.println("Next steps:");
  Serial.println("1. Verify BIND address above matches your slave");
  Serial.println("2. Power off this HC-05");
  Serial.println("3. Wire slave HC-05 to ATmega1284 (pins 14/15)");
  Serial.println("4. Wire master HC-05 to ATmega328 (pins 2/3)");
  Serial.println("5. Flash the real master/slave code");
  Serial.println("6. Power on slave first, then master");
  Serial.println("7. Wait for double-blink = connected!");
}

void loop() {
  // Nothing — config is done in setup
}