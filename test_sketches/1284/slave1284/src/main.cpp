#include <Arduino.h>

void setup() {
  pinMode(0, OUTPUT); // PB0, physical pin 1
}

void loop() {
  digitalWrite(0, HIGH);
  delay(500);
  digitalWrite(0, LOW);
  delay(500);
}
