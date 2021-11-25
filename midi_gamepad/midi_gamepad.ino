/*
 ; Sample MIDI sketch
 ; author: chegewara
 */
#include "midiusb.h"
#include "song.h"
#if 1
#define DG 1
#include "hidgamepad.h"

HIDgamepad gamepad;
#endif

MIDIusb midi;

void setup() {
  Serial.begin(115200);
  char name[] = "midi.ino";
  midi.begin(name);
#ifdef DG
  gamepad.begin();
  Serial.write("HIDgamepad begun\n");
#endif
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
  static uint32_t note_pos = sizeof(note_sequence);

  // Send Note Off for previous note.
  midi.noteOFF(note_sequence[note_pos]);
  if (sizeof(note_sequence) == note_pos) {
    Serial.write("running midi_task();\n");
    note_pos = 0;
  }

  // Send Note On for current position at full velocity (127) on channel 1.
  midi.noteON(note_sequence[note_pos++], 127);
}

#ifdef DG
void gamepad_task()
{
  static uint8_t i = 255;
  uint32_t buttons = i;

  buttons |= (buttons << 8);
  buttons |= (buttons << 16);
  gamepad.sendAll(buttons, i, i, i, i, i, i, i%10);
  if (255 <= i) {
    i = 0;
    Serial.write("gamepad_task() loop\n");
  }
  else i++;
}
#endif

void loop() {
  static uint32_t start_ms = 0;

  // once in awhile
  if (millis() - start_ms >= 286) {
    start_ms += 286;

    midi_task();
#ifdef DG
    gamepad_task();
#endif
  } else delay(100);
}
