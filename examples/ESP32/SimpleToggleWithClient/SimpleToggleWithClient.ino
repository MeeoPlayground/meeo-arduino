/*
  SimpleToggle by BeeGee

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.io and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino

  This is a very simple example for ESP32 modules.
	It will read the status of a TOGGLE widget from Meeo
	If the toggle is on, a LED on the ESP32 module will be light up
	The toggle status is sent back to Meeo to be displayed in an
	ON/OFF STATE widget
  
  This examples connects to a WiFi network before calling Meeo.begin().
  This can be usefull if you use a captive connection portal like
  WiFiManager (https://github.com/tzapu/WiFiManager).

	Before running this app, simple create on the Meebo Dashboard two widgets:
	TOGGLE widget named toggle
	ON/OFF STATE widget named onoff

  Copyright: BeeGee
  Author: Bernd Giesecke
  License: MIT
*/
#include <Arduino.h>
#include <Meeo.h>

// IO port for the onboard LED
int ledPin  = 16;

// WiFi network name and password:
const char * networkName = "MHC2";
const char * networkPswd = "qwerty123";

// Meeo name space and access key
String nameSpace = "my_namespace";
String accessKey = "my_access_key";

// Meeo channel names
String toggleChannel = "toggle";
String onOffChannel = "onoff";

// WiFi client to access MEEO
WiFiClient meeoClient;

// Uncomment if you wish to see the events on the Meeo dashboard
#define LOGGER_CHANNEL "logger"

// PlatformIO requires to have functions declared before first use
void meeoDataHandler(String topic, String payload);
void meeoEventHandler(MeeoEventType event);

void setup() {
	Serial.begin(115200);
	Serial.setDebugOutput(true);

	Serial.println("Connecting to WiFi");
	WiFi.begin(networkName, networkPswd);

	 while (WiFi.status() != WL_CONNECTED) {
			 delay(500);
			 Serial.print(".");
	 }

	 Serial.println("");
	 Serial.println("WiFi connected");
	 Serial.println("IP address: ");
	 Serial.println(WiFi.localIP());

	 Serial.println("Connecting to WiFi and Meeo");
	// Define handler for Meeo and WiFi events
	Meeo.setEventHandler(meeoEventHandler);
	// Define handler for incoming Meeo messages
	Meeo.setDataReceivedHandler(meeoDataHandler);
	// Connect to WiFi and Meeo
	Serial.println("Connecting to Meeo");
	Meeo.begin(nameSpace, accessKey, meeoClient);

#ifdef LOGGER_CHANNEL
	Meeo.setLoggerChannel(LOGGER_CHANNEL);
#endif

	pinMode(ledPin, OUTPUT);
}

void loop() {
    Meeo.run();
}

void meeoDataHandler(String topic, String payload) {
	Serial.print("Received from Meeo: topic = ");
  Serial.print(topic);
  Serial.print(" payload = ");
  Serial.println(payload);

  if (Meeo.isChannelMatched(topic, toggleChannel)) {
    if (payload.toInt() == 1) {
			// Switch on the LED
      digitalWrite(ledPin, LOW);
			// Send status back to Meeo
			Meeo.publish(onOffChannel, (char *) "1");
    } else {
			// Switch off the LED
      digitalWrite(ledPin, HIGH);
			// Send status back to Meeo
			Meeo.publish(onOffChannel, (char *) "0");
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
			// Subscribe to the toggle button on Meeo
			Meeo.subscribe(toggleChannel);
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
