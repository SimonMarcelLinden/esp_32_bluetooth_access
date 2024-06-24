#include "global.h"

const int redLedPin     = 27;
const int yellowLedPin  = 14;
const int greenLedPin   = 12;

void setup() {
	// Initialisierung der eigenen seriellen Schnittstellen
	Serial.begin(9600);
    mySerial.begin(1200, SERIAL_8N1, 17, 16); // RX1, TX1

	// Konfiguration der Drehschalter-Pins als Eingänge
	for (int i = 0; i < 8; i++) {
		pinMode(switchPins[i], INPUT_PULLUP);
	}

	// Konfiguration der LED-Pins als Ausgänge
	for (int i = 0; i < 3; i++) {
		pinMode(ledPins[i], OUTPUT);
		digitalWrite(ledPins[i], LOW); // LEDs ausschalten
	}
}

void loop() {
    // Lesen des Drehschalterzustands
    int switchPosition = readSwitchPosition();

	if (switchPosition != currentSwitchPosition && switchPosition != -1) {
        currentSwitchPosition = switchPosition;

        mySerial.updateBaudRate(baudRates[switchPosition]);
    }

	if (mySerial.available() > 0) {
		// Nachricht lesen
		String message = mySerial.readStringUntil('\n');
		Serial.println(message);
	}

	if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();

        if (input.startsWith("1") || input.startsWith("2") || input.startsWith("3")) {
            mySerial.println(input);
		}
	}

    delay(500);
}
