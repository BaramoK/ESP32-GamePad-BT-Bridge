// USBHost.h
#ifndef USBHOST_H
#define USBHOST_H

#include <SPI.h>
#include <Usb.h>
#include <USBHub.h>
#include <XBOXRECV.h>
#include <XBOXONE.h>
#include <PS3USB.h>
#include <PS4USB.h>

#ifdef IS_ESP32H2
    #define USB_HOST_MINI_INT_PIN 9
    #define USB_HOST_MINI_SS_PIN  10
    #define USB_HOST_MINI_RESET_PIN 11
#endif

extern USB Usb;
extern USBHub Hub;
extern XBOXRECV Xbox360;
extern XBOXONE XboxOne;
extern PS3USB PS3;
extern PS4USB PS4;

void initUSBHost();

#endif