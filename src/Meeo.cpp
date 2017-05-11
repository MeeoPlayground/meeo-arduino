#include "Meeo.h"

const char * mqttServer = "mq.meeo.io";
MeeoCore Meeo;
WiFiServer server(80);
WiFiClient espClient;
PubSubClient pubSubClient(espClient);

#ifdef DEBUG_MEEO
    template <typename Generic>
    void meeo_log(Generic text) {
        Serial.print("Meeo: ");
        Serial.println(text);
    }
#else
    #define meeo_log(l);
#endif

void MeeoCore::begin(const char * nameSpace, const char * accessKey) {
    beginMeeo(nameSpace, accessKey, "", "");
}

void MeeoCore::begin(const char * nameSpace, const char * accessKey, const char * ssid, const char * pass) {
    meeo_log(F("SSID and password were provided"));
    beginMeeo(nameSpace, accessKey, "", "");
}

void MeeoCore::run() {
    if (!pubSubClient.connected()) {
        this->_event = MQ_DISCONNECTED;
        this->meeoEventHandler(this->_event);
        if (pubSubClient.connect(this->_macAddress, this->_nameSpace, this->_accessKey)) {
            this->_event = MQ_CONNECTED;
            this->meeoEventHandler(this->_event);
        }
    }
    pubSubClient.loop();
}

void MeeoCore::setEventHandler(void (* f)(MeeoEvent)) {
    this->meeoEventHandler = f;
}

void MeeoCore::setDataReceivedHandler(void (* handler)(char *, uint8_t *, unsigned int)); {
    pubSubClient.setCallback(handler);
}

boolean MeeoCore::publish(const char * topic, const char * payload, boolean retained) {
    int newTopicLength = strlen(this->_nameSpace) + strlen(topic) + 2;
    char newTopic[newTopicLength];
    snprintf(newTopic, newTopicLength, "%s/%s", this->_nameSpace, topic);
    meeo_log(newTopic);

    return pubSubClient.publish(newTopic, payload, retained);
}

boolean MeeoCore::subscribe(const char * topic, uint8_t qos, boolean asMqttTopic) {
    if (asMqttTopic) {
        return pubSubClient.subscribe(topic, qos);
    } else {
        int newTopicLength = strlen(this->_nameSpace) + strlen(topic) + 2;
        char newTopic[newTopicLength];
        snprintf(newTopic, newTopicLength, "%s/%s", this->_nameSpace, topic);
        meeo_log(newTopic);

        return pubSubClient.subscribe(newTopic, qos);
    }
}

boolean MeeoCore::unsubscribe(const char * topic) {
    int newTopicLength = strlen(this->_nameSpace) + strlen(topic) + 2;
    char newTopic[newTopicLength];
    snprintf(newTopic, newTopicLength, "%s/%s", this->_nameSpace, topic);

    return pubSubClient.unsubscribe(newTopic);
}

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

boolean MeeoCore::isChannelMatched(String MeeoTopic, String topic) {
    String temp = String(this->_nameSpace) + "/" + topic;
    return MeeoTopic.equalsIgnoreCase(temp);
}

//Private Methods

void MeeoCore::beginMeeo(const char * nameSpace, const char * accessKey, String ssid, String pass) {
    this->_nameSpace = nameSpace;
    this->_accessKey = accessKey;
    this->_ssid = ssid;
    this->_pass = pass;

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
    String AP_NameString = "ESP8266-" + macID;
    this->_macAddress = AP_NameString.c_str();

    this->_event = WIFI_CONNECTING;
    this->meeoEventHandler(this->_event);

    WiFi.begin(this->_ssid, this->_pass);
    if (!testWiFi()) {
        meeo_log(F("Was not able to connect"));
        server.begin();
        this->_listenForClient = true;
        this->_event = WIFI_DISCONNECTED;
        this->meeoEventHandler(this->_event);
        setupAP();
    } else {
        meeo_log(F("Connected!"));
        this->_event = WIFI_CONNECTED;
        this->meeoEventHandler(this->_event);

        pubSubClient.setServer(mqttServer, 1883);
        if (pubSubClient.connect(this->_macAddress, this->_nameSpace, this->_accessKey)) {
            this->_event = MQ_CONNECTED;
            this->meeoEventHandler(this->_event);
        } else {
            if (pubSubClient.state() == 4) {
                this->_event = MQ_BAD_CREDENTIALS;
                this->meeoEventHandler(this->_event);
            }
        }
    }
}

void MeeoCore::setupAP() {
    meeo_log(F("Set to AP mode"));

    WiFi.mode(WIFI_AP);
    WiFi.softAP(this->_macAddress);

    this->_event = AP_MODE;
    this->meeoEventHandler(this->_event);

    getWiFiCredentials();
}

boolean MeeoCore::testWiFi() {
    uint8_t c = 0;

    meeo_log(F("Connecting..."));

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
            meeo_log(tempssid); meeo_log(temppass);
            this->_ssid = tempssid.c_str();
            this->_pass = temppass.c_str();

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json");
            client.println("Connection: close");
            client.println();

            if (this->_ssid.equals("")) {
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
            client.print(this->_ssid);
            client.print("\", \"pass\":\"");
            client.print(this->_pass);
            client.print("\"}");
            client.flush();
        }

        if (connectIndex != -1) {
            WiFi.mode(WIFI_STA);

            this->_event = WIFI_CONNECTING;
            this->meeoEventHandler(this->_event);

            WiFi.begin(this->_ssid.c_str(), this->_pass.c_str());
            if (testWiFi()) {
                this->_listenForClient = false;
                meeo_log(F("Connection success!"));
                this->_event = WIFI_CONNECTED;
                this->meeoEventHandler(this->_event);

                pubSubClient.setServer(mqttServer, 1883);
                if (pubSubClient.connect(this->_macAddress, this->_username, this->_accessKey)) {
                    this->_event = MQ_CONNECTED;
                    this->meeoEventHandler(this->_event);
                } else {
                    if (pubSubClient.state() == 4) {
                        this->_event = MQ_BAD_CREDENTIALS;
                        this->meeoEventHandler(this->_event);
                    }
                }
            } else {
                meeo_log(F("Connection failed"));
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
