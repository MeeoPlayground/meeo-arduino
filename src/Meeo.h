#ifndef Meeo_h
#define Meeo_h

#define DEBUG_MEEO

#ifndef ESP8266
#error This code is intended to run on the ESP8266 platform! Please check your Tools->Board setting.
#endif

#include <Arduino.h>
#include <ESP8266WiFi.h>
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
        void begin(const char * nameSpace, const char * accessKey);
        void begin(const char * nameSpace, const char * accessKey, String ssid, String pass);
        void run();
        void setEventHandler(void (* f)(MeeoEvent));
        void setDataReceivedHandler(void (* handler)(char *, uint8_t *, unsigned int));
        boolean publish(const char * topic, const char * payload, boolean retained = true);
        boolean subscribe(const char * topic, uint8_t qos = 0, boolean asMqttTopic = false);
        boolean unsubscribe(const char * topic);

        String toString(char * message);
        String toString(byte * message, unsigned int length);
        void stringToRGB(String payload, int * r, int * g, int * b);
        boolean isTopicEqual(String uttrTopic, String topic);
    private:
        const char * _nameSpace;
        const char * _accessKey;
        const char * _ssid;
        const char * _pass;
        const char * _macAddress;
        boolean _listenForClient = false;

        void beginMeeo(const char * nameSpace, const char * accessKey, const char * ssid, const char * pass);
        void setupAP();
        boolean testWiFi();
        void getWiFiCredentials();
        String urlDecode(String str);
        unsigned char h2int(char c);

        MeeoEvent _event;
        void (* meeoEventHandler)(MeeoEvent);
};

extern MeeoCore Meeo;

#endif
