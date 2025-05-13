// main.cpp
#include "Controller.h"
#include "Bluetooth.h"
#include "USBHost.h"
Controller controller;
unsigned long specialButtonPressTime = 0;

void setup() {
    Serial.begin(115200);

    #ifdef IS_ESP32S3
        Serial.println("Initializing ESP32-S3 for USB controllers");
    #else
        Serial.println("Initializing ESP32-H2 with USB Host Mini");
        pinMode(USB_HOST_MINI_RESET_PIN, OUTPUT);
        digitalWrite(USB_HOST_MINI_RESET_PIN, LOW);
        delay(100);
        digitalWrite(USB_HOST_MINI_RESET_PIN, HIGH);

        SPI.begin();
    #endif

    initBluetooth();
    initUSBHost();
    if (Usb.Init() == -1) {
        Serial.println("USB interface initialization failed");
        while(1);
    }

    Serial.println("USB interface successfully initialized");
    Serial.println("Compatible with Xbox 360, Xbox One, Xbox Series, PS3 and PS4 controllers");
    Serial.println("To initiate Bluetooth pairing, hold the Xbox/PS button for 3 seconds");

    startPairingMode();
    memset(&controller, 0, sizeof(controller));
}

void loop() {
    Usb.Task();
    checkPairingTimeout();
    readControllerInput();
    if (controller.type != CONTROLLER_NONE) {
        sendControllerData();
        delay(10);
    }
}