/*
  RoomLightStatus by Meeo

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.io and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino

  Did you left the room lights on again?! Monitor its state
  with a light-dependent resistor (LDR) sensor
  More details of the project here: https://meeo.io/l/1000

  Copyright: Meeo
  Author: Terence Anton Dela Fuente
  License: MIT
*/

#include <Meeo.h>

#define LDRPIN A0
// If the value is greater than VALUE_THRESHOLD, then it is considered
// that the room is illuminated
#define VALUE_THRESHOLD 710

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String ssid = "MyWiFi";
String pass = "qwerty123";
String channel = "room-light-status";

unsigned long previous = 0;

void setup() {
  Serial.begin(115200);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataHandler);
  Meeo.begin(nameSpace, accessKey, ssid, pass);

  pinMode(LDRPIN, INPUT);
}

void loop() {
  Meeo.run();

  long now = millis();
  // Check value every 5 seconds
  if (now - previous >= 5000) {
    previous = now;
    int value = analogRead(LDRPIN);
    Serial.print("Value: ");
    Serial.println(value);
    if (value > VALUE_THRESHOLD) {
      Meeo.publish(channel, "on");
    } else {
      Meeo.publish(channel, "off");
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
