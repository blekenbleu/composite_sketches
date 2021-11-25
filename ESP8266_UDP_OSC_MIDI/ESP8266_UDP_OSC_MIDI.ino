// https://little-scale.blogspot.com/2017/11/midi-data-over-udp-via-osc-using-esp8266.html
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
WiFiUDP Udp;

const char* ssid = "name"; // this is the WIFI network name
const char* password = "password"; // this is the WIFI network password
byte target_ip[4] = {  1, 1, 1, 1 }; // this is the IP address of the target computer e.g. 192.168.1.1
int target_port = 7400; // this is the UDP port
byte midi_message[3];
int current;
int previous;

void setup()
{
  pinMode(4, INPUT_PULLUP); // a button is connected between this pin and ground
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  { delay(500);
  }
  Udp.begin(target_port);
}

void loop()
{
  current = digitalRead(4);
  if (current != previous) { // test for change in button state
    previous = current;
    digitalWrite(LED_BUILTIN, current); // turn LED on or off depending on button state
    sendNoteOSC(0, 60, (1 - current) * 127);
    delay(3);
  }
}

void sendNoteOSC(byte channel, byte pitch, byte velocity) {
  sendMIDIOSC(144 + channel, pitch, velocity);
}

void sendMIDIOSC(byte MIDI_byte_1, byte MIDI_byte_2, byte MIDI_byte_3) {
  OSCMessage msg("/MIDI");
  msg.add((int) MIDI_byte_1);
  msg.add((int) MIDI_byte_2);
  msg.add((int) MIDI_byte_3);

  Udp.beginPacket(target_ip, target_port);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}
