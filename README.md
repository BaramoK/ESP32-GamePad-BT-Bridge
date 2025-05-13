# ESP32-GamePad-BT-Bridge 
## Description
ESP32-GamePad-BT-Bridge is a project that transforms an ESP32 into a gateway to connect USB game controllers to Bluetooth-compatible devices. This solution allows you to use your favorite wired controllers with wireless devices such as smartphones, tablets, Smart TVs, or game consoles.

## Features
- Connection of standard USB game controllers to the ESP32
- Emulation of a Bluetooth HID controller compatible with many devices
- Low latency for an optimal gaming experience
- Support for multiple controller protocols (to be specified according to your implementations)
- Customizable configuration via web or serial interface

## Required Hardware
- An ESP32-S3 (DevKit or other ESP32-S3 based board)
- USB OTG cable to connect the controller to the ESP32
- Compatible USB game controller
- 5V power supply for the ESP32-S3
- (Optional) 3D printed case
## Installation

### Prerequisites
- [PlatformIO](https://platformio.org/) or [Arduino IDE](https://www.arduino.cc/en/software) with ESP32 support
- Required libraries (to be specified according to your project)
```bash
# Clone the repository
git clone https://github.com/your-name/ESP32-GamePad-BT-Bridge.git

# Navigate to the project directory
cd ESP32-GamePad-BT-Bridge

# If you use PlatformIO
platformio run
