// http://arduino-er.blogspot.com/2021/04/esp32-s2arduino-esp32-s2-saola-1-web.html
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "esp32s2LED.h"

CREATE_ESP32_WS2812_INSTANCE();

AsyncWebServer server(80);

const char* Param_COLOR ="color";
const char* Param_ID = "id";
unsigned int count = 0;
int r = 5, g = 2, b = 8;

#define RMT_TRACE 0
#if RMT_TRACE
void testWS2812(u8 r, u8 g, u8 b)
{
  u32 color = r << 16 | g << 8 | b;
  rmt_data_t led_data[24];
  int bit;
  static u32 old_color = 0;
  if (old_color == color)
    return;
  old_color = color;
  Serial.write("testWS2812(");
  Serial.print(color, HEX);
  Serial.write(");\n");

  for (bit = 0; bit < 24; bit++) {
    if (color & (1 << (23 - bit))) {
      led_data[bit].level0 = 1;
      led_data[bit].duration0 = 8;
      led_data[bit].level1 = 0;
      led_data[bit].duration1 = 4;
    } else {
      led_data[bit].level0 = 1;
      led_data[bit].duration0 = 4;
      led_data[bit].level1 = 0;
      led_data[bit].duration1 = 8;
    }
    Serial.write("rmt_data_t led_data[");
    Serial.print(bit);
    Serial.write(") = ");
    Serial.println(led_data[bit].val, HEX);
  }
  ESP32_LED(r,g,b);
}

#define TEST_LED(r,g,b) testWS2812(r,g,b);

#else
#define TEST_LED(r,g,b) ESP32_LED(r,g,b)
#endif // RMT_TRACE

// HTML: rename to range.htm to test in browsers
#include "range.h"	// const char index_html[]

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  ESP32_WS2812_SETUP(10);      // WS2812 red

  Serial.begin(115200);
  Serial.write("--- ESP32_WebRGB_ColorPicker Start---\n"
               "Chip Model: ");
  Serial.print(ESP.getChipModel());
  Serial.print("\nChip Revision: ");
  Serial.println(ESP.getChipRevision());
  
  //Act as STA, connect to WiFi network
  WiFi.mode(WIFI_STA);
  Serial.write("\nWiFi.begin()\n");
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.write("WiFi Failed!\n");
    return;
  }
  Serial.write("\nVisit IP Address: ");
  Serial.println(WiFi.localIP());
  ESP32_LED(0,10,0);
  
  // Send HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
    Serial.write("index_html sent\n");
  });

  // Send a GET request to <ESP_IP>/get?color=rr -or- gg -or- bb
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String color_value, id_value;
    bool change = request->hasParam(Param_COLOR); // if parameter "color" found
    
    if (change) {
      Serial.write("param_COLOR received:  ");
      color_value = request->getParam(Param_COLOR)->value();
      Serial.println(color_value);
      // color_value.remove(0, 1);  // remove '#' ??
      if (request->hasParam(Param_ID)) {
        Serial.write("param_ID received:  ");
        id_value = request->getParam(Param_ID)->value();
        char c[id_value.length() + 1];
        id_value.toCharArray(c, id_value.length() + 1);
        Serial.println(c);
        //Convert intMessage to int
        int intColor = stol(color_value);
        if ('0' == c[1])
          r = intColor;
        else if ('1' == c[1])
          g = intColor;
        else if ('2' == c[1])
          b = intColor;
        else {
          change = false;
          Serial.write("param_COLOR ignored for Param_ID = ");
          Serial.println(c);
        }
        if (change) {
          Serial.write("Setting ESP32_LED(");
          Serial.print(r);
          Serial.write(",");
          Serial.print(g);
          Serial.write(",");
          Serial.print(b);
          Serial.write(")\n");

          //set NeoPixel color
          ESP32_LED(r,g,b);
        }
        // tell loop to cool it for awhile
        count = 0;
      }
      else Serial.write("param_ID NOT received; ignoring param_COLOR\n");
    }
    else Serial.write("param_COLOR NOT found\n");
  });        // server.on("/get"...)
  server.onNotFound(notFound);
  server.begin();
}

// intMessage to long
long stol(String recv){
  char c[recv.length() + 1];
  recv.toCharArray(c, recv.length() + 1);
  return strtol(c, NULL, 10); 
}

void loop() {
  if(count > (10+r+g+b))
    count =  11;	// loop colors here
  if (count > (10+r+g))
    TEST_LED(0, 0, count-(10+r+g));   // blue is dimmer
  else if (count > 10+r)
    TEST_LED(0, count-(10+r), 0);     // plenty bright
  else if (count > 10)
    TEST_LED(count - 10, 0, 0);
  else delay(800);	     // hold web colors longer
  delay(300);
  count++;
}
