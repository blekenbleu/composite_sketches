/* works with Packet Sender 7.2.3
 ; https://packetsender.com/documentation
 ; 
 */
#include "WiFi.h"
#include "AsyncUDP.h"

const char * ssid = "Actiontec-8318-2.4G";
const char * password = "e3d9dd3c";
uint16_t port = 51352;			// change to value at bottom of Packet Sender
AsyncUDP udp;
bool ping = true;

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.write("WiFi.begin(Actiontec-8318-2.4G)\n");
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.write("WiFi Failed\n");
      while(1) {
          delay(1000);
      }
  }
  else {
    Serial.write("WiFi connected as ");
    Serial.println(WiFi.localIP());
    if(udp.connect(IPAddress(192,168,1,144), port)) {
        
        Serial.println("UDP connected");
        udp.onPacket([](AsyncUDPPacket packet) {           
            Serial.write(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
            Serial.write(" UDP Packet from: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", To: ");
            Serial.print(packet.localIP());
            Serial.print(":");
            Serial.print(packet.localPort());
            Serial.print(", Length: ");
            Serial.print(packet.length());
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
            //reply to the packet; gets received by Packet Sender
            packet.printf("AsyncUDPClient.ino got %u bytes of data", packet.length());
            ping = true;
        });
        delay(1000);   // Even with delay, Packet Sender did not detect this:
        // Send unicast
        Serial.write("sending UDP unicast Anyone there\n");
        udp.print("Anyone there?");
    }
  }
}

void loop()
{
  if(ping) {
    delay(500);
    //Send broadcast
    Serial.write("sending UDP broadcast hello\n");
    udp.broadcastTo("wassup anyone?", port);
    delay(500);
    Serial.write("sending UDP unicast Hello\n");
    udp.print("Hello Server!");
    ping = false;
  }
}
