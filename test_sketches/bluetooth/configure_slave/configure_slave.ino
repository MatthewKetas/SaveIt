/*
  Configure SLAVE HC-05 via Arduino Nano
  
  ---- WIRING ----
  Nano Pin 2  -> HC-05 TX
  Nano Pin 3  -> HC-05 RX (use voltage divider: 1K + 2K)
  Nano 5V     -> HC-05 VCC
  Nano GND    -> HC-05 GND
  -----------------

  STEPS:
  1. Wire up the HC-05 but do NOT power it yet
  2. Flash this sketch to the Nano
  3. Open Serial Monitor at 9600 baud, line ending "Both NL & CR"
  4. Hold the HC-05 button, THEN plug in power (or reset Nano)
     — HC-05 LED should blink slowly (~2 sec) = AT mode
  5. Watch the Serial Monitor for results
  6. WRITE DOWN THE ADDRESS — you need it for the master
*/

#include <SoftwareSerial.h>

SoftwareSerial BTSerial(2, 3); // RX, TX

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
  Serial.println("  SLAVE HC-05 CONFIGURATION");
  Serial.println("===============================");
  Serial.println();

  // 1. Basic test
  sendCommand("AT");

  // 2. Set role to slave
  sendCommand("AT+ROLE=0");

  // 3. Set baud rate for normal communication
  sendCommand("AT+UART=9600,0,0");

  // 4. Set a recognizable name (optional but helpful)
  sendCommand("AT+NAME=HC05_SLAVE");

  // 5. Set password (both must match)
  sendCommand("AT+PSWD=1234");

  // 6. Get this module's address — WRITE THIS DOWN
  Serial.println("*** IMPORTANT: COPY THE ADDRESS BELOW ***");
  Serial.println("*** YOU WILL NEED IT FOR THE MASTER   ***");
  sendCommand("AT+ADDR?", 2000);

  // 7. Verify all settings
  Serial.println("--- Verifying settings ---");
  sendCommand("AT+ROLE?");
  sendCommand("AT+UART?");
  sendCommand("AT+NAME?");
  sendCommand("AT+PSWD?");

  Serial.println("===============================");
  Serial.println("  SLAVE CONFIGURATION COMPLETE");
  Serial.println("===============================");
  Serial.println();
  Serial.println("Next steps:");
  Serial.println("1. Copy the address printed above");
  Serial.println("2. Replace colons with commas for the master");
  Serial.println("   Example: 1234:56:789ABC -> 1234,56,789ABC");
  Serial.println("3. Power off this HC-05");
  Serial.println("4. Flash the master config sketch");
}

void loop() {
  // Nothing — config is done in setup
}