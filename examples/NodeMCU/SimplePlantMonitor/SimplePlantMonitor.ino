/*
  SimplePlantMonitor by Meeo

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.xyz and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino

  OTHER REQUIREMENTS
  Under Sketch > Include Library > Manage Libraries...
  Search and install the following:
  * DHT sensor library by Adafruit
  * Adafruit Unified Sensor by Adafruit

  --- IMPORTANT NOTE ---
  ESP8266 board's analog pin is only 1volt tolerant. Use a voltage divider
  or any other means to bring down the voltage within 0 ~ 1 volt.



  Remotely monitor your plants using a simple DHT11 with Temperature
  and Humidity sensor builtin, plus
  Soil Moisture sensor!
  More details of the project here:
  https://medium.com/meeo/meeo-project-simple-plant-monitor-b2bb9d6f2dc6

  Copyright: Meeo
  Author: Terence Anton Dela Fuente
  License: MIT
*/

#include <Meeo.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Uncomment if you wish to see the events on the Meeo dashboard
// #define LOGGER_CHANNEL "logger"

#define MOISTUREPIN A0
#define DHTPIN D2
#define DHTTYPE DHT11

unsigned long previous = 0;

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String ssid = "MyWiFi";
String pass = "qwerty123";
String temperatureChannel = "plant-ambient-temperature";
String humidityChannel = "plant-ambient-humidity";
String soilMoistureChannel = "plant-soil-moisture";

DHT_Unified dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataHandler);
  Meeo.begin(nameSpace, accessKey, ssid, pass);

#ifdef LOGGER_CHANNEL
  Meeo.setLoggerChannel(LOGGER_CHANNEL);
#endif

  //Initialize your DHT Sensor
  dht.begin();

  pinMode(MOISTUREPIN, INPUT);
}

void loop() {
  Meeo.run();

  unsigned long now = millis();
  if (now - previous >= 10000) {
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
    int soilMoistureLevel = map(analogRead(MOISTUREPIN), 0, 1023, 0, 100);
    Meeo.publish(soilMoistureChannel, String(soilMoistureLevel));

#ifdef LOGGER_CHANNEL
    if (soilMoistureLevel < 30) {
      Meeo.println("[WARNING] Your plant is getting dry!");
    }
#endif
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
