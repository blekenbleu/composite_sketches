/*
 ; Sample MIDI sketch
 ; author: chegewara
 */
#include "midiusb.h"
#include "song.h"

MIDIusb midi;

void setup() {
  Serial.begin(115200);
  char name[] = "midi.ino";
  midi.begin(name);
  delay(1000);
#if 0
  if(0 && midi.setSong(song, song_len)) {
    Serial.write("midi.setSong() true\n");
    midi.playSong();
    Serial.write("midi.playSong() ended\n");
  } else Serial.write("midi.setSong() false\n");
#else
  Serial.write("MIDIusb begun\n");
#endif
}

// Sample melody as an array of note values
uint8_t note_sequence[] =
{
  74,78,81,86,90,93,98,102,57,61,66,69,73,78,81,85,88,92,97,100,97,
  92,88,85,81,78,74,69,66,62,57,62,66,69,74,78,81,86,90,93,97,102,
  97,93,90,85,81,78,73,68,64,61,56,61,64,68,74,78,81,86,90,93,98,102
};

void midi_task(void)
{
  static uint32_t start_ms = 0;
  static uint32_t note_pos = sizeof(note_sequence);

  // send note every 1000 ms
  if (millis() - start_ms >= 286) {
    start_ms += 286;

    // Send Note Off for previous note.
    midi.noteOFF(note_sequence[note_pos]);
    if (sizeof(note_sequence) == note_pos) {
      Serial.write("running midi_task();\n");
      note_pos = 0;
    }

    // Send Note On for current position at full velocity (127) on channel 1.
    midi.noteON(note_sequence[note_pos++], 127);
  }
}

void loop() {
  midi_task();
}
