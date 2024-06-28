#include "global.h"
#include "BLEService.h"

// Initialisierung der LED-Pins
const int redLedPin     = 27;
const int yellowLedPin  = 14;
const int greenLedPin   = 12;

// RX und TX Pins definieren
const int rxPin = 16;
const int txPin = 17;

void clearSerialBuffer() {
	while (Serial.available() > 0) {
		Serial.read();
	}
}

/**
 * @brief Leert den seriellen Puffer.
 */
void setup() {
	// Initialisierung der eigenen seriellen Schnittstellen
	Serial.begin(9600);
	if (!Serial) {
		handleError("Serial initialization failed");
		return;
	} else {
    	DEBUG_PRINT("Serial initialized");
	}

	mySerial.begin(1200, SERIAL_8N1, txPin, rxPin); // RX1, TX1
	// Setze die Pins auf INPUT_PULLUP und überprüfe den Zustand
	// pinMode(rxPin, INPUT);
	// pinMode(txPin, INPUT);

	if (!mySerial) {
		handleError("mySerial initialization failed");
		return;
	} else {
    	DEBUG_PRINT("mySerial initialized with 1200 baud");
	}

	BLEDevice::init("Schulz-BT-ESP32");

	// Konfiguration der Drehschalter-Pins als Eingänge
	for (int i = 0; i < 8; i++) {
		pinMode(switchPins[i], INPUT_PULLUP);
	}

	// Konfiguration der LED-Pins als Ausgänge
	for (int i = 0; i < 3; i++) {
		pinMode(ledPins[i], OUTPUT);
		digitalWrite(ledPins[i], LOW); // LEDs ausschalten
	}

	std::string deviceName = "Schulz BT - " + BLEDevice::getAddress().toString();
	setupBLE(deviceName);
	DEBUG_PRINT(String("Device Name: ") + deviceName.c_str());
}

void loop() {
	// Lesen des Drehschalterzustands
	int switchPosition = readSwitchPosition();

	if (switchPosition != currentSwitchPosition && switchPosition != -1) {
		currentSwitchPosition = switchPosition;
		mySerial.updateBaudRate(baudRates[switchPosition]);
		DEBUG_PRINT("Baudrate updated to: " + String(baudRates[switchPosition]));
	}

	if (mySerial.available() > 0) {
		// Nachricht lesen
		String message = mySerial.readStringUntil('\n');
		if (message.length() == 0) {
			handleError("Empty message received from mySerial");
			return;
		}
        message.replace("\n", "\r\n");
		Serial.println(message);
		Serial.flush(); // Stelle sicher, dass die Nachricht vollständig gesendet wurde

		// Nachricht über Bluetooth senden
		if (btDeviceConnected) {
			sendBLEMessage(message);
		}
	}

	if (Serial.available()) {
		String input = Serial.readStringUntil('\n');
		input.trim();
		if (input.length() > 0) {
            mySerial.print(input + "\r\n"); // Füge '\r\n' hinzu
			mySerial.println(input);
			mySerial.flush(); // Stelle sicher, dass die Nachricht vollständig gesendet wurde
		}
	}

	int rxState = digitalRead(rxPin);
	int txState = digitalRead(txPin);

	if (rxState == HIGH && txState == HIGH) {
		// Gelbe LED blinken lassen, wenn keine serielle Kommunikation stattfindet
        blinkLed(yellowLedPin, 500);
		setLedState(redLedPin, LOW);
	} else {
		setLedState(redLedPin, HIGH);
	}
	// Puffer leeren
	clearSerialBuffer();

	delay(500);
}
