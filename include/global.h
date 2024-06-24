#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <HardwareSerial.h>

#define DEBUG true

HardwareSerial mySerial(1);

extern const int greenLedPin, yellowLedPin, redLedPin;

// Pinbelegung für den Drehschalter (8 Positionen)
const int switchPins[] = {2, 4, 5, 18, 26, 25, 33, 32};

// Mögliche Baudraten, die eingestellt werden können
const int baudRates[] = {115200, 57600, 38400, 19200, 1200, 2400, 4800, 9600};

// Pinbelegung für die LEDs
const int ledPins[] = {redLedPin, yellowLedPin, greenLedPin};;

int currentSwitchPosition = -1;

void blinkLed(int pin, int duration);


int readSwitchPosition() {
    for (int i = 0; i < 8; i++) {
        if (digitalRead(switchPins[i]) == LOW) {
            return i;
        }
    }
    return -1; // Keine gültige Position gefunden
}

#endif // GLOBAL_H
