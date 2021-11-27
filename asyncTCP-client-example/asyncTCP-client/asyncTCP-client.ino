/* Works with Packet Sender
 ; https://packetsender.com/
 ; File > Settings > Smart Responses
 ; set multiple tests for Packet data equals "hello" with no quotes;
 ; Enable a response of "goodbye" (no quotes) to stop;
 ; enable any other response to loop.
 ; 
 */
#include "WiFi.h"
#include "AsyncTCP.h"
#include "DNSServer.h"

#define SSID "Actiontec-8318-2.4G"
#define PASSWORD "e3d9dd3c"

#define SERVER_HOST_NAME "esp-tcp-client"

#define TCP_SERVER_IP "192.168.1.144"
#define TCP_SERVER_PORT 50024
#define DNS_PORT 53

bool ping = true, pong = false;

static DNSServer DNS;

void replyToServer(void *arg)
{
  AsyncClient *client = reinterpret_cast<AsyncClient *>(arg);
  String jsonString = "hello"; // string test

  // send reply
  if (client->space() > strlen(jsonString.c_str()) && client->canSend()) {
    client->add(jsonString.c_str(), strlen(jsonString.c_str()));
    client->send();
  }
}

void handleData(void *arg, AsyncClient *client, void *data, size_t len)
{
  Serial.printf(".  Data received from %s :", client->remoteIP().toString().c_str());
  Serial.write((uint8_t *)data, len);
  Serial.write("\n");
  pong = true;
  if (0 != strncmp("goodbye", (char *)data, 7))
    ping = true;
}

void onConnect(void *arg, AsyncClient *client)
{
  Serial.printf("Client has connected to server %s port %d \nsending a reply...", TCP_SERVER_IP, TCP_SERVER_PORT);
  replyToServer(client);
}

AsyncClient *client_tcp = new AsyncClient;

void setup()
{
  Serial.begin(115200);

  // connect to access point
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

  client_tcp->onData(handleData, client_tcp);
  client_tcp->onConnect(onConnect, client_tcp);

  // start dns server
  if (!DNS.start(DNS_PORT, SERVER_HOST_NAME, WiFi.softAPIP()))
    Serial.printf("\n DNS service start failed\n");
}

void loop()
{
  DNS.processNextRequest();
  if (pong) {
    Serial.write("Closing..");
    client_tcp->close();
    Serial.write(".  closed.\n");
    pong = false;
    delay(2000);
  }
  if (ping) {
    Serial.write("Connecting...");
    client_tcp->connect(TCP_SERVER_IP, TCP_SERVER_PORT);
    Serial.write(".  connected.");
    ping = false;
  }
}
