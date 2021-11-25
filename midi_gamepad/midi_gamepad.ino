/*
 ; Sample MIDI + gamepad sketch
 ; author: blekenbleu
 */

#if 0			// MIDI
#define DM 1
#include "midiusb.h"
#include "song.h"
#endif

#if 1			// gamepad
#define DG 1
#include "hidgamepad.h"

HIDgamepad gamepad;
#endif

#ifdef DM
MIDIusb midi;
#endif

void setup() {
  Serial.begin(115200);

#ifdef DM
  char name[] = "midi_gamepad.ino";
  midi.begin(name);
  Serial.write("MIDIusb begun\n");
#endif

#ifdef DG
  gamepad.begin();
  Serial.write("HIDgamepad begun\n");
#endif

#ifdef DM
  Serial.write("delay(1000)");  
  delay(1000);
  if(0 && midi.setSong(song, song_len)) {
    Serial.write("midi.setSong() true\n");
    midi.playSong();
    Serial.write("midi.playSong() ended\n");
  } else Serial.write("midi.setSong() false\n");
#endif

  Serial.write("setup() complete\n");
}

#ifdef DM
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
#endif

#ifdef DG
void gamepad_task()
{
  static uint8_t i = 255;
  static uint32_t count = 0;
  static bool ping = true;

  if (ping)  // 32 buttons, 3 axes, 3 rotations, 8 hat positions
    gamepad.sendAll(1 << (i%32), i, i, i, i, i, i, 1 + i%8);
  else       // axes and rotations are actually 8-bit signed
    gamepad.sendAll(1 << (31 - i%32),255 - i, i, 255 -i, i, 255 - i, i, 8 - i%8);
  if (255 <= i) {
    i = 0;
    ping = !ping;
    Serial.printf("gamepad_task() %ld loops\n", 256 * count++);
  }
  else i++;
}
#endif

void loop() {
  static uint32_t start_ms = 0;

  // once in awhile
  if (millis() - start_ms >= 286) {
    start_ms += 286;

#ifdef DM
    midi_task();
#endif
#ifdef DG
    gamepad_task();
#endif
  } else delay(100);
}
