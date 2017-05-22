/*
  RemoteServoControl by Meeo

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.io and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino


  Remotely control a servo motor
  More details of the project here: https://meeo.io/l/1001

  Copyright: Meeo
  Author: Terence Anton Dela Fuente
  License: MIT
*/

#include <Meeo.h>
#include <Servo.h>

#define SERVO_PIN D1
#define MAX_ANGLE 180
#define MIN_ANGLE 0

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String ssid = "MyWiFi";
String pass = "qwerty123";
String channel = "servo-sweep";

Servo servo;

void setup() {
  Serial.begin(115200);

  servo.attach(SERVO_PIN);
  // Set to starting position
  servo.write(MIN_ANGLE);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataHandler);
  Meeo.begin(nameSpace, accessKey, ssid, pass);
}

void loop() {
  Meeo.run();
}

void meeoDataHandler(String topic, String payload) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  if (Meeo.isChannelMatched(topic, channel)) {
    servo.write(map(payload.toInt(), 0, 100, MIN_ANGLE, MAX_ANGLE));
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
