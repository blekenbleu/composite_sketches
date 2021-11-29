#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#include <ipMIDI.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
IPMIDI_CREATE_INSTANCE(WiFiUDP, MIDI, 21928);

unsigned long t1 = millis();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Booting");

  Serial.write("Getting IP address...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("WiFi connected @ IP address =");
  Serial.println(WiFi.localIP());

  // Listen for MIDI messages on channel 1
  MIDI.begin(1);

  MIDI.setHandleNoteOn(OnMidiNoteOn);
  MIDI.setHandleNoteOff(OnMidiNoteOff);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{
  static bool first = true;
  if (first) {
    Serial.print("MIDI.read()\n");
    first = false;
  }
  MIDI.read();

  if ((millis() - t1) > 400)
  {
    t1 = millis();
    static byte oldnote = 64;
    byte note = random(17, 106);
    byte velocity = 65;
    byte channel = 1;
    Serial.print("MIDI.sendNoteOn,OFF\n");
    MIDI.sendNoteOn(note, velocity, channel);
    MIDI.sendNoteOff(oldnote, velocity, channel);
    oldnote = note;
  }
}

// ====================================================================================
// Event handlers for incoming MIDI messages
// ====================================================================================

// -----------------------------------------------------------------------------
// received note on
// -----------------------------------------------------------------------------
void OnMidiNoteOn(byte channel, byte note, byte velocity) {
  Serial.print("Incoming NoteOn  from channel: ");
  Serial.print(channel);
  Serial.print(", note: ");
  Serial.print(note);
  Serial.print(", velocity: ");
  Serial.println(velocity);
}


// -----------------------------------------------------------------------------
// received note off
// -----------------------------------------------------------------------------
void OnMidiNoteOff(byte channel, byte note, byte velocity) {
  Serial.print("Incoming NoteOff from channel: ");
  Serial.print(channel);
  Serial.print(", note: ");
  Serial.print(note);
  Serial.print(", velocity: ");
  Serial.println(velocity);
}
