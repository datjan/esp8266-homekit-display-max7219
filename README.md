# esp8266-homekit-display-max7219
This is a native HomeKit display with D1 mini ESP8266 and max7219 pixel matrix.

## Hardware
The following hardware is required:
```
- D1 mini (ESP8266)
- Pixel matrix (max7219)
```

Connection:
D1 mini -> max7219
```
3V3 -> VCC
GND -> GND
D4 -> CS
D5 -> CLK
D7 - DIN
```
![alt text](https://github.com/datjan/esp8266-homekit-display-max7219/blob/main/connection-schema.png?raw=true)

## Development
This sketch is for following development environment
```
Arduino
```

Following libraries are required
```
https://github.com/datjan/Arduino-HomeKit-ESP8266 (fork from Mixiaoxiao/Arduino-HomeKit-ESP8266:master)
https://github.com/wayoda/LedControl
```

## Setup
Setup esp8266-homekit-display-max7219.ino:
```
int countModules = 1; // max7219 modules count
int intense = 3; // intense: 8 (default), Range 1 - 16
int orientation = 1; // rotation: 1 (default), 2 (90°), 3 (180°), 4 (270°)
int inverted = 0; // inverted: 0 (default), 1 (inverted)
```

Setup my_accessory.c:
```
.password = "555-11-123" // Homekit Code
```

Setup wifi_info.h
```
const char *ssid = "xxx"; // SETUP Wlan ssid
const char *password = "xxx"; // SETUP Wlan password
```

## Upload to device
Following files needs to be uploaded to the ESP8266 (D1 mini)
```
esp8266-homekit-display-max7219.ino
my_accessory.c
wifi_info.h
```

## Add device to Homekit
The device can be added to homekit like every other homekit device.
