/*
  ===== MASTER / SENDER — ATmega328 =====
  Reads thermistor voltage on A5 (physical pin 28)
  and sends the voltage as a string to the ESP32 via HC-05.

  Edited for ECE 1885 by: Matthew Ketas
  Last Edited: 3/26/2026
  LLMs Used: Claude Sonnet 4.6 Extended

  ---- ATmega328 Pin Mapping ----
  Arduino Pin A5 (PC5, physical pin 28) -> Thermistor (ADC input)
  Arduino Pin 2  (PD2) -> HC-05 TX  (receive from BT)
  Arduino Pin 3  (PD3) -> HC-05 RX  (send to BT, use voltage divider!)
  Arduino Pin 8  (PB0) -> LED + resistor -> GND  (status indicator)
  Hardware Serial (PD0/PD1) -> USB/FTDI for debug monitor
  --------------------------------
*/

#include <SoftwareSerial.h>

SoftwareSerial BTSerial(2, 3); // RX, TX

int ledPin = 8;
int thermPin = A5; // Physical pin 28 = PC5 = A5

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  pinMode(ledPin, OUTPUT);

  // Code compliation test
  for(int i = 0; i < 3; i++){
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
    delay(1000);
  }
  
  Serial.println("=== MASTER (ATmega328) ===");
  Serial.println("Reading thermistor on A5, sending via BT");
  delay(1000);
}

void loop() {
  // Read ADC (0-1023) and convert to voltage (5V reference)
  int rawADC = analogRead(thermPin);
  float voltage = (rawADC / 1023.0) * 5.0;

  if(voltage < 0.5){
    digitalWrite(ledPin, HIGH);
  }

  // Send voltage as a string with newline delimiter
  BTSerial.println(voltage, 2); // 2 decimal places, e.g. "3.24\n"

  // Local debug
  Serial.print("ADC: ");
  Serial.print(rawADC);
  Serial.print("  Voltage: ");
  Serial.print(voltage, 2);
  Serial.println("V");

  // Local LED mirrors threshold for quick visual check
  if (voltage > 2.5) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(500); // Send twice per second
}