#include "BLEService.h"

BLECharacteristic *pCharacteristic 	= nullptr;
const char* SERVICE_UUID        	= "0000180d-0000-1000-8000-00805f9b34fb";
const char* CHARACTERISTIC_UUID 	= "00002a37-0000-1000-8000-00805f9b34fb";

bool btDeviceConnected = false;

void BTServerCallbacks::onConnect(BLEServer* pServer) {
    btDeviceConnected = true;
   	digitalWrite(greenLedPin, HIGH);
	DEBUG_PRINT("Ein Gerät hat sich verbunden");
	pServer->getAdvertising()->stop(); // Stop advertising
}

void BTServerCallbacks::onDisconnect(BLEServer* pServer) {
    btDeviceConnected = false;
    digitalWrite(greenLedPin, LOW); // Grüne LED aus, wenn getrennt
    DEBUG_PRINT("Gerät wurde getrennt.");
    pServer->getAdvertising()->start();    // Restart advertising
}

void BTCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
	std::string value = pCharacteristic->getValue();
    if (value.empty()) {
        handleError("Received empty BLE characteristic value");
        return;
    }
    mySerial.println(value.c_str());
}

uint32_t BTSecurity::onPassKeyRequest() {
	DEBUG_PRINT("PassKeyRequest");
    return 134210; // 6 Stelliger Statischer PIN-Code
}

void BTSecurity::onPassKeyNotify(uint32_t pass_key) {
    DEBUG_PRINT("PassKeyNotify: " + String(pass_key));
}

bool BTSecurity::onSecurityRequest() {
    DEBUG_PRINT("SecurityRequest");
    return true;
}

void BTSecurity::onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) {
	DEBUG_PRINT("Authentifizierungsstatus: " + String(cmpl.success ? "Pairing erfolgreich!" : "Pairing fehlgeschlagen! Fehlercode: " + String(cmpl.fail_reason)));
}

bool BTSecurity::onConfirmPIN(uint32_t pin) {
    DEBUG_PRINT("The passkey YES/NO number: " + String(pin));
    return true;
}

void setupBLE(const std::string &deviceName) {
    DEBUG_PRINT("Initializing BLE Device...");
    BLEDevice::init(deviceName);

    DEBUG_PRINT("Creating BLE Server...");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new BTServerCallbacks());

    DEBUG_PRINT("Configuring Security...");
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND ); // Bonding erforderlich
    // pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_MITM); // Kein Bonding erforderlich
    pSecurity->setCapability(ESP_IO_CAP_IO); // Keine Ein-/Ausgabe-Kapazität
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    pSecurity->setStaticPIN(134210); // Weiterhin statischer PIN

    BLEDevice::setSecurityCallbacks(new BTSecurity());

    DEBUG_PRINT("Creating BLE Service...");
    BLEService *pService = pServer->createService(SERVICE_UUID);

    DEBUG_PRINT("Creating BLE Characteristic...");
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

    DEBUG_PRINT("Adding Descriptor to Characteristic...");
    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setCallbacks(new BTCallbacks());

    DEBUG_PRINT("Starting Service...");
    pService->start();

    DEBUG_PRINT("Starting Advertising...");
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    // pAdvertising->setMinPreferred(0x06);  // Funktionen für iOS spezifisch
    // pAdvertising->setMinPreferred(0x12);  // Funktionen für iOS spezifisch
    pAdvertising->setMinPreferred(0x50);  // 60 ms

    BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
    // oAdvertisementData.setFlags(0x08); // BR_EDR_NOT_SUPPORTED 0x04 | LE General Discoverable Mode 0x02
    oAdvertisementData.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT); // Flags für allgemeine Entdeckbarkeit und BR/EDR nicht unterstützt
    oAdvertisementData.setCompleteServices(BLEUUID(SERVICE_UUID));

    // Appearance-Wert für ein generisches Gerät setzen
    oAdvertisementData.setAppearance(128);

    pAdvertising->setAdvertisementData(oAdvertisementData);

    BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
    oScanResponseData.setName(deviceName);

    pAdvertising->setScanResponseData(oScanResponseData);

    BLEDevice::startAdvertising();
    DEBUG_PRINT("Waiting for a client connection...");
}

void sendBLEMessage(const String& message) {
	const size_t chunkSize = 600;
	size_t messageLength = message.length();
	size_t offset = 0;

	while (offset < messageLength) {
		size_t len = (messageLength - offset) > chunkSize ? chunkSize : (messageLength - offset);
		String chunk = message.substring(offset, offset + len);
		pCharacteristic->setValue(chunk.c_str());
		pCharacteristic->notify();
		offset += len;
		delay(50); // Kleine Verzögerung, um Puffer zu leeren
	}
}
