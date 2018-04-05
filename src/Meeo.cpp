/*
    Arduino Library for Meeo
    https://meeo.xyz

    MIT License

    Copyright (c) 2017 Meeo by Circuitrocks

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#include "Meeo.h"

// Meeo server defaults
String mqttServer = "mq.meeo.xyz";
int mqttPort = 1883;
MeeoCore Meeo;

String _ssid;
String _pass;
String _macAddress;

// Due to limitations with the PubSubClient, only one (1) data handler can be registered
// at any given time. Which is fine since our aim is to just have a single instance
// of MeeoCore
static void _callbackHandler(char * topic, uint8_t * payload, unsigned int payloadLength);
void (* _dataReceivedHandler)(String, String) = NULL;


#ifdef ESP8266
    WiFiServer server(80);
    WiFiClient espClient;
    PubSubClient pubSubClient(espClient);
#elif defined __AVR
    PubSubClient pubSubClient;
#endif

#ifdef DEBUG_MEEO
    template <typename Generic>
    void meeo_log(Generic text) {
        Serial.print("[Meeo] ");
        Serial.println(text);
    }
#else
    #define meeo_log(l);
#endif

#ifdef ESP8266
    void MeeoCore::begin(String nameSpace, String accessKey, String ssid, String pass) {
        meeo_log(F("Board detected: ESP8266"));
        beginMeeo(nameSpace, accessKey, ssid, pass);
    }
#elif defined __AVR
    void MeeoCore::begin(String nameSpace, String accessKey, Client & client) {
        meeo_log(F("Board detected: Arduino"));
        beginMeeo(nameSpace, accessKey, client);
    }
#endif

//Main Methods
void MeeoCore::run() {
    if (!pubSubClient.connected()) {
        this->_event = MQ_DISCONNECTED;
        this->_meeoEventHandler(this->_event);
        if (pubSubClient.connect(_macAddress.c_str(), this->_nameSpace.c_str(), this->_accessKey.c_str())) {
            this->_event = MQ_CONNECTED;
            this->_meeoEventHandler(this->_event);
        }
    }
    pubSubClient.loop();
}

void MeeoCore::setEventHandler(void (* f)(MeeoEventType)) {
    this->_meeoEventHandler = f;
}

void MeeoCore::setDataReceivedHandler(void (* f)(String, String)) {
    _dataReceivedHandler = f;
    pubSubClient.setCallback(_callbackHandler);
}

boolean MeeoCore::publish(String channel, String payload, boolean asMqttTopic, boolean retained) {
    // Check if the channel is a raw MQTT topic or just simple a channel (MQTT topic without namespace)
    if (asMqttTopic) {
        return pubSubClient.publish(channel.c_str(), payload.c_str(), retained);
    } else {
        //If passed as channel, prepend the namespace
        String newTopic = this->_nameSpace + "/" + channel;
        return pubSubClient.publish(newTopic.c_str(), payload.c_str(), retained);
    }
}

boolean MeeoCore::subscribe(String channel, uint8_t qos, boolean asMqttTopic) {
    // Check if the channel is a raw MQTT topic or just simple a channel (MQTT topic without namespace)
    if (asMqttTopic) {
        return pubSubClient.subscribe(channel.c_str(), qos);
    } else {
        //If passed as channel, prepend the namespace
        String newTopic = this->_nameSpace + "/" + channel;
        return pubSubClient.subscribe(newTopic.c_str(), qos);
    }
}

boolean MeeoCore::unsubscribe(String channel, boolean asMqttTopic) {
    // Check if the channel is a raw MQTT topic or just simple a channel (MQTT topic without namespace)
    if (asMqttTopic) {
        return pubSubClient.unsubscribe(channel.c_str());
    } else {
        //If passed as channel, prepend the namespace
        String newTopic = this->_nameSpace + "/" + channel;
        return pubSubClient.unsubscribe(newTopic.c_str());
    }
}


void MeeoCore::setLoggerChannel(String channel){
  this->_loggerChannel = this->_nameSpace + "/" + channel;
}
size_t MeeoCore::write(const uint8_t *buffer, size_t size){
  // Ignore if no logger channel defined or data is a simple new line
  if(this->_loggerChannel == NULL || (buffer[0] == '\r' && buffer[1] == '\n')) return 0;
  if(!pubSubClient.publish(this->_loggerChannel.c_str(), (const char *) buffer, false)){
    return 0;
  }
  return size;
}

//No support for this function
size_t MeeoCore::write(uint8_t b){
  return 0;
}

void _callbackHandler(char * topic, uint8_t * payload, unsigned int payloadLength) {
    //If no handler defined, simply return
    if(_dataReceivedHandler == NULL) return;

    String sTopic = Meeo.convertToString(topic);
    String sPayload = Meeo.convertToString(payload, payloadLength);

    _dataReceivedHandler(sTopic, sPayload);
}

//Private Methods

#ifdef ESP8266
    void MeeoCore::beginMeeo(String nameSpace, String accessKey, String ssid, String pass) {
        this->_nameSpace = nameSpace;
        this->_accessKey = accessKey;

        uint8_t mac[WL_MAC_ADDR_LENGTH];
        WiFi.softAPmacAddress(mac);
        String macID = "";

        for (int i = 0; i < WL_MAC_ADDR_LENGTH; i++) {
          if (mac[i] < 10) {
              macID += "0" + String(mac[i], HEX);
          } else {
              macID += String(mac[i], HEX);
          }
        }

        macID.toUpperCase();
        String AP_NameString = "Meeo-" + macID;
        _macAddress = AP_NameString;

        this->_event = WIFI_CONNECTING;
        this->_meeoEventHandler(this->_event);

        WiFi.begin(ssid.c_str(), pass.c_str());
        if (!testWiFi()) {
            server.begin();
            this->_listenForClient = true;
            this->_event = WIFI_DISCONNECTED;
            this->_meeoEventHandler(this->_event);
            setupAP();
        } else {
            this->_event = WIFI_CONNECTED;
            this->_meeoEventHandler(this->_event);

            pubSubClient.setServer(mqttServer.c_str(), mqttPort);
            if (pubSubClient.connect(_macAddress.c_str(), this->_nameSpace.c_str(), this->_accessKey.c_str())) {
                this->_event = MQ_CONNECTED;
                this->_meeoEventHandler(this->_event);
            } else {
                if (pubSubClient.state() == 4) {
                    this->_event = MQ_BAD_CREDENTIALS;
                    this->_meeoEventHandler(this->_event);
                }
            }
        }
    }
#elif defined __AVR
    void MeeoCore::beginMeeo(String nameSpace, String accessKey, Client & client) {
        this->_nameSpace = nameSpace;
        this->_accessKey = accessKey;

        //use analog pin 3 for random seed
        randomSeed(analogRead(3));

        String clientId = String(nameSpace) + "-" + String(random(65536)) + String(random(65536)) + String(random(65536));
        meeo_log("Client ID: " + clientId);

        pubSubClient.setClient(client);
        pubSubClient.setServer(mqttServer.c_str(), mqttPort);
        if (pubSubClient.connect(clientId.c_str(), this->_nameSpace.c_str(), this->_accessKey.c_str())) {
            this->_event = MQ_CONNECTED;
            this->_meeoEventHandler(this->_event);
        } else {
            if (pubSubClient.state() == 4) {
                this->_event = MQ_BAD_CREDENTIALS;
                this->_meeoEventHandler(this->_event);
            }
        }
    }
#endif

#ifdef ESP8266
    void MeeoCore::setupAP() {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(_macAddress.c_str());

        this->_event = AP_MODE;
        this->_meeoEventHandler(this->_event);

        getWiFiCredentials();
    }

    boolean MeeoCore::testWiFi() {
        uint8_t c = 0;

        while (c < 40) {
        if (WiFi.status() == WL_CONNECTED) {
            return 1;
        }
            delay(500);
            c++;
        }

      return 0;
    }

    void MeeoCore::getWiFiCredentials() {
        while (this->_listenForClient) {
            WiFiClient client = server.available();

            if (!client) {
                continue;
            }

            String req = urlDecode(client.readStringUntil('\r'));
            int ssidIndex = req.indexOf("/?ssid=");
            int passIndex = req.indexOf("&pass=");
            int getCredsIndex = req.indexOf("/getCreds");
            int connectIndex = req.indexOf("/connect");

            if (ssidIndex != -1 && passIndex != -1) {
                String tempssid = req.substring(ssidIndex + 7, passIndex);
                String temppass = req.substring(passIndex + 6, req.indexOf(" HTTP"));
                _ssid = tempssid;
                _pass = temppass;

                meeo_log(F("Trying to configure WiFi credentials..."));
                meeo_log("SSID: " + _ssid);
                meeo_log("PASS: " + _pass);

                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: application/json");
                client.println("Connection: close");
                client.println();

                if (tempssid.equals("")) {
                    meeo_log(F("SSID is blank. Connection won't be successful"));
                    client.print("{\"connect\":false}");
                } else {
                    client.print("{\"connect\":true}");
                }
                client.flush();
            }

            if (getCredsIndex != -1) {
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: application/json");
                client.println("Connection: close");
                client.println();

                client.print("{\"ssid\":\"");
                client.print(_ssid);
                client.print("\", \"pass\":\"");
                client.print(_pass);
                client.print("\"}");
                client.flush();
            }

            if (connectIndex != -1) {
                meeo_log(F("Trying to connect to WiFi..."));
                WiFi.mode(WIFI_STA);

                this->_event = WIFI_CONNECTING;
                this->_meeoEventHandler(this->_event);

                WiFi.begin(_ssid.c_str(), _pass.c_str());
                if (testWiFi()) {
                    this->_listenForClient = false;
                    this->_event = WIFI_CONNECTED;
                    this->_meeoEventHandler(this->_event);

                    pubSubClient.setServer(mqttServer.c_str(), mqttPort);
                    if (pubSubClient.connect(_macAddress.c_str(), this->_nameSpace.c_str(), this->_accessKey.c_str())) {
                        this->_event = MQ_CONNECTED;
                        this->_meeoEventHandler(this->_event);
                    } else {
                        if (pubSubClient.state() == 4) {
                            this->_event = MQ_BAD_CREDENTIALS;
                            this->_meeoEventHandler(this->_event);
                        }
                    }
                } else {
                    this->_event = WIFI_DISCONNECTED;
                    this->_meeoEventHandler(this->_event);
                    setupAP();
                }
            }
        }
    }

    String MeeoCore::urlDecode(String str) {
        String encodedString = "";
        char c, code0, code1;
        for (int i =0; i < str.length(); i++) {
            c = str.charAt(i);
            if (c == '+') {
                encodedString += ' ';
            } else if (c == '%') {
                i++;
                code0 = str.charAt(i);
                i++;
                code1 = str.charAt(i);
                c = (h2int(code0) << 4) | h2int(code1);
                encodedString += c;
            } else {
                encodedString += c;
            }
        }
        return encodedString;
    }

    unsigned char MeeoCore::h2int(char c) {
        if (c >= '0' && c <='9'){
            return ((unsigned char)c - '0');
        }
        if (c >= 'a' && c <='f'){
            return ((unsigned char)c - 'a' + 10);
        }
        if (c >= 'A' && c <='F'){
            return ((unsigned char)c - 'A' + 10);
        }
        return(0);
    }
#endif

//Utility Methods

String MeeoCore::convertToString(char * message) {
    return String(message);
}

String MeeoCore::convertToString(byte * message, unsigned int length) {
    String output = "";

    for (int i = 0; i < length; i++) {
        output += (char)message[i];
    }

    return output;
}

void MeeoCore::convertStringToRGB(String payload, int * r, int * g, int * b) {
    *r = atoi(strtok(&payload[0], ","));
    *g = atoi(strtok(NULL, ","));
    *b = atoi(strtok(NULL, ","));
}

boolean MeeoCore::isChannelMatched(String rawTopic, String channel) {
    String temp = String(this->_nameSpace) + "/" + channel;
    return rawTopic.equalsIgnoreCase(temp);
}
