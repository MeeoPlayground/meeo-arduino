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
* [`Meeo.begin(nameSpace, accessKey,[ssid],[pass])`](#function-begin)
* [`Meeo.begin(nameSpace, accessKey,client)`](#function-begin-1)
* [`Meeo.run()`](#function-run)
* [`Meeo.setEventHandler(void (*f)(MeeoEventType))`](#function-seteventhandler)
* [`Meeo.setDataReceivedHandler(void (*f)(topic,payload))`](#function-setdatareceivedhandler)
* [`Meeo.subscribe(channel)`](#function-subscribe)
* [`Meeo.publish(channel,payload)`](#function-publish)


-------------------------------------------------------
<a name="function-begin"></a>
#### `Meeo.begin(String nameSpace, String accessKey, String ssid, String pass)`
Connects to Meeo. To get your `nameSpace` and `accessKey`, check our guide [here](https://medium.com/meeo/meeo-credentials-e84db15c7978). 

To use WiFi, provide your wifi networks' SSID (`ssid`) and password (`pass`). The library will automatically handles the WiFi connectivity initialization. **NOTE:** For ESP8266-based boards, if WiFi credentials are not provided or the board can't connect to previously set network, this function will not return. Instead it will try to run the board in **AP Mode**(Hotspot) where the credentials can be set via REST calls. Check [Running in AP Mode](#ap-mode) below for more details. This feature is useful if you want to deploy your project on a different network without re-flashing your board.

Enable [debug mode](#debug-mode) to see detailed logs.

Example:
```c++
void setup(){
  Serial.begin(115200);
  Meeo.begin("my_namespace","my_access_key","MyWiFi","qwerty123"));
}
```
-------------------------------------------------------
<a name="function-begin-1"></a>
#### `Meeo.begin(String nameSpace, String accessKey, Client client)`
To use a different means to connect to the internet, you can provide a [`Client`](https://github.com/arduino/Arduino/blob/master/hardware/arduino/avr/cores/arduino/Client.h) instance instead. This is applicable for scenarios such using Arduino Yun board or Yun Shield with `YunClient` or Arduino Ethernet shield with `EthernetClient`. *NOTE* For Arduino Yun board or Yun Shield using `YunClient`, make sure to call `Bridge.begin()` first before calling `Meeo.begin()`.

Example:
```c++
EthernetClient ethClient;
void setup(){
  Serial.begin(115200);
  Ethernet.begin(mac);
  Meeo.begin("my_namespace","my_access_key",ethClient));
}
```
-------------------------------------------------------
<a name="function-run"></a>
#### `Meeo.run()`
In most cases, Arduinos run on a single thread thus new data coming from Meeo are buffered before getting processed. Add this inside `loop()` function to monitor data changes from Meeo. Example:
```c++
void loop() {
  Meeo.run();
  
  // YOUR CODE HERE
  // ...
}
```
-------------------------------------------------------
<a name="function-seteventhandler"></a>
#### `Meeo.setEventHandler(void (*f)(MeeoEventType))`
Sets the handler of generic events coming from the subsystem. This is useful if you want to handle system status changes such WiFi connectivity process. **IMPORTANT NOTE:** Set the handler before calling `begin()`; `begin()` function triggers several events relevant to initialization.

`MeeoEventType` is an enum and the available values are:
* `WIFI_CONNECTING`
* `WIFI_CONNECTED`
* `WIFI_DISCONNECTED`
* `MQ_CONNECTED`
* `MQ_DISCONNECTED`
* `MQ_BAD_CREDENTIALS`
* `MQ_ERROR`
* `AP_MODE`


Example:
```c++
void setup(){
  Serial.begin(115200);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.begin("my_namespace","my_access_key","MyWiFi","qwerty123"));
}

...

void meeoEventHandler(MeeoEventType eventType){
  switch (eventType) {
    case WIFI_CONNECTED:
      Serial.println("Connected to WiFi");
      break;
    case MQ_CONNECTED:
      Serial.println("Operations are all green!");
      Uttr.subscribe(channel);
      break;
    case AP_MODE:
      Serial.println("Running in AP Mode");
      break;
    default:
      break;
  }
}
```
-------------------------------------------------------
<a name="function-setdatareceivedhandler"></a>
#### `Meeo.setDataReceivedHandler(void (*f)(String topic, String payload))`
Sets the function callback to be triggered once there are available data from the server. The function expects data from topics registered via `subscribe()` calls. 

Parameters are
* `topic` - raw MQTT topic source of the data
* `payload` - data payload

To compare raw MQTT topics to channel, use `Meeo.isChannelMatched(rawTopic,channel)`. This function returns `true` if the `channel` prepended with the `nameSpace` provided during `begin()` is equal to `rawTopic`; `false` otherwise.


Example:
```c++
void setup(){
  Serial.begin(115200);


  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataReceivedHandler);
  Meeo.begin("my_namespace","my_access_key","MyWiFi","qwerty123"));
}

...

void meeoDataHandler(String topic, String payload) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  if (Meeo.isChannelMatched(topic, channel)) {
    if (payload.toInt() == 1) {
      digitalWrite(CONTROLLABLE_COMPONENT, LOW);
    } else {
      digitalWrite(CONTROLLABLE_COMPONENT, HIGH);
    }
  }
}
```
-------------------------------------------------------
<a name="function-subscribe"></a>
#### `Meeo.subscribe(String channel)`
Let's your device listen to one of your channels. Channel is simply an MQTT topic without your `namespace` (So you don't need to repeat yourself for every subscribe). 

Example: 
```c++
void setup(){
  Serial.begin(115200);


  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataReceivedHandler);
  Meeo.begin("my_namespace","my_access_key","MyWiFi","qwerty123"));
}

...

void meeoEventHandler(MeeoEventType event) {
  switch (event) {
    case WIFI_DISCONNECTED:
      Serial.println("Not Connected to WiFi");
      break;
    case WIFI_CONNECTING:
      Serial.println("Connecting to WiFi");
      break;
    case WIFI_CONNECTED:
      Serial.println("Connected to WiFi");
      break;
    case MQ_DISCONNECTED:
      Serial.println("Not Connected to MQTT Server");
      break;
    case MQ_CONNECTED:
      Serial.println("Connected to MQTT Server");
      Meeo.subscribe("kitchen-lights");
      Meeo.subscribe("door-lock-state");
      break;
    case MQ_BAD_CREDENTIALS:
      Serial.println("Bad Credentials");
      break;
    case AP_MODE:
      Serial.println("AP Mode");
      break;
    default:
      break;
  }
}
```

<a name="debug-mode"></a>
## Enabling debug mode

<a name="ap-mode"></a>
## Running in AP mode

