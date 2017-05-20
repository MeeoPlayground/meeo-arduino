/*
  SimplePlantMonitor by Meeo

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.io and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino

  OTHER REQUIREMENTS
  Under Sketch > Include Library > Manage Libraries...
  Search and install the following:
  * DHT sensor library by Adafruit
  * Adafruit Unified Sensor by Adafruit

  Remotely monitor your plants using a simple DHT11 with Temperature
  and Humidity sensor builtin, plus
  Soil Moisture sensor!
  More details of the project here: https://meeo.io/l/1001

  Copyright: Meeo
  Author: Terence Anton Dela Fuente
  License: MIT
*/

#include <Meeo.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SPI.h>
#include <Ethernet.h>

#define MOISTUREPIN A0
#define DHTPIN 3
#define DHTTYPE DHT11

unsigned long previous = 0;

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String temperatureChannel = "plant-ambient-temperature";
String humidityChannel = "plant-ambient-humidity";
String soilMoistureChannel = "plant-soil-moisture";

DHT_Unified dht(DHTPIN, DHTTYPE);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient ethClient;

void setup() {
  Serial.begin(115200);

  Ethernet.begin(mac);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataHandler);
  Meeo.begin(nameSpace, accessKey, ethClient);

  //Initialize your DHT Sensor
  dht.begin();

  pinMode(MOISTUREPIN,INPUT);
}

void loop() {
  Meeo.run();

  unsigned long now = millis();
  if (now - previous > 3000) {
    previous = now;

    sensors_event_t event;

    dht.temperature().getEvent(&event);
    int temperature = event.temperature;

    dht.humidity().getEvent(&event);
    int humidity = event.relative_humidity;

    if (!isnan(temperature) && temperature <= 200) {
      Meeo.publish(temperatureChannel, String(temperature) + "C");
    }

    if (!isnan(humidity) && humidity <= 100) {
      Meeo.publish(humidityChannel, String(humidity));
    }

    // Convert raw analog value (0 - 1023) to percent (0-100)
    //
    // Note that moisture value does not reach 1023 and might stay at a much lower range.
    // Change 1023 as you please to calibrate your results
    Meeo.publish(soilMoistureChannel, String(map(analogRead(MOISTUREPIN), 0, 1023, 0, 100)));
  }
}

void meeoDataHandler(String topic, String payload) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);
}

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
