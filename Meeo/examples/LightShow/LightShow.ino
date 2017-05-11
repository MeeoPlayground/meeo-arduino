#include <Uttr.h>
#include <Adafruit_NeoPixel.h>

#define PIN D3
#define NUMPIXELS 4

unsigned long previous = 0;

const char * ssid = "your-router-ssid";
const char * pass = "your-router-password";
const char * deviceId = "enter-any-unique-id";
const char * username = "your-username";
const char * accessKey = "your-access-key";
const char * nameSpace = "your-name-space";
const char * colorChannel = "your-color-channel";
const char * speedChannel = "your-speed-channel";

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int r, g, b;
unsigned long duration;

int pixelCount = 0;
unsigned long colorMillis;

void setup() {
  Serial.begin(115200);
  Uttr.onEvent(eventListener);
  Uttr.begin(deviceId, username, accessKey, nameSpace, ssid, pass);
  Uttr.setCallback(callback);

  pixels.begin();

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();
}

void loop() {
  Uttr.run();

  unsigned long currentMillis = millis();

  if (duration == 0) {
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0,0,0));
    }
    pixels.show();
  } else if (duration == 100) {
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(r,g,b));
    }
    pixels.show();
  } else {
    if (currentMillis - colorMillis >= (100 - duration)) {
      colorMillis = currentMillis;
      pixels.setPixelColor(pixelCount, pixels.Color(r,g,b));
      turnOffPixels(pixelCount);
      pixelCount++;
    }
  }
  if (pixelCount >= NUMPIXELS) pixelCount = 0;
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  String sTopic = Uttr.toString(topic);
  String sPayload = Uttr.toString(payload, payloadLength);
  Serial.print(sTopic);
  Serial.print(": ");
  Serial.println(sPayload);

  if (Uttr.isTopicEqual(sTopic, colorChannel)) {
    Uttr.stringToRGB(sPayload, &r, &g, &b);
  } else if (Uttr.isTopicEqual(sTopic, speedChannel)) {
    duration = sPayload.toInt();
  }
}

void turnOffPixels(int ctr) {
  for (int i = 0; i < NUMPIXELS; i++) {
    if (i != ctr) {
      pixels.setPixelColor(i, pixels.Color(0,0,0));
      pixels.show();
    }
  }
  pixels.show();
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
      Uttr.subscribe(colorChannel);
      Uttr.subscribe(speedChannel);
      break;
    case AP_MODE:
      Serial.println("AP Mode");
      break;
    default:
      break;
  }
}
