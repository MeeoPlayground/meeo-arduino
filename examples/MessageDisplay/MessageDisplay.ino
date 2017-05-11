#include <Uttr.h>
#include <SH1106.h>

const char * ssid = "your-router-ssid";
const char * pass = "your-router-password";
const char * deviceId = "enter-any-unique-id";
const char * username = "your-username";
const char * accessKey = "your-access-key";
const char * nameSpace = "your-name-space";
const char * textChannel = "your-text-channel";

SH1106 display(0x3c, 14, 12);

void setup() {
  Serial.begin(115200);
  Uttr.onEvent(eventListener);
  Uttr.begin(deviceId, username, accessKey, nameSpace, ssid, pass);
  Uttr.setCallback(callback);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
}

void loop() {
  Uttr.run();
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  String sTopic = Uttr.toString(topic);
  String sPayload = Uttr.toString(payload, payloadLength);
  Serial.print(sTopic);
  Serial.print(": ");
  Serial.println(sPayload);

  if (Uttr.isTopicEqual(sTopic, textChannel)) {
    display.clear();
    display.drawString(64, 24, sPayload.c_str());
    display.display();
  }
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
      Uttr.subscribe(textChannel);
      break;
    case AP_MODE:
      Serial.println("AP Mode");
      break;
    default:
      break;
  }
}
