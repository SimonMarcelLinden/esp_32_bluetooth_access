#pragma once

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "global.h"

extern BLECharacteristic *pCharacteristic;
extern bool btDeviceConnected;

/**
 * @brief Initialisiert den BLE-Dienst mit dem angegebenen Gerätenamen.
 *
 * @param deviceName Der Name des BLE-Geräts.
 */
void setupBLE(const std::string &deviceName);

/**
 * @brief Klasse für BLE-Server-Callbacks.
 */
class BTServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer);
    void onDisconnect(BLEServer* pServer);
};

/**
 * @brief Klasse für BLE-Charakteristik-Callbacks.
 */
class BTCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic);
};

/**
 * @brief Klasse für BLE-Sicherheits-Callbacks.
 */
class BTSecurity: public BLESecurityCallbacks {
    uint32_t onPassKeyRequest();
    void onPassKeyNotify(uint32_t pass_key);
    bool onSecurityRequest();
    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl);
    bool onConfirmPIN(uint32_t pin);
};

