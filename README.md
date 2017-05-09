![Meeo for Arduino](https://res.cloudinary.com/circuitrocks/image/upload/v1494252922/meeo-arduino-banner_rll8rv.jpg)
# Meeo for Arduino
This library lets you connect your Arduino to your Meeo dashboard, and to your other devices through Meeo.

## Compatible Hardware
This library is tested to be used on the following board:
* [Adafruit HUZZAH Breakout](https://circuit.rocks/wifi-adafruit-huzzah-esp8266-breakout)
* [Adafruit Feather HUZZAH](https://circuit.rocks/wifi-feather-huzzah-esp8266-adafruit.html)
* [NodeMCU V2](https://circuit.rocks/nodemcu-v2-esp8266-development-board.html)
* [Crowtail NodeMCU](https://www.elecrow.com/crowtail-esp8266-node-mcu-p-1634.html)

_If you want your board to be supported, open an issue to let us know._

## Prerequisites

### Setup your Arduino IDE
Install the latest version of the [Arduino IDE](https://www.arduino.cc/en/Main/Software). After the installation, if an ESP8266 board is used, install the ESP8266 Board Package in order to program the ESP8266 modules through Arduino IDE. To do so, Adafruit provided a great guide on how to setup the ESP8266 modules on the Arduino IDE [here](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide)

### PubSubClient Library
Meeo makes use of MQTT at its core, therefore we need an MQTT Client for Arduino. 
Search and install knolleary's [PubSubClient](https://github.com/knolleary/pubsubclient) via the Library Manager inside the Arduino IDE. Go to `Sketch`->`Include Library`->`Manage Libraries...`

### Install your board's drivers
If you are using an ESP8266-based board, in most cases you will need to install their corresponding drivers. We've collated a few:
* [Adafruit HUZZAH Breakout Driver](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
* [Adafruit Feather HUZZAH Driver](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
* NodeMCU V2 Driver - for [Mac](https://github.com/jimaobian/CH34x_Install_V1.3) and for [Windows](https://github.com/nodemcu/nodemcu-devkit/tree/master/Drivers)
* [Crowtail NodeMCU Driver](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)

## Functions (API)
* [`Meeo.begin(namespace, access_key,[wifi_ssid],[wifi_password])`](#function-begin)
* [`Meeo.run()`](#function-run)

-------------------------------------------------------
<a name="function-begin"></a>
### `Meeo.begin(namespace, access_key,[wifi_ssid],[wifi_password])`
Connects to Meeo. To get your `namespace` and `access_key`, check our guide [here](https://medium.com/meeo/meeo-credentials-e84db15c7978). 

To use WiFi, provide your wifi networks' SSID (`wifi_ssid`) and password (`wifi_password`). The library will automatically handles the WiFi connectivity. **NOTE:** For ESP8266-based boards, if WiFi credentials are not provided, it will try to run as SoftAP (Hotspot) where the credentials can be set via REST calls. Check [Running on SoftAP mode](#softap-mode) below for more details. This feature is useful if you want to deploy your project on a different network without re-flashing your board.

This function will return `true` for a successful initialization/connection, `false` otherwise. Enable [debug mode](#debug-mode) to see detailed logs.

Example:
```c++
void setup(){
  Serial.begin(115200);
  if( !Meeo.begin("my_namespace","my_access_key","OpenWiFi","qwerty123")) {
    Serial.println("Can't connect to Meeo servers");
  }
  // YOUR CODE HERE
  // ...
}
```
-------------------------------------------------------

<a name="function-run"></a>
### `Meeo.run()`
In most cases, Arduinos run on a single thread thus new data coming from Meeo are buffered before getting processed. Add this inside `loop()` function to monitor data changes from Meeo. Example:
```c++
void loop() {
  Meeo.run();
  
  // YOUR CODE HERE
  // ...
}
```

<a name="debug-mode"></a>
## Enabling debug mode

<a name="softap-mode"></a>
## Running on SoftAP mode
