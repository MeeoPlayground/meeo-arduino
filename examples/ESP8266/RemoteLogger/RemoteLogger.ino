/*
  RemoteLogger by Meeo

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.io and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino

  Log important events and see it anywhere you go.
  More details of the project here: https://meeo.io/l/1000

  Copyright: Meeo
  Author: Terence Anton Dela Fuente
  License: MIT
*/

#include <Meeo.h>

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String ssid = "MyWiFi";
String pass = "qwerty123";
String loggerChannel = "button-logs";

#define BUTTON_A_PIN D1
#define BUTTON_B_PIN D2
uint8_t previousButtonAState = LOW;
uint8_t previousButtonBState = LOW;

unsigned long previous = 0;

void setup() {
  Serial.begin(115200);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataHandler);
  Meeo.begin(nameSpace, accessKey, ssid, pass);
  Meeo.setLoggerChannel(loggerChannel);

  pinMode(BUTTON_A_PIN,INPUT);
  pinMode(BUTTON_B_PIN,INPUT);
}

void loop() {
  Meeo.run();

  uint8_t buttonAState = digitalRead(BUTTON_A_PIN);
  if( buttonAState != previousButtonAState){
    delay(100); //Debounce
    buttonAState = digitalRead(BUTTON_A_PIN);

    if(buttonAState != previousButtonAState){
      previousButtonAState = buttonAState;

      if(buttonAState == HIGH){
        Meeo.println("[INFO] Button A Pressed! " + String(millis()));
      }
    }
  }
  uint8_t buttonBState = digitalRead(BUTTON_B_PIN);
  if( buttonBState != previousButtonBState){
    delay(100); //Debounce
    buttonBState = digitalRead(BUTTON_B_PIN);

    if(buttonBState != previousButtonBState){
      previousButtonBState = buttonBState;

      if(buttonBState == HIGH){
        Meeo.println("[ERROR] Button B Pressed! " + String(millis()));
      }
    }
  }


}

void meeoDataHandler(String topic, String payload) {

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
