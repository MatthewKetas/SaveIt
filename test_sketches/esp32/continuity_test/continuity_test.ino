void setup() {
    // Cable 1
    pinMode(19, OUTPUT);
    pinMode(21, INPUT);

    // Cable 2
    pinMode(22, OUTPUT);
    pinMode(13, INPUT);

    // Set the output wires to high
    digitalWrite(22, HIGH);
    digitalWrite(19, HIGH);

    // Initialize serial monitor
    Serial.begin(115200);
    Serial.print("The serial monitor is initialized.");
}

void loop() {
    // Check for cable 1
    if(digitalRead(21) == HIGH) {
        Serial.println("Cable 1 connection");
    }

    // Check for cable 2
    if(digitalRead(13) == HIGH) {
        Serial.println("Cable 2 connection");
    }

    delay(1000);
}
