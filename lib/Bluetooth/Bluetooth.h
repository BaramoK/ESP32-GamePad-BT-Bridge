// Bluetooth.h
#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <BleGamepad.h>
#include <BLEDevice.h>

extern bool isPairing;
extern bool isConnected;
extern unsigned long pairingStartTime;
const unsigned long PAIRING_TIMEOUT = 60000;
const unsigned long BUTTON_HOLD_TIME = 3000;

extern BleGamepadConfiguration bleConfig;
extern BleGamepad bleGamepad;

void initBluetooth();
void startPairingMode();
void checkPairingTimeout();

#endif