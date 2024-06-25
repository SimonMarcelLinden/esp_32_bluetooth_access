#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>

#define DEBUG 0

#if DEBUG
	#define DEBUG_PRINT(x) Serial.println(x)
#else
	#define DEBUG_PRINT(x)
#endif

// Deklaration der seriellen Schnittstelle
extern HardwareSerial mySerial;

// LED-Pins
extern const int greenLedPin;
extern const int yellowLedPin;
extern const int redLedPin;

// Pinbelegung für den Drehschalter (8 Positionen)
const int NUM_SWITCH_PINS = 8;
extern const int switchPins[NUM_SWITCH_PINS];

// Mögliche Baudraten, die eingestellt werden können
const int NUM_BAUD_RATES = 8;
extern const int baudRates[NUM_BAUD_RATES];

// Pinbelegung für die LEDs
const int NUM_LED_PINS = 3;
extern const int ledPins[NUM_LED_PINS];

// Aktuelle Position des Drehschalters
extern int currentSwitchPosition;

// Zustand der LED
extern bool ledState;

/**
 * @brief Blinkt die LED am gegebenen Pin für die angegebene Dauer.
 *
 * @param pin Der Pin der LED.
 * @param duration Die Dauer des Blinkens in Millisekunden.
 */
extern void blinkLed(int pin, int duration);

/**
 * @brief Setzt den Zustand der LED am gegebenen Pin.
 *
 * @param pin Der Pin der LED.
 * @param state Der Zustand der LED (true für an, false für aus).
 */
extern void setLedState(int pin, bool state);

/**
 * @brief Liest die aktuelle Position des Drehschalters.
 *
 * @return Die Position des Drehschalters (0-7) oder -1, wenn keine gültige Position gefunden wurde.
 */
extern int readSwitchPosition();
