/*
  IPLocation by Meeo

  This example will make use of Meeo. If you haven't already,
  visit Meeo at https://meeo.xyz and create an account. Then
  check how to get started with the Meeo library through
  https://github.com/meeo/meeo-arduino

  OTHER REQUIREMENTS
  Under Sketch > Include Library > Manage Libraries...
  Search and install the following:
  * ESP8266RestClient by fabianofranca
  * ArduinoJson by Benoit Blanchon

  Know your GPS location based on your IP address with ipapi.co!
  More details of the project here:
  https://medium.com/meeo/meeo-project-ip-location-4a81316c0b30

  Copyright: Meeo
  Author: Terence Anton Dela Fuente
  License: MIT
*/

#include <Meeo.h>
#include <RestClient.h>
#include <ArduinoJson.h>

// Uncomment if you wish to see the events on the Meeo dashboard
// #define LOGGER_CHANNEL "logger"

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String ssid = "MyWiFi";
String pass = "qwerty123";
String ipLocationChannel = "ip-location";

// ipapi.co will serve as our GPS tracker based on the current IP address of the
// device
RestClient client = RestClient("ipapi.co");
DynamicJsonBuffer jsonBuffer;

unsigned long previous = 0;

void setup() {
  Serial.begin(115200);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataHandler);
  Meeo.begin(nameSpace, accessKey, ssid, pass);

  #ifdef LOGGER_CHANNEL
    Meeo.setLoggerChannel(LOGGER_CHANNEL);
  #endif
}

void loop() {
  Meeo.run();

  unsigned long now = millis();
  // Check value every 10 seconds
  if (now - previous >= 10000) {
    previous = now;

    String response = "";
    int statusCode = client.get("/json", &response);

    if (statusCode == 200) {
      // parse the response from request
      JsonObject& root = jsonBuffer.parseObject(response);
      float latitude = root[String("latitude")];
      float longitude = root[String("longitude")];
      String location = String(latitude, 6) + "," + String(longitude, 6);

      Meeo.publish(ipLocationChannel, location);

#ifdef LOGGER_CHANNEL
      String tempCity = root[String("city")];
      String tempRegion = root[String("region")];
      String tempCountryName = root[String("country_name")];
      String tempTimezone = root[String("timezone")];

      String city = "[INFO] City: " + tempCity;
      String region = "[INFO] Region: " + tempRegion;
      String countryName = "[INFO] Country: " + tempCountryName;
      String timezone = "[INFO] Timezone: " + tempTimezone;

      Meeo.println(city);
      Meeo.println(region);
      Meeo.println(countryName);
      Meeo.println(timezone);
#endif
    }
  }
}

void meeoDataHandler(String topic, String payload) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);
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
