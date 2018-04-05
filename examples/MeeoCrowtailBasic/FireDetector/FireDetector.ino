/*
  FireDetector by Meeo

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.xyz and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino

  Equip your home with this Do-It-Yourself Fire Detector
  More details of the project here:
  https://medium.com/meeo/meeo-project-fire-detector-9a658b6c25d1

  Copyright: Meeo
  Author: Terence Anton Dela Fuente
  License: MIT
*/

#include <Meeo.h>

// Uncomment if you wish to see the events on the Meeo dashboard
// #define LOGGER_CHANNEL "logger"

#define FLAME_SENSOR_PIN D1

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String ssid = "MyWiFi";
String pass = "qwerty123";
String flameSensingChannel = "flame-sensing-state";

unsigned long previous = 0;

void setup() {
  Serial.begin(115200);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.begin(nameSpace, accessKey, ssid, pass);

#ifdef LOGGER_CHANNEL
  Meeo.setLoggerChannel(LOGGER_CHANNEL);
#endif

  pinMode(FLAME_SENSOR_PIN, INPUT);
}

void loop() {
  Meeo.run();

  unsigned long now = millis();
  // Check value every second
  if (now - previous >= 1000) {
    previous = now;
    int state = digitalRead(FLAME_SENSOR_PIN);
    // Flame sensor is Active Low. It turns to LOW when a presence of flame
    // is detected
    if (state == LOW) {
      Meeo.publish(flameSensingChannel, "1");
#ifdef LOGGER_CHANNEL
      Meeo.println("[WARNING] Flame detected");
#endif
    } else {
      Meeo.publish(flameSensingChannel, "0");
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
      Meeo.subscribe(flameSensingChannel);
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
