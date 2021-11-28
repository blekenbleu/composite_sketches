#include <WiFi.h>
#include <ESPmDNS.h>

static bool wifi_connected = false;

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_START:
      DBG("WiFi Started");
      //set wifi hostname here
      WiFi.setHostname("esp32-wifi");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      DBG("WiFi Connected");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      DBG("WiFi MAC:", WiFi.macAddress(), "IPv4:", WiFi.localIP());
      wifi_connected = true;
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      DBG("WiFi Disconnected");
      wifi_connected = false;
      break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
      DBG("WiFi Stopped");
      wifi_connected = false;
      break;
    default:
      break;
  }
}

bool WiFi_startup()
{
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DBG("Establishing connection to WiFi..");
  }
  DBG("Connected to network as ", WiFi.localIP());
  WiFi.setHostname("esp32-NoteOnOff");
  return true; 
}
