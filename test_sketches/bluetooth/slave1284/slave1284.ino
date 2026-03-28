/*   
  ===== SLAVE / RECEIVER — ATmega1284 =====
  HC-05 Bluetooth module configured as SLAVE.
  Receives bytes from the master and toggles an LED accordingly.

  modified on 10 Feb 2019 
  by Saeed Hosseini 
  https://electropeak.com/learn/ 

  Edited for ECE 1885 by: Matthew Ketas
  Last Edited: 3/26/2026
  LLMs Used: Claude Sonnet 4.6 Extended

  ---- ATmega1284 Pin Mapping ----
  Physical Pin 14 (PD1) = TX0 -> HC-05 RX  (send to BT, use voltage divider!)
  Physical Pin 15 (PD0) = RX0 -> HC-05 TX  (receive from BT)
  These are hardware UART0, accessed via "Serial" in code.

  Physical Pin 16 (PD2) = RX1 \
  Physical Pin 17 (PD3) = TX1 /  UART1 -> USB/FTDI for debug monitor
  These are hardware UART1, accessed via "Serial1" in code.

  Arduino Pin 8  (PB0) -> LED + resistor -> GND  (status indicator)
  ----------------------------------

  NOTE: No SoftwareSerial needed! The ATmega1284 has two
  hardware UARTs, so we use:
    Serial  (UART0, pins 14/15) -> HC-05 Bluetooth
    Serial1 (UART1, pins 16/17) -> Debug output via FTDI/USB
*/

int flag = 0;
int ledPin = 8;

void setup() {
  Serial.begin(9600);       // UART0 (phys pins 14/15) -> HC-05

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial1.println("=== SLAVE (ATmega1284) ===");
  Serial1.println("Waiting for Bluetooth data on UART0 (pins 14/15)...");
}

void loop() {
  // Check if data arrived from the master via HC-05
  if (Serial.available()) {
    flag = Serial.read();

    if (flag == 1) {
      digitalWrite(LED, HIGH);
    }
    else if (flag == 0) {
      digitalWrite(LED, LOW);
    }
  }
}