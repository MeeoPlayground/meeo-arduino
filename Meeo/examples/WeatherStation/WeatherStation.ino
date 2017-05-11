#include <Uttr.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN D2
#define DHTTYPE DHT11

unsigned long previous = 0;

const char * ssid = "your-router-ssid";
const char * pass = "your-router-password";
const char * deviceId = "enter-any-unique-id";
const char * username = "your-username";
const char * accessKey = "your-access-key";
const char * nameSpace = "your-name-space";
const char * temperatureChannel = "your-temperature-channel";
const char * humidityChannel = "your-humidity-channel";

DHT_Unified dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Uttr.onEvent(eventListener);
  Uttr.begin(deviceId, username, accessKey, nameSpace, ssid, pass);
  Uttr.setCallback(callback);

  dht.begin();
}

void loop() {
  Uttr.run();

  unsigned long now = millis();
  if (now - previous > 3000) {
    previous = now;

    sensors_event_t event;

    dht.temperature().getEvent(&event);
    int temperature = event.temperature;

    dht.humidity().getEvent(&event);
    int humidity = event.relative_humidity;

    if (!isnan(temperature) && temperature <= 100) {
      String sTemp = String(temperature) + "C";
      Uttr.publish(temperatureChannel, sTemp.c_str(), true);
    }

    if (!isnan(humidity) && humidity <= 100) {
      Uttr.publish(humidityChannel, String(humidity).c_str(), true);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  String sTopic = Uttr.toString(topic);
  String sPayload = Uttr.toString(payload, payloadLength);
  Serial.print(sTopic);
  Serial.print(": ");
  Serial.println(sPayload);
}

void eventListener(ConnectionEvent event) {
  switch (event) {
    case NOT_CONNECTED_TO_WIFI:
      Serial.println("Not Connected to WiFi");
      break;
    case CONNECTING_TO_WIFI:
      Serial.println("Connecting to WiFi");
      break;
    case CONNECTED_TO_WIFI:
      Serial.println("Connected to WiFi");
      break;
    case NOT_CONNECTED_TO_MQTT_SERVER:
      Serial.println("Not Connected to MQTT Server");
      break;
    case CONNECTED_TO_MQTT_SERVER:
      Serial.println("Connected to MQTT Server");
      Uttr.subscribe(temperatureChannel);
      Uttr.subscribe(humidityChannel);
      break;
    case AP_MODE:
      Serial.println("AP Mode");
      break;
    default:
      break;
  }
}
