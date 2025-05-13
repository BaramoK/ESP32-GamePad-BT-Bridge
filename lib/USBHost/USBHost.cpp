// USBHost.cpp
#include "USBHost.h"

USB Usb;
USBHub Hub(&Usb);
XBOXRECV Xbox360(&Usb);
XBOXONE XboxOne(&Usb);
PS3USB PS3(&Usb);
PS4USB PS4(&Usb);

void initUSBHost() {
    #ifdef IS_ESP32H2
        Usb.init();
        Usb.gpioInit(USB_HOST_MINI_INT_PIN, USB_HOST_MINI_SS_PIN);
    #endif
}