// Controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include <SPI.h>
#include <Usb.h>
#include <USBHub.h>
#include <XBOXRECV.h>
#include <XBOXONE.h>
#include <PS3USB.h>
#include <PS4USB.h>

#if defined(CONFIG_IDF_TARGET_ESP32S3)
    #define IS_ESP32S3
#elif defined(CONFIG_IDF_TARGET_ESP32H2)
    #define IS_ESP32H2
#else
    #error "This code requires either an ESP32-S3 or ESP32-H2"
#endif

enum {
    BUTTON_NONE = 0,
    BUTTON_A,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_LB,
    BUTTON_RB,
    BUTTON_LS,
    BUTTON_RS,
    BUTTON_START,
    BUTTON_SELECT,
    BUTTON_XBOX,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_TOUCHPAD,
    BUTTON_COUNT
};

enum ControllerType {
    CONTROLLER_NONE,
    CONTROLLER_XBOX360,
    CONTROLLER_XBOXONE,
    CONTROLLER_XBOXSERIES,
    CONTROLLER_PS3,
    CONTROLLER_PS4
};

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

extern Controller controller;
extern unsigned long specialButtonPressTime;

void readControllerInput();
void sendControllerData();

#endif