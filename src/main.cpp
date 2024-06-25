#include "global.h"
#include "BLEService.h"

// Initialisierung der LED-Pins
const int redLedPin     = 27;
const int yellowLedPin  = 14;
const int greenLedPin   = 12;

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
    }

	mySerial.begin(1200, SERIAL_8N1, 17, 16); // RX1, TX1
	if (!mySerial) {
        handleError("mySerial initialization failed");
        return;
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
	DEBUG_PRINT("Device Name: " + deviceName);

	setLedState(redLedPin, LOW);
	setLedState(yellowLedPin, LOW);
	setLedState(greenLedPin, LOW);
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
		Serial.println(message);
   		digitalWrite(redLedPin, LOW);
		blinkLed(yellowLedPin, 1000);

		// Nachricht über Bluetooth senden
		if (btDeviceConnected) {
			pCharacteristic->setValue(message.c_str());
			pCharacteristic->notify();
			// blinkLed(greenLedPin, 500);
		}
   		digitalWrite(greenLedPin, HIGH);
	} else if (mySerial.available() == 0)  {
		// Blinken der gelben LED, wenn keine serielle Kommunikation stattfindet
   		digitalWrite(redLedPin, HIGH);
	}

	if (Serial.available()) {
		String input = Serial.readStringUntil('\n');
		input.trim();
		if (input.length() > 0) {
            mySerial.println(input);
        }
	}

	// Puffer leeren
	clearSerialBuffer();

	delay(500);
}
