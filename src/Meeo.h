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
#ifndef Meeo_h
#define Meeo_h

// Uncomment this if you don't want to see
// runtime logs from the library
#define DEBUG_MEEO



#include <Arduino.h>

#ifdef ESP8266
    #include <ESP8266WiFi.h>
#elif defined __AVR
    #include <Client.h>
#else
    #error Unsupported Board
#endif

#include <PubSubClient.h>
#include "Print.h"

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

class MeeoCore : public Print {
    public:
        #ifdef ESP8266
            void begin(String nameSpace, String accessKey, String ssid = "", String pass = "");
        #elif defined __AVR
            void begin(String nameSpace, String accessKey, Client & client);
        #else
            #error Unsupported Board
        #endif
        void run();
        void setEventHandler(void (* f)(MeeoEventType));
        void setDataReceivedHandler(void (* f)(String, String));
        boolean publish(String channel, String payload, boolean asMqttTopic = false, boolean retained = true);
        boolean subscribe(String channel, uint8_t qos = 0, boolean asMqttTopic = false);
        boolean unsubscribe(String channel, boolean asMqttTopic = false);

        String convertToString(char * message);
        String convertToString(byte * message, unsigned int length);
        void convertStringToRGB(String payload, int * r, int * g, int * b);
        boolean isChannelMatched(String rawTopic, String channel);

        void setLoggerChannel(String channel);
        size_t write(const uint8_t *buffer, size_t size);

        // Meeo does not support this function
        size_t write(uint8_t);
    private:
        String _nameSpace;
        String _accessKey;
        boolean _listenForClient = false;
        String _loggerChannel;

        #ifdef ESP8266
            void beginMeeo(String nameSpace, String accessKey, String ssid, String pass);
            void setupAP();
            boolean testWiFi();
            void getWiFiCredentials();
            String urlDecode(String str);
            unsigned char h2int(char c);
        #elif defined __AVR
            void beginMeeo(String nameSpace, String accessKey, Client & client);
        #else
            #error Unsupported Board
        #endif

        MeeoEventType _event;
        void (* _meeoEventHandler)(MeeoEventType);
};

extern MeeoCore Meeo;

#endif
