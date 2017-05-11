#include <Meeo.h>

const char * nameSpace = "tdf-b20hfmlb";
const char * accessKey = "user_qVMRgrK4sQduDK6n";
const char * channel = "door-lock-state";

void setup() {
  Serial.begin(115200);

  Meeo.setEventHandler(eventHandler);
  Meeo.setDataReceivedHandler(dataHandler);
  Meeo.begin(nameSpace, accessKey, "CIRCUITROCKSLOL", "********");
}

void loop() {
  Meeo.run();
}

void dataHandler(char* topic, byte* payload, unsigned int payloadLength) {
  String sTopic = Meeo.convertToString(topic);
  String sPayload = Meeo.convertToString(payload, payloadLength);
  Serial.print(sTopic);
  Serial.print(": ");
  Serial.println(sPayload);
}

void eventHandler(MeeoEventType event) {
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
      Serial.println("Bad Creds");
    case AP_MODE:
      Serial.println("AP Mode");
      break;
    default:
      break;
  }
}
