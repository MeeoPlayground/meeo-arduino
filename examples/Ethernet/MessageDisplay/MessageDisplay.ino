#include <Meeo.h>
#include <SPI.h>
#include <Ethernet.h>
#include <U8glib.h>

String nameSpace = "my_namespace";
String accessKey = "my_access_key";
String channel = "my-channel";

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient ethClient;

String message;

void draw() {
  u8g_uint_t x, y;
  x = u8g.getWidth();
  x -= u8g.getStrWidth(message.c_str());
  x /= 2;

  y = u8g.getHeight();
  y += 10;
  y /= 2;

  u8g.setFont(u8g_font_unifont_0_8);
  u8g.drawStr( x, y, message.c_str());
}

void setup() {
  Serial.begin(115200);

  Ethernet.begin(mac);

  Meeo.setEventHandler(meeoEventHandler);
  Meeo.setDataReceivedHandler(meeoDataHandler);
  Meeo.begin(nameSpace, accessKey, ethClient);

  u8g.setColorIndex(1);
}

void loop() {
  Meeo.run();

  u8g.firstPage();
  do {
    draw();
  } while( u8g.nextPage() );
}

void meeoDataHandler(String topic, String payload) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  if (Meeo.isChannelMatched(topic, channel)) {
    message = payload;
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
      Meeo.subscribe(channel);
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
