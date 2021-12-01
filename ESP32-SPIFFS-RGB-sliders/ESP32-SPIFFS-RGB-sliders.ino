#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include "esp32s2LED.h" 
#include <Arduino_JSON.h>

CREATE_ESP32_WS2812_INSTANCE();

AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // Create a WebSocket object

String message = "";
String sliderValue[] = {"0", "0", "0"};

String getSliderValues(){
  JSONVar Jvalues;
  
  Jvalues["sliderValue1"] = String(sliderValue[0]);
  Jvalues["sliderValue2"] = String(sliderValue[1]);
  Jvalues["sliderValue3"] = String(sliderValue[2]);
  return JSON.stringify(Jvalues);
}

int wsrgb[] = {0, 0, 0};
uint8_t last;   // preserve last byte sent
void setSliderValue(uint8_t i)
{
  static bool first = true;

  sliderValue[i] = message.substring(2) + String((char)last);
  wsrgb[i] = map(sliderValue[i].toInt(), 0, 100, 0, 10);
  ws.textAll(getSliderValues());
  if (first) {
    first = false;

    Serial.printf("setSliderValue(): %s%c\n", message, (char)last);
    Serial.printf("sliderValue[%d] = ", i);
    Serial.printf("%s mapped to %d for WS2812\n", sliderValue[i], wsrgb[i]);
    Serial.print(getSliderValues());
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len
   && info->opcode == WS_TEXT) {
    last = data[len - 1]; // valid data
    data[len - 1] = 0;    // string termination
    message = (char*)data;
    if (message.indexOf("1s") >= 0)
      setSliderValue(0);
    else if (message.indexOf("2s") >= 0)
      setSliderValue(1);
    else if (message.indexOf("3s") >= 0)
      setSliderValue(2);
    else if (strcmp((char*)data, "getValues") == 0)
      ws.textAll(getSliderValues());
  }
}

void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  static bool first = true;
  char wsc[] = "WebSocket client ";

  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("%s#%u connected from %s\n", wsc,
                    client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("%s#%u disconnected\n", wsc, client->id());
      break;
    case WS_EVT_DATA:
      if (first) {
        Serial.printf("%sdata length %d\n", wsc, len);
        first = false;
      }
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
      Serial.printf("%sping\n", wsc);
      break;
    case WS_EVT_ERROR:
      Serial.printf("%sERROR\n", wsc);
      break;
    default:
      Serial.printf("%sunknown event type\n", wsc);
      break;
  }
}

void onRequest(AsyncWebServerRequest *request) {
  request->send(404); //Handle Unknown Request
}

void setup() {
  ESP32_WS2812_SETUP(2);      // WS2812 red

  Serial.begin(115200);
  Serial.write("ESP32-SPIFFS-PWM-SLIDER setup()\n");
  ESP32_LED(0,2,0);

  if (SPIFFS.begin())
    Serial.write("SPIFFS mounted successfully");
  else {
    Serial.write("SPIFFS mount failed; formatting...\nmount ");
    Serial.write(SPIFFS.begin(true) ? "succeeded" : "failed");
    Serial.write(" after formatting");
  }
  ESP32_LED(0,0,2);

  WiFi.mode(WIFI_STA);
  Serial.write("\nWiFi.begin()\n");
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  Serial.write("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.write('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  ESP32_LED(0,2,2);

  Serial.write("initWebSocket()\n");
  ws.onEvent(onWSEvent);
  server.addHandler(&ws);
  ESP32_LED(2,2,0);
  
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.write(" HTTP_GET request");
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  // respond to GET requests on URL /heap
  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  // attach filesystem root at URL /
  server.serveStatic("/", SPIFFS, "/");
  server.onNotFound(onRequest);
  ESP32_LED(1,0,0);

  Serial.write("server.begin()\n");
  server.begin(); // Start server
  ESP32_LED(2,0,2);
}

void loop() {
  static uint32_t start_ms = 0;
  static int RGB[] = {0,0,0};

  if (RGB[0] != wsrgb[0] || RGB[1] != wsrgb[1] || RGB[2] != wsrgb[2]) {
    static bool first = true;

    if (first) {
      Serial.printf("\nloop(): RGB = [%d, %d, %d]\n", wsrgb[0], wsrgb[1], wsrgb[2]);
      first = false;
    }
    ESP32_LED(wsrgb[0], wsrgb[1], wsrgb[2]);
    RGB[0] = wsrgb[0];    
    RGB[1] = wsrgb[1];
    RGB[2] = wsrgb[2];
  }
  if(millis() >= 1000 + start_ms) {
    ws.cleanupClients();
    start_ms += 1000;
  }
}
