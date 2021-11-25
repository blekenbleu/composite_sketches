/*
 * This is composite example
 * Devices using available S2 endpoints
 * HID (keyboard or gamepad), MIDI (problematic), CDC serial and CP210x Serial
 * 
 * author: chegewara; Hacked by blekenbleu
 */

#include "Arduino.h"

#define DC 1
#ifdef DC
#include "cdcusb.h"
#define DCB 1
#endif

#ifdef DK			// mutually exclusive with DB (only 1 HID device)
#define DHCB 1			// for MyHIDCallbacks
#include "hidkeyboard.h"
#define DCB 1
#endif

//#define DM 1			// MIDI does NOT play well with others
#ifdef DM
#include "midiusb.h"
#endif

#define DG 1
#ifdef DG
#undef DK
#define DHCB 1			// for MyHIDCallbacks
#include "hidgamepad.h"
#endif

#ifdef DK
HIDkeyboard keyboard;
#endif

#ifdef DM
MIDIusb midi;
#endif

#ifdef DG
HIDgamepad gamepad;
#endif

#ifdef DC
CDCusb USBSerial;

class MyCDCCallbacks : public CDCCallbacks {
    void onCodingChange(cdc_line_coding_t const* p_line_coding)
    {
        int bitrate = USBSerial.getBitrate();
        Serial.printf("new bitrate: %d\n", bitrate);
    }

    bool onConnect(bool dtr, bool rts)
    {
        Serial.printf("connection state changed, dtr: %d, rts: %d\n", dtr, rts);
        return true;  // allow to persist reset, when Arduino IDE is trying to enter bootloader mode
    }

#if 0
    void onData()
    {
        int len = USBSerial.available();
        Serial.printf("\nnew data, len %d\n", len);
        uint8_t buf[len] = {};
        USBSerial.read(buf, len);
        Serial.write(buf, len);
    }
#endif
};
#endif

#ifdef DCB
class Device: public USBCallbacks {
    void onMount() { Serial.println("Mount"); }
    void onUnmount() { Serial.println("Unmount"); }
    void onSuspend(bool remote_wakeup_en) { Serial.println("Suspend"); }
    void onResume() { Serial.println("Resume"); }
};
#endif

#ifdef DHCB				// either HID device: keyboard or gamepad
class MyHIDCallbacks : public HIDCallbacks
{
  void onData(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
  {
    Serial.printf("ID: %d, type: %d, size: %d\n", report_id, (int)report_type, bufsize);
//  for (size_t i = 0; i < bufsize; i++)
//    Serial.printf("%d\n", buffer[i]);
  }
};
#endif

#if 0
void keyboardTask(void* p)		// bombs real keyboard activity
{
  while(1) {
    delay(5000);
    Serial.println(keyboard.sendString(String("123456789\n")) ? "keyboard OK" : "keyboard FAIL");
    delay(5000);
    Serial.println(keyboard.sendString(String("abcdefghijklmnopqrst Uvwxyz\n")) ? "OK" : "FAIL");
  }
}
#endif

void setup()
{
  Serial.begin(115200);

#ifdef DK
  keyboard.setBaseEP(4);  // was 3
  if(keyboard.begin()) {
    keyboard.setCallbacks(new MyHIDCallbacks());
    Serial.write("HIDkeyboard .begin() OK\n");
  }
  else Serial.write("HIDkeyboard .begin() false\n");
#endif

#ifdef DC
  if (USBSerial.begin()) {
    USBSerial.setCallbacks(new MyCDCCallbacks());
    Serial.write("CDCusb .begin() OK\n");
  }
  else Serial.write("Failed to start CDC USB stack\n");
#endif

#ifdef DM
  char port[] = "3in1";
  if (midi.begin(port))
    Serial.write("MIDIusb .begin() OK.\n");
  else Serial.write("MIDIusb .begin() failed.\n");
#endif

#ifdef DG
   if (gamepad.begin()) {
     gamepad.setCallbacks(new MyHIDCallbacks());
     Serial.write("HIDgamepad .begin() OK.\n");
   }
   else Serial.write("HIDgamepad .begin() failed.\n");
#endif

#ifdef DCB
  EspTinyUSB::registerDeviceCallbacks(new Device());
#endif

//  xTaskCreate(keyboardTask, "kTask", 3*1024, NULL, 5, NULL);
}

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

void loop()
{
  static uint8_t i = 0;
  static uint32_t start_ms = 0;

  if(0 == start_ms)
    Serial.write("loop()ing\n");

  // change once in awhile
  if (millis() >= 286 + start_ms) {
    start_ms += 286;

#ifdef DG
    gamepad_task();
#endif
#ifdef DM
    midi.noteOFF(i%127, 127);
    if (255 <= i)
      i = 0;
    else i++;
    midi.noteON(i%127, 127);
#endif
  }

#ifdef DC
  while (USBSerial.available())
    Serial.write(USBSerial.read());
  while (Serial.available())
    USBSerial.write(Serial.read());
#else
  delay(100);		// time for interrupts
#endif
}
