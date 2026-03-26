/*   
  ===== MASTER / SENDER — ATmega328 =====
  HC-05 Bluetooth module configured as MASTER.
  Sends a test byte (toggles 1 and 0) to the slave every 2 seconds.

  modified on 10 Feb 2019 
  by Saeed Hosseini 
  https://electropeak.com/learn/ 

  Edited for ECE 1885 by: Matthew Ketas
  Last Edited: 3/26/2026
  LLMs Used: Claude Sonnet 4.6 Extended

  ---- ATmega328 Pin Mapping ----
  Arduino Pin 2  (PD2) -> HC-05 TX  (receive from BT)
  Arduino Pin 3  (PD3) -> HC-05 RX  (send to BT, use voltage divider!)
  Arduino Pin 8  (PB0) -> LED + resistor -> GND  (status indicator)
  Hardware Serial (PD0/PD1) -> USB/FTDI for debug monitor
  --------------------------------
*/

#include <SoftwareSerial.h>

// SoftwareSerial: RX on pin 2, TX on pin 3
SoftwareSerial BTSerial(2, 3);

int LED = 8;
int sendValue = 1; // Toggles between 1 and 0

void setup() {
  Serial.begin(9600);       // Debug over USB/FTDI
  BTSerial.begin(9600);     // HC-05 default baud
  pinMode(LED, OUTPUT);

  Serial.println("=== MASTER (ATmega328) ===");
  Serial.println("Ready to connect");
  Serial.println("Default password is 1234 or 0000");
  delay(1000);
}

void loop() {
  // Send the current value to the slave
  BTSerial.write(sendValue);

  // Mirror on the local LED so you can see what's being sent
  if (sendValue == 1) {
    digitalWrite(LED, HIGH);
    Serial.println("Sent: 1  (LED ON command)");
  } else {
    digitalWrite(LED, LOW);
    Serial.println("Sent: 0  (LED OFF command)");
  }

  // Toggle for next cycle
  sendValue = (sendValue == 1) ? 0 : 1;

  delay(2000); // Send every 2 seconds
}