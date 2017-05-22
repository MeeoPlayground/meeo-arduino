/*
  ProjectTemplate by Meeo

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.io and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino

  Let this be your starting project template for Meeo.
  More details of the project here: https://meeo.io/l/1000

  Copyright: Meeo
  Author: Terence Anton Dela Fuente
  License: MIT
*/

#include <Meeo.h>
#include <SPI.h>
#include <Ethernet.h>

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String channel = "loopback";

unsigned long previous = 0;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient ethClient;

void setup() {
  Serial.begin(115200);

  Ethernet.begin(mac);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataHandler);
  Meeo.begin(nameSpace, accessKey, ethClient);
}

void loop() {
  Meeo.run();

  if (millis() - previous >= 1000) {
    previous = millis();
    String message = "stamp_" + String(previous);
    Serial.print("Sent: ");
    Serial.println(message);
    Meeo.publish(channel, message);
  }
}

void meeoDataHandler(String topic, String payload) {
  if (Meeo.isChannelMatched(topic, channel)) {
    Serial.print("Received: ");
    Serial.println(payload);
  }
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
      Meeo.subscribe(channel);
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
