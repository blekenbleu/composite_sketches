#include <ETH.h>
#include <ESPmDNS.h>

static bool eth_connected = false;

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      DBG("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      DBG("ETH Connected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      DBG("ETH MAC:", ETH.macAddress(), "IPv4:", ETH.localIP(), ETH.linkSpeed(), "Mbps");
      if (ETH.fullDuplex())
              DBG("FULL_DUPLEX");
      eth_connected = true;
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      DBG("ETH Disconnected");
      eth_connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      DBG("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

bool orig_ETH_startup()
{
  WiFi.onEvent(WiFiEvent);
  ETH.begin();

  DBG(F("Getting IP address..."));

  while (!eth_connected)
    delay(100);

  return true;
}

bool ETH_startup()
{
  char ssid[] = "Actiontec-8318-2.4G"; //  your network SSID (name)
  char pass[] = "e3d9dd3c";

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DBG("Establishing connection to WiFi..");
  }
  DBG("Connected to network as ", WiFi.localIP());
  ETH.setHostname("esp32-NoteOnOff");
  return true; 
}
