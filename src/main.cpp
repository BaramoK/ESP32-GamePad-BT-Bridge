// main.cpp
#include "Controller.h"
#include "Bluetooth.h"
#include "USBHost.h"

// Instantiate the controller object
Controller controller;

// Variable to track the time when a special button is pressed
unsigned long specialButtonPressTime = 0;

// Function to initialize the system
void setup() {
    // Start serial communication at 115200 baud rate
    Serial.begin(115200);

    // Check if the device is an ESP32-S3
    #ifdef IS_ESP32S3
        Serial.println("Initializing ESP32-S3 for USB controllers");
    #else
        // Initialize ESP32-H2 with USB Host Mini
        Serial.println("Initializing ESP32-H2 with USB Host Mini");
        pinMode(USB_HOST_MINI_RESET_PIN, OUTPUT);
        digitalWrite(USB_HOST_MINI_RESET_PIN, LOW);
        delay(100);
        digitalWrite(USB_HOST_MINI_RESET_PIN, HIGH);

        // Initialize SPI communication
        SPI.begin();
    #endif

    // Initialize Bluetooth
    initBluetooth();
    // Initialize USB Host
    initUSBHost();
    // Check if USB interface initialization failed
    if (Usb.Init() == -1) {
        Serial.println("USB interface initialization failed");
        while(1);
    }

    // Print initialization success message
    Serial.println("USB interface successfully initialized");
    Serial.println("Compatible with Xbox 360, Xbox One, Xbox Series, PS3 and PS4 controllers");
    Serial.println("To initiate Bluetooth pairing, hold the Xbox/PS button for 3 seconds");

    // Start pairing mode
    startPairingMode();
    // Clear the controller object
    memset(&controller, 0, sizeof(controller));
}

// Main loop function
void loop() {
    // Perform USB tasks
    Usb.Task();
    // Check for pairing timeout
    checkPairingTimeout();
    // Read input from the controller
    readControllerInput();

    // Check if a controller is connected
    if (controller.type != CONTROLLER_NONE) {
        // Send controller data
        sendControllerData();
        // Delay for 10 milliseconds
        delay(10);
    }
}