/*
  PartyLights by Meeo

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.io and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino

  Pump-up your room with a set of Party Lights!
  More details of the project here: https://meeo.io/l/1000

  Copyright: Meeo
  Author: Terence Anton Dela Fuente
  License: MIT
*/

#include <Meeo.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_NeoPixel.h>

#define PIN 3
// Number of WS2812B (or NeoPixel) in your light chain
#define NUMPIXELS 4

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String colorChannel = "party-lights-color";
String speedChannel = "party-lights-speed";

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int r, g, b;
unsigned long duration;

int pixelCount = 0;
unsigned long colorMillis;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient ethClient;

void setup() {
  Serial.begin(115200);

  Ethernet.begin(mac);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataHandler);
  Meeo.begin(nameSpace, accessKey, ethClient);

  pixels.begin();

  //Reset all LEDs to off
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();
}

void loop() {
  Meeo.run();

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

void meeoDataHandler(String topic, String payload) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  if (Meeo.isChannelMatched(topic, colorChannel)) {
    Meeo.convertStringToRGB(payload, &r, &g, &b);
  } else if (Meeo.isChannelMatched(topic, speedChannel)) {
    duration = payload.toInt();
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

      //Once connected, subscribe to the channels
      Meeo.subscribe(colorChannel);
      Meeo.subscribe(speedChannel);
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

void turnOffPixels(int ctr) {
  for (int i = 0; i < NUMPIXELS; i++) {
    if (i != ctr) {
      pixels.setPixelColor(i, pixels.Color(0,0,0));
      pixels.show();
    }
  }
  pixels.show();
}
