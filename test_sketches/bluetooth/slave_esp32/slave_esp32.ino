/*
  ===== SLAVE / RECEIVER — ESP32-WROOM-32 =====
  Receives thermistor voltage from ATmega328 via HC-05.
  Blinks LED on GPIO 22 if voltage > 2.5V.

  Edited for ECE 1885 by: Matthew Ketas
  Last Edited: 3/26/2026
  LLMs Used: Claude Sonnet 4.6 Extended

  ---- ESP32 Pin Mapping ----
  HC-05 Bluetooth (Serial1):
    GPIO 26 (TX1) -> HC-05 RX (680+1K divider)
    GPIO 27 (RX1) <- HC-05 TX
    GPIO 35       <- HC-05 STATE (input only)
    GPIO 14       -> HC-05 EN/KEY

  Alert LED:
    GPIO 22 -> LED + resistor -> GND

  USB Serial (Serial):
    GPIO 1/3 -> USB debug
  ---------------------------
*/

#define BT_RX  27
#define BT_TX  26
#define BT_STATE 35
#define BT_EN  14
#define LED_PIN 22

#define VOLTAGE_THRESHOLD 2.5
#define BLINK_INTERVAL 200  // ms per blink toggle

String incomingData = "";
float lastVoltage = 0.0;
bool alertActive = false;
unsigned long lastBlink = 0;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, ESP_RX, ESP_TX);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BT_STATE, INPUT);
  pinMode(BT_EN, OUTPUT);
  digitalWrite(BT_EN, LOW);  // Normal mode (not AT command)
  digitalWrite(LED_PIN, LOW);

  Serial.println("=== SLAVE (ESP32) ===");
  Serial.println("Waiting for thermistor data via Bluetooth...");
}

void loop() {
  // Check Bluetooth connection status
  bool btConnected = digitalRead(BT_STATE) == HIGH;

  // Read incoming voltage strings from HC-05
  while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\n') {
      incomingData.trim();
      if (incomingData.length() > 0) {
        lastVoltage = incomingData.toFloat();

        Serial.print("Received: ");
        Serial.print(lastVoltage, 2);
        Serial.print("V");

        if (lastVoltage > VOLTAGE_THRESHOLD) {
          alertActive = true;
          Serial.println(" -> ALERT: above threshold!");
        } else {
          alertActive = false;
          digitalWrite(LED_PIN, LOW);
          Serial.println(" -> OK");
        }
      }
      incomingData = "";
    } else {
      incomingData += c;
    }
  }

  // Blink LED if voltage is above threshold
  if (alertActive) {
    if (millis() - lastBlink >= BLINK_INTERVAL) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      lastBlink = millis();
    }
  }

  // Optional: warn if BT disconnected
  if (!btConnected) {
    // Could add a different blink pattern here
  }
}