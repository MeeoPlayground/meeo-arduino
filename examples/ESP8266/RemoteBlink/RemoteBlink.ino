/*
  RemoteBlink by Meeo

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.io and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino

  Super charge your very basic LED project by connecting and
  controlling it over the internet!
  More details of the project here: https://meeo.io/l/1000

  Copyright: Meeo
  Author: Terence Anton Dela Fuente
  License: MIT
*/

#include <Meeo.h>

#define LED_PIN LED_BUILTIN

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String ssid = "MyWiFi";
String pass = "qwerty123";
String channel = "remote-blink";

void setup() {
  Serial.begin(115200);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataHandler);
  Meeo.begin(nameSpace, accessKey, ssid, pass);

  pinMode(LED_PIN, OUTPUT);
  //ESP8266 builtin LED is Active Low. Set it to HIGH to turn it off.
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  Meeo.run();
}

void meeoDataHandler(String topic, String payload) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  if (Meeo.isChannelMatched(topic, channel)) {
    if (payload.toInt() == 1) {
      digitalWrite(LED_PIN, LOW);
    } else {
      digitalWrite(LED_PIN, HIGH);
    }
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
      // Once connected, subscribe to the channel
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
