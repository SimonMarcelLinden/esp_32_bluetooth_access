#include "global.h"

// Definition der globalen Variablen
HardwareSerial mySerial(1);

// Pinbelegung für den Drehschalter (8 Positionen)
const int switchPins[NUM_SWITCH_PINS] = {2, 4, 5, 18, 26, 25, 33, 32};

// Mögliche Baudraten, die eingestellt werden können
const int baudRates[NUM_BAUD_RATES] = {115200, 57600, 38400, 19200, 1200, 2400, 4800, 9600};

// Pinbelegung für die LEDs
const int ledPins[NUM_LED_PINS] = {redLedPin, yellowLedPin, greenLedPin};

// Initialisierung der globalen Variablen
int currentSwitchPosition = -1;
bool ledState = false;

void handleError(const String &errorMessage) {
    DEBUG_PRINT("Error: " + errorMessage);
    blinkLed(redLedPin, 1000);
}

bool isValidPin(int pin) {
    for (int i = 0; i < NUM_LED_PINS; i++) {
        if (ledPins[i] == pin) {
            return true;
        }
    }
    return false;
}

void setLedState(int pin, bool state) {
	if (!isValidPin(pin)) {
        handleError("Invalid LED pin: " + String(pin));
        return;
    }
	digitalWrite(pin, state ? HIGH : LOW);
}

void blinkLed(int pin, int duration) {
    if (!isValidPin(pin)) {
        handleError("Invalid LED pin: " + String(pin));
        return;
    }
	setLedState(pin, HIGH);
    delay(duration / 2);
    setLedState(pin, LOW);
    delay(duration / 2);
}

int readSwitchPosition() {
    for (int i = 0; i < 8; i++) {
		if (digitalRead(switchPins[i]) == LOW) {
			return i;
		}
	}
	return -1; // Keine gültige Position gefunden
}
