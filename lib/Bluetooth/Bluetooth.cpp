// Bluetooth.cpp
#include "Bluetooth.h"

bool isPairing = false;
bool isConnected = false;
unsigned long pairingStartTime = 0;

BleGamepadConfiguration bleConfig;
BleGamepad bleGamepad("ESP32-BLE-Gamepad", "ESP32", 100);

class BleGamepadCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* server) {
        isConnected = true;
        isPairing = false;
        Serial.println("Bluetooth device connected!");
    }

    void onDisconnect(BLEServer* server) {
        isConnected = false;
        Serial.println("Bluetooth device disconnected!");
        server->startAdvertising();
    }
};

void initBluetooth() {
    bleConfig.setAutoReport(false);
    bleConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD);
    bleConfig.setButtonCount(BUTTON_COUNT);
    bleConfig.setWhichAxes(true, true, true, true, true, true, false, false);
    bleConfig.setAxesMin(0x0000);
    bleConfig.setAxesMax(0x7FFF);
    bleGamepad.begin(&bleConfig);
}

void startPairingMode() {
    if (!isPairing) {
        isPairing = true;
        pairingStartTime = millis();

        BLEDevice::init("ESP32-BLE-Gamepad");
        BLEServer *pServer = BLEDevice::createServer();
        pServer->setCallbacks(new BleGamepadCallbacks());

        Serial.println("Bluetooth pairing mode activated for 1 minute");
        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->start();
    }
}

void checkPairingTimeout() {
    if (isPairing && (millis() - pairingStartTime > PAIRING_TIMEOUT)) {
        isPairing = false;
        if (!isConnected) {
            Serial.println("Pairing timeout reached. Pairing mode deactivated.");
            BLEDevice::deinit(true);
        }
    }
}