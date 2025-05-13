// Controller.cpp
#include "Controller.h"
#include "Bluetooth.h"

USB Usb;
USBHub Hub(&Usb);
XBOXRECV Xbox360(&Usb);
XBOXONE XboxOne(&Usb);
PS3USB PS3(&Usb);
PS4USB PS4(&Usb);

void checkSpecialButtonHold() {
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

void readControllerInput() {
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

void sendControllerData() {
    if (isConnected && bleGamepad.isConnected()) {
        bleGamepad.setAxes(
            controller.leftX,
            controller.leftY,
            controller.rightX,
            controller.rightY,
            controller.leftTrigger,
            controller.rightTrigger,
            0, 0
        );

        for (int i = BUTTON_A; i < BUTTON_COUNT; i++) {
            if (controller.buttons[i]) {
                bleGamepad.press(i);
            } else {
                bleGamepad.release(i);
            }
        }

        bleGamepad.sendReport();
    }
}