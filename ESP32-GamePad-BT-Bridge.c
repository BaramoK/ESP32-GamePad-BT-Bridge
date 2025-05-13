#include <SPI.h>
#include <Usb.h>
#include <USBHub.h>
#include <XBOXRECV.h>
#include <XBOXONE.h>
#include <PS3USB.h>
#include <PS4USB.h>
#include <BleGamepad.h>
#include <BLEDevice.h>

// Board type definition
#if defined(CONFIG_IDF_TARGET_ESP32S3)
    #define IS_ESP32S3
#elif defined(CONFIG_IDF_TARGET_ESP32H2)
    #define IS_ESP32H2
#else
    #error "This code requires either an ESP32-S3 or ESP32-H2"
#endif

// Controller button enumeration
enum {
    BUTTON_NONE = 0,
    BUTTON_A,          // Xbox A / PS Cross
    BUTTON_B,          // Xbox B / PS Circle
    BUTTON_X,          // Xbox X / PS Square
    BUTTON_Y,          // Xbox Y / PS Triangle
    BUTTON_LB,         // Xbox LB / PS L1
    BUTTON_RB,         // Xbox RB / PS R1
    BUTTON_LS,         // Xbox LS / PS L3
    BUTTON_RS,         // Xbox RS / PS R3
    BUTTON_START,      // Xbox Start / PS Options/Start
    BUTTON_SELECT,     // Xbox Select / PS Share/Select
    BUTTON_XBOX,       // Xbox Guide / PS Home/PS
    BUTTON_UP,         // D-pad up
    BUTTON_DOWN,       // D-pad down
    BUTTON_LEFT,       // D-pad left
    BUTTON_RIGHT,      // D-pad right
    BUTTON_TOUCHPAD,   // PS4 TouchPad
    BUTTON_COUNT       // Total count
};

// Controller type enumeration
enum ControllerType {
    CONTROLLER_NONE,
    CONTROLLER_XBOX360,
    CONTROLLER_XBOXONE,
    CONTROLLER_XBOXSERIES,
    CONTROLLER_PS3,
    CONTROLLER_PS4
};

// Objects for USB communication and controller management
USB Usb;
USBHub Hub(&Usb);
XBOXRECV Xbox360(&Usb);
XBOXONE XboxOne(&Usb);
PS3USB PS3(&Usb);
PS4USB PS4(&Usb);

// Configuration for the Bluetooth peripheral emulating a controller
BleGamepadConfiguration bleConfig;
BleGamepad bleGamepad("ESP32-BLE-Gamepad", "ESP32", 100);

// Variables for Bluetooth pairing management
bool isPairing = false;
bool isConnected = false;
unsigned long pairingStartTime = 0;
const unsigned long PAIRING_TIMEOUT = 60000; // 1 min
const unsigned long BUTTON_HOLD_TIME = 3000; // 3 seconds to activate pairing

// Pin definitions for USB Host Mini on ESP32-H2
#ifdef IS_ESP32H2
    #define USB_HOST_MINI_INT_PIN 9
    #define USB_HOST_MINI_SS_PIN  10
    #define USB_HOST_MINI_RESET_PIN 11
#endif

// Structure for controller state data
typedef struct {
    int16_t leftX;
    int16_t leftY;
    int16_t rightX;
    int16_t rightY;
    uint8_t leftTrigger;
    uint8_t rightTrigger;
    bool buttons[BUTTON_COUNT];
    ControllerType type;
} Controller;

Controller controller;
unsigned long specialButtonPressTime = 0;

// Class for managing Bluetooth connection/disconnection events
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

// Activates Bluetooth pairing mode
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

// Checks if pairing timeout has been reached
inline void checkPairingTimeout() {
    if (isPairing && (millis() - pairingStartTime > PAIRING_TIMEOUT)) {
        isPairing = false;
        if (!isConnected) {
            Serial.println("Pairing timeout reached. Pairing mode deactivated.");
            BLEDevice::deinit(true);
        }
    }
}

// Checks if the special button is held for pairing
inline void checkSpecialButtonHold() {
    if (controller.buttons[BUTTON_XBOX]) {
        if (specialButtonPressTime == 0) {
            specialButtonPressTime = millis();
        } else if (millis() - specialButtonPressTime > BUTTON_HOLD_TIME) {
    startPairingMode();
            specialButtonPressTime = 0;
}
    } else {
        specialButtonPressTime = 0;
    }
}

// Reads inputs from Xbox 360 controller
void readXbox360Input() {
    controller.type = CONTROLLER_XBOX360;
    controller.leftX = Xbox360.getAnalogHat(LeftHatX);
    controller.leftY = Xbox360.getAnalogHat(LeftHatY);
    controller.rightX = Xbox360.getAnalogHat(RightHatX);
    controller.rightY = Xbox360.getAnalogHat(RightHatY);
    controller.leftTrigger = Xbox360.getAnalogButton(LT);
    controller.rightTrigger = Xbox360.getAnalogButton(RT);

    controller.buttons[BUTTON_A] = Xbox360.getButtonPress(A);
    controller.buttons[BUTTON_B] = Xbox360.getButtonPress(B);
    controller.buttons[BUTTON_X] = Xbox360.getButtonPress(X);
    controller.buttons[BUTTON_Y] = Xbox360.getButtonPress(Y);
    controller.buttons[BUTTON_LB] = Xbox360.getButtonPress(LB);
    controller.buttons[BUTTON_RB] = Xbox360.getButtonPress(RB);
    controller.buttons[BUTTON_LS] = Xbox360.getButtonPress(LS);
    controller.buttons[BUTTON_RS] = Xbox360.getButtonPress(RS);
    controller.buttons[BUTTON_START] = Xbox360.getButtonPress(START);
    controller.buttons[BUTTON_SELECT] = Xbox360.getButtonPress(BACK);
    controller.buttons[BUTTON_XBOX] = Xbox360.getButtonPress(GUIDE);
    controller.buttons[BUTTON_UP] = Xbox360.getButtonPress(UP);
    controller.buttons[BUTTON_DOWN] = Xbox360.getButtonPress(DOWN);
    controller.buttons[BUTTON_LEFT] = Xbox360.getButtonPress(LEFT);
    controller.buttons[BUTTON_RIGHT] = Xbox360.getButtonPress(RIGHT);
    controller.buttons[BUTTON_TOUCHPAD] = false;

    checkSpecialButtonHold();
}

// Reads inputs from Xbox One and Series controllers
void readXboxOneInput() {
    controller.type = XboxOne.isXboxSeries() ? CONTROLLER_XBOXSERIES : CONTROLLER_XBOXONE;
    controller.leftX = XboxOne.getAnalogHat(LeftHatX);
    controller.leftY = XboxOne.getAnalogHat(LeftHatY);
    controller.rightX = XboxOne.getAnalogHat(RightHatX);
    controller.rightY = XboxOne.getAnalogHat(RightHatY);
    controller.leftTrigger = XboxOne.getAnalogButton(LT);
    controller.rightTrigger = XboxOne.getAnalogButton(RT);
    controller.buttons[BUTTON_A] = XboxOne.getButtonPress(A);
    controller.buttons[BUTTON_B] = XboxOne.getButtonPress(B);
    controller.buttons[BUTTON_X] = XboxOne.getButtonPress(X);
    controller.buttons[BUTTON_Y] = XboxOne.getButtonPress(Y);
    controller.buttons[BUTTON_LB] = XboxOne.getButtonPress(LB);
    controller.buttons[BUTTON_RB] = XboxOne.getButtonPress(RB);
    controller.buttons[BUTTON_LS] = XboxOne.getButtonPress(LS);
    controller.buttons[BUTTON_RS] = XboxOne.getButtonPress(RS);
    controller.buttons[BUTTON_START] = XboxOne.getButtonPress(START);
    controller.buttons[BUTTON_SELECT] = XboxOne.getButtonPress(BACK);
    controller.buttons[BUTTON_XBOX] = XboxOne.getButtonPress(XBOX);
    controller.buttons[BUTTON_UP] = XboxOne.getButtonPress(UP);
    controller.buttons[BUTTON_DOWN] = XboxOne.getButtonPress(DOWN);
    controller.buttons[BUTTON_LEFT] = XboxOne.getButtonPress(LEFT);
    controller.buttons[BUTTON_RIGHT] = XboxOne.getButtonPress(RIGHT);
    controller.buttons[BUTTON_TOUCHPAD] = XboxOne.getButtonPress(SHARE);

    checkSpecialButtonHold();
}

// Reads inputs from PS3 controller
void readPS3Input() {
    controller.type = CONTROLLER_PS3;
    controller.leftX = PS3.getAnalogHat(LeftHatX) * 256 - 32768;
    controller.leftY = PS3.getAnalogHat(LeftHatY) * 256 - 32768;
    controller.rightX = PS3.getAnalogHat(RightHatX) * 256 - 32768;
    controller.rightY = PS3.getAnalogHat(RightHatY) * 256 - 32768;
    controller.leftTrigger = PS3.getAnalogButton(L2);
    controller.rightTrigger = PS3.getAnalogButton(R2);

    controller.buttons[BUTTON_A] = PS3.getButtonPress(CROSS);
    controller.buttons[BUTTON_B] = PS3.getButtonPress(CIRCLE);
    controller.buttons[BUTTON_X] = PS3.getButtonPress(SQUARE);
    controller.buttons[BUTTON_Y] = PS3.getButtonPress(TRIANGLE);
    controller.buttons[BUTTON_LB] = PS3.getButtonPress(L1);
    controller.buttons[BUTTON_RB] = PS3.getButtonPress(R1);
    controller.buttons[BUTTON_LS] = PS3.getButtonPress(L3);
    controller.buttons[BUTTON_RS] = PS3.getButtonPress(R3);
    controller.buttons[BUTTON_START] = PS3.getButtonPress(START);
    controller.buttons[BUTTON_SELECT] = PS3.getButtonPress(SELECT);
    controller.buttons[BUTTON_XBOX] = PS3.getButtonPress(PS);
    controller.buttons[BUTTON_UP] = PS3.getButtonPress(UP);
    controller.buttons[BUTTON_DOWN] = PS3.getButtonPress(DOWN);
    controller.buttons[BUTTON_LEFT] = PS3.getButtonPress(LEFT);
    controller.buttons[BUTTON_RIGHT] = PS3.getButtonPress(RIGHT);
    controller.buttons[BUTTON_TOUCHPAD] = false;

    checkSpecialButtonHold();
}

// Reads inputs from PS4 controller
void readPS4Input() {
    controller.type = CONTROLLER_PS4;
    controller.leftX = PS4.getAnalogHat(LeftHatX) * 256 - 32768;
    controller.leftY = PS4.getAnalogHat(LeftHatY) * 256 - 32768;
    controller.rightX = PS4.getAnalogHat(RightHatX) * 256 - 32768;
    controller.rightY = PS4.getAnalogHat(RightHatY) * 256 - 32768;
    controller.leftTrigger = PS4.getAnalogButton(L2);
    controller.rightTrigger = PS4.getAnalogButton(R2);

    controller.buttons[BUTTON_A] = PS4.getButtonPress(CROSS);
    controller.buttons[BUTTON_B] = PS4.getButtonPress(CIRCLE);
    controller.buttons[BUTTON_X] = PS4.getButtonPress(SQUARE);
    controller.buttons[BUTTON_Y] = PS4.getButtonPress(TRIANGLE);
    controller.buttons[BUTTON_LB] = PS4.getButtonPress(L1);
    controller.buttons[BUTTON_RB] = PS4.getButtonPress(R1);
    controller.buttons[BUTTON_LS] = PS4.getButtonPress(L3);
    controller.buttons[BUTTON_RS] = PS4.getButtonPress(R3);
    controller.buttons[BUTTON_START] = PS4.getButtonPress(OPTIONS);
    controller.buttons[BUTTON_SELECT] = PS4.getButtonPress(SHARE);
    controller.buttons[BUTTON_XBOX] = PS4.getButtonPress(PS);
    controller.buttons[BUTTON_UP] = PS4.getButtonPress(UP);
    controller.buttons[BUTTON_DOWN] = PS4.getButtonPress(DOWN);
    controller.buttons[BUTTON_LEFT] = PS4.getButtonPress(LEFT);
    controller.buttons[BUTTON_RIGHT] = PS4.getButtonPress(RIGHT);
    controller.buttons[BUTTON_TOUCHPAD] = PS4.getButtonPress(TOUCHPAD);

    checkSpecialButtonHold();
}
// Transmits controller data via Bluetooth
inline void sendControllerData() {
    if (isConnected && bleGamepad.isConnected()) {
        // Normalization and sending of axes
        bleGamepad.setAxes(
            controller.leftX,
            controller.leftY,
            controller.rightX,
            controller.rightY,
            controller.leftTrigger,
            controller.rightTrigger,
            0, 0
        );

        // Update buttons
        for (int i = BUTTON_A; i < BUTTON_COUNT; i++) {
            if (controller.buttons[i]) {
                bleGamepad.press(i);
            } else {
                bleGamepad.release(i);
            }
        }

        // Send complete report
        bleGamepad.sendReport();
    }
}

// Reads inputs from the active controller
void readControllerInput() {
    // Reset button states
    memset(controller.buttons, 0, sizeof(controller.buttons));

    if (Xbox360.Xbox360Connected[0]) {
        readXbox360Input();
    } else if (XboxOne.connected()) {
        readXboxOneInput();
    } else if (PS3.connected()) {
        readPS3Input();
    } else if (PS4.connected()) {
        readPS4Input();
    } else {
        controller.type = CONTROLLER_NONE;
        return;
    }
}

// System initialization
void setup() {
    Serial.begin(115200);

    #ifdef IS_ESP32S3
        Serial.println("Initializing ESP32-S3 for USB controllers");
    #else
        Serial.println("Initializing ESP32-H2 with USB Host Mini");
        // Pin configuration for USB Host Mini on ESP32-H2
        pinMode(USB_HOST_MINI_RESET_PIN, OUTPUT);
        digitalWrite(USB_HOST_MINI_RESET_PIN, LOW); // Reset USB Host shield
        delay(100);
        digitalWrite(USB_HOST_MINI_RESET_PIN, HIGH);

        SPI.begin();
    #endif
    // Bluetooth peripheral configuration
    bleConfig.setAutoReport(false);
    bleConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD);
    bleConfig.setButtonCount(BUTTON_COUNT);
    bleConfig.setWhichAxes(true, true, true, true, true, true, false, false);
    bleConfig.setAxesMin(0x0000);
    bleConfig.setAxesMax(0x7FFF);
    bleGamepad.begin(&bleConfig);

    #ifdef IS_ESP32H2
        Usb.init();
        Usb.gpioInit(USB_HOST_MINI_INT_PIN, USB_HOST_MINI_SS_PIN);
    #endif

    if (Usb.Init() == -1) {
        Serial.println("USB interface initialization failed");
        while(1);
    }

    Serial.println("USB interface successfully initialized");
    Serial.println("Compatible with Xbox 360, Xbox One, Xbox Series, PS3 and PS4 controllers");
    Serial.println("To initiate Bluetooth pairing, hold the Xbox/PS button for 3 seconds");

    // Initial activation of pairing mode
    startPairingMode();

    // Controller structure initialization
    memset(&controller, 0, sizeof(controller));
}

// Main program loop
void loop() {
    Usb.Task();  // Processing USB events

    // Check pairing timeout
    checkPairingTimeout();

    // Process controller data if connected
    readControllerInput();

    if (controller.type != CONTROLLER_NONE) {
        sendControllerData();
        delay(10);  // Delay to stabilize BLE communication
    }
}
