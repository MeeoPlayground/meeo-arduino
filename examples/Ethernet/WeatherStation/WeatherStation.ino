/*
  WeatherStation by Meeo

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.io and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino

  OTHER REQUIREMENTS
  Under Sketch > Include Library > Manage Libraries...
  Search and install the following:
  * DHT sensor library by Adafruit
  * Adafruit Unified Sensor by Adafruit

  Remotely monitor the weather in a small area using a
  simple DHT11 with Temperature and Humidity sensor builtin, plus
  Water sensor!
  More details of the project here: https://meeo.io/l/1001

  Copyright: Meeo
  Author: Terence Anton Dela Fuente
  License: MIT
*/

#include <Meeo.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define WATERPIN A0
// If water presence value is less than WATER_LEVEL_THRESHOLD, it means
// water is present
#define WATER_LEVEL_THRESHOLD 200
#define DHTPIN 2
#define DHTTYPE DHT11

unsigned long previous = 0;

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String temperatureChannel = "ambient-temperature";
String humidityChannel = "ambient-humidity";
String waterPresenceChannel = "water-presence";

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

  pinMode(WATERPIN, INPUT);
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

    if (!isnan(temperature) && temperature <= 100) {
      Meeo.publish(temperatureChannel, String(temperature) + "C");
    }

    if (!isnan(humidity) && humidity <= 100) {
      Meeo.publish(humidityChannel, String(humidity));
    }

    int value = analogRead(WATERPIN);
    // If water presence value is less than WATER_LEVEL_THRESHOLD, it means
    // water is present
    if(value < WATER_LEVEL_THRESHOLD){
      Meeo.publish(waterPresenceChannel, "on");
    } else {
      Meeo.publish(waterPresenceChannel, "off");
    }
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
