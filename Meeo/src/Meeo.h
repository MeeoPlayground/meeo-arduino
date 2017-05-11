#ifndef Meeo_h
#define Meeo_h

#define DEBUG_MEEO

#include <Arduino.h>

#ifdef ESP8266
    #include <ESP8266WiFi.h>
#elif defined __AVR
    #include <Client.h>
#endif

#include <PubSubClient.h>

typedef enum meeoEventType {
    WIFI_CONNECTING = 0,
    WIFI_CONNECTED,
    WIFI_DISCONNECTED,
    MQ_CONNECTED,
    MQ_DISCONNECTED,
    MQ_BAD_CREDENTIALS,
    MQ_ERROR,
    AP_MODE,
} MeeoEventType;

class MeeoCore {
    public:
        #ifdef ESP8266
            void begin(String nameSpace, String accessKey, String ssid = "", String pass = "");
        #elif defined __AVR
            void begin(String nameSpace, String accessKey, Client & client);
        #endif
        void run();
        void setEventHandler(void (* f)(MeeoEventType));
        void setDataReceivedHandler(void (* f)(String, String));
        boolean publish(String topic, String payload, boolean retained = true, boolean asMqttTopic = false);
        boolean subscribe(String topic, uint8_t qos = 0, boolean asMqttTopic = false);
        boolean unsubscribe(String topic, boolean asMqttTopic = false);

        String convertToString(char * message);
        String convertToString(byte * message, unsigned int length);
        void convertStringToRGB(String payload, int * r, int * g, int * b);
        boolean isChannelMatched(String uttrTopic, String topic);
    private:
        String _nameSpace;
        String _accessKey;
        boolean _listenForClient = false;

        #ifdef ESP8266
            void beginMeeo(String nameSpace, String accessKey, String ssid, String pass);
            void setupAP();
            boolean testWiFi();
            void getWiFiCredentials();
            String urlDecode(String str);
            unsigned char h2int(char c);
        #elif defined __AVR
            void beginMeeo(String nameSpace, String accessKey, Client & client);
        #endif

        void _callbackHandler(char * topic, uint8_t * payload, unsigned int payloadLength);
        void (* _dataReceivedHandler)(String, String);

        MeeoEventType _event;
        void (* _meeoEventHandler)(MeeoEventType);
};

extern MeeoCore Meeo;

#endif
