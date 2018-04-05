/*
  RemoteApplianceControl by Meeo

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.xyz and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino

  Make your own Smart Home by turning your appliances on/off remotely using a
  relay
  More details of the project here:
  https://medium.com/meeo/meeo-project-remote-appliance-control-c34cf5c8e853

  Copyright: Meeo
  Author: Terence Anton Dela Fuente
  License: MIT
*/

#include <Meeo.h>

// Uncomment if you wish to see the events on the Meeo dashboard
// #define LOGGER_CHANNEL "logger"

#define RELAY_PIN D1

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String ssid = "MyWiFi";
String pass = "qwerty123";
String remoteApplianceControlChannel = "remote-appliance-control";

void setup() {
  Serial.begin(115200);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataHandler);
  Meeo.begin(nameSpace, accessKey, ssid, pass);

#ifdef LOGGER_CHANNEL
  Meeo.setLoggerChannel(LOGGER_CHANNEL);
#endif

  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
  Meeo.run();
}

void meeoDataHandler(String topic, String payload) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  if (Meeo.isChannelMatched(topic, remoteApplianceControlChannel)) {
    if (payload.toInt() == 1) {
      digitalWrite(RELAY_PIN, HIGH);
#ifdef LOGGER_CHANNEL
      Meeo.println("[INFO] Appliance is turned on");
#endif
    } else {
      digitalWrite(RELAY_PIN, LOW);
#ifdef LOGGER_CHANNEL
      Meeo.println("[INFO] Appliance is turned off");
#endif
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
      Meeo.subscribe(remoteApplianceControlChannel);
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
