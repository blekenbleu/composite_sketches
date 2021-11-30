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

//Json Variable to Hold Slider Values
JSONVar sliderValues;

int wsrgb[] = {0, 0, 0};

//Get Slider Values
String getSliderValues(){
  sliderValues["sliderValue1"] = String(sliderValue[0]);
  sliderValues["sliderValue2"] = String(sliderValue[1]);
  sliderValues["sliderValue3"] = String(sliderValue[2]);

  String jsonString = JSON.stringify(sliderValues);
  return jsonString;
}

void notifyClients(String sliderValues) {
  ws.textAll(sliderValues);
}

void setSliderValue(uint8_t i)
{
  sliderValue[i] = message.substring(2);
  wsrgb[i] = map(sliderValue[i].toInt(), 0, 100, 0, 15);
  Serial.println(wsrgb[i]);
  Serial.print(getSliderValues());
  notifyClients(getSliderValues());
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len
   && info->opcode == WS_TEXT) {
    data[len - 1] = 0;
    message = (char*)data;
    if (message.indexOf("1s") >= 0)
      setSliderValue(0);
    if (message.indexOf("2s") >= 0)
      setSliderValue(1);
    if (message.indexOf("3s") >= 0)
      setSliderValue(2);
    if (strcmp((char*)data, "getValues") == 0)
      notifyClients(getSliderValues());
  }
}

void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  Serial.write("WebSocket client ");
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("#%u connected from %s\n",
                    client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("#%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      Serial.printf("data length %d\n", len);
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
      Serial.write("ping\n");
      break;
    case WS_EVT_ERROR:
      Serial.write("ERROR\n");
      break;
    default:
      Serial.write("unknown event type\n");
      break;
  }
}

void onRequest(AsyncWebServerRequest *request) {
  request->send(404); //Handle Unknown Request
}

void setup() {
  ESP32_WS2812_SETUP(5);      // WS2812 red

  Serial.begin(115200);
  Serial.write("ESP32-SPIFFS-PWM-SLIDER setup()\n");
  ESP32_LED(0,5,0);

  if (SPIFFS.begin())
    Serial.write("SPIFFS mounted successfully");
  else {
    Serial.write("SPIFFS mount failed; formatting...\nmount ");
    Serial.write(SPIFFS.begin(true) ? "succeeded" : "failed");
    Serial.write(" after formatting");
  }
  ESP32_LED(0,0,5);

  WiFi.mode(WIFI_STA);
  Serial.write("\nWiFi.begin()\n");
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  Serial.write("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.write('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  ESP32_LED(0,5,5);

  Serial.write("initWebSocket()\n");
  ws.onEvent(onWSEvent);
  server.addHandler(&ws);
  ESP32_LED(5,5,0);
  
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
  ESP32_LED(5,0,5);
}

void loop() {
  static bool first = true;
  static uint32_t start_ms = 0;
  static int RGB[] = {1,1,1};

  if (RGB[0] != wsrgb[0] || RGB[1] != wsrgb[1] || RGB[2] != wsrgb[2]) {
    if (0 == start_ms++)
      Serial.write("loop()-ing:  ");
    ESP32_LED(wsrgb[0], wsrgb[1], wsrgb[2]);
    Serial.printf("RGB = [%d, %d, %d]\n", wsrgb[0], wsrgb[1], wsrgb[2]);
    RGB[0] = wsrgb[0];    
    RGB[1] = wsrgb[1];
    RGB[2] = wsrgb[2];
  }
  if(millis() >= 1000 + start_ms) {
    if (1000 > start_ms)
      Serial.write("first ws.cleanupClients()\n");
    ws.cleanupClients();
    start_ms += 1000;
  }
}
