#include <Meeo.h>
#include <SPI.h>
#include <Ethernet.h>

#define CONTROLLABLE_COMPONENT 8

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String channel = "my-channel";

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient ethClient;

void setup() {
  Serial.begin(115200);

  Ethernet.begin(mac);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataHandler);
  Meeo.begin(nameSpace, accessKey, ethClient);

  pinMode(CONTROLLABLE_COMPONENT, OUTPUT);
  digitalWrite(CONTROLLABLE_COMPONENT, LOW);
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
      digitalWrite(CONTROLLABLE_COMPONENT, HIGH);
    } else {
      digitalWrite(CONTROLLABLE_COMPONENT, LOW);
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
