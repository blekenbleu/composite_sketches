/*
 * This is composite example
 * Devices using available S2 endpoints
 * HID, MIDI, CDC serial and Serial from CP210x
 * 
 * author: chegewara; Hacked by blekenbleu
 */

#include "Arduino.h"

#ifdef DC
#include "cdcusb.h"
#endif

#ifdef DK
#define DHCB 1			// for MyHIDCallbacks
#include "hidkeyboard.h"
#endif

#define DM 1
#ifdef DM
#include "midiusb.h"
#endif

//#define DG 1
#ifdef DG
#define DHCB 1			// for MyHIDCallbacks
#include "hidgamepad.h"
#endif


#ifdef DC
CDCusb USBSerial;
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

/*
    void onData()
    {
        int len = USBSerial.available();
        Serial.printf("\nnew data, len %d\n", len);
        uint8_t buf[len] = {};
        USBSerial.read(buf, len);
        Serial.write(buf, len);
    }
 */
};
#endif

class Device: public USBCallbacks {
    void onMount() { Serial.println("Mount"); }
    void onUnmount() { Serial.println("Unmount"); }
    void onSuspend(bool remote_wakeup_en) { Serial.println("Suspend"); }
    void onResume() { Serial.println("Resume"); }
};

#ifdef DHCB
class MyHIDCallbacks : public HIDCallbacks
{
    void onData(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
    {
        Serial.printf("ID: %d, type: %d, size: %d\n", report_id, (int)report_type, bufsize);
        for (size_t i = 0; i < bufsize; i++)
        {
            Serial.printf("%d\n", buffer[i]);
        }
    }
};
#endif

void setup()
{
    Serial.begin(115200);

#ifdef DK
    keyboard.setBaseEP(4);  // was 3
    if(keyboard.begin())
      Serial.write("HIDkeyboard .begin() OK\n");
    else Serial.write("HIDkeyboard .begin() false\n");
#define DCB 1
    keyboard.setCallbacks(new MyHIDCallbacks());
#endif

#ifdef DC
    if (USBSerial.begin())
        Serial.write("CDCusb .begin() OK\n");
    else Serial.write("Failed to start CDC USB stack\n");
#define DCB 1
    USBSerial.setCallbacks(new MyCDCCallbacks());
#endif

#ifdef DM
    char port[] = "3in1";
    if (midi.begin(port))
      Serial.write("MIDIusb .begin() OK.\n");
    else Serial.write("MIDIusb .begin() failed.\n");
#endif
#ifdef DG
     if (gamepad.begin())
       Serial.write("HIDgamepad .begin() OK.\n");
     else Serial.write("HIDgamepad .begin() failed.\n");
#endif
#ifdef DB
    EspTinyUSB::registerDeviceCallbacks(new Device());
#endif

//  xTaskCreate(keyboardTask, "kTask", 3*1024, NULL, 5, NULL);

}

/*
void keyboardTask(void* p)
{
    while(1) {
        delay(5000);
        Serial.println(keyboard.sendString(String("123456789\n")) ? "keyboard OK" : "keyboard FAIL");
        delay(5000);
        Serial.println(keyboard.sendString(String("abcdefghijklmnopqrst Uvwxyz\n")) ? "OK" : "FAIL");
    }
}
*/

void loop()
{
    static uint8_t i = 0;
    static uint32_t start_ms = 0;

    if(0 == start_ms)
      Serial.write("loop()ing\n");

    // change once per second
    if (millis() - start_ms >= 286) {
      start_ms += 286;

#ifdef DG
      uint32_t buttons = i;
      // 8 bits to 32
      buttons |= (buttons << 8);
      buttons |= (buttons << 16);
      gamepad.sendAll(buttons, i, i, i, i, i, i, i%5);
#endif
#ifdef DM
      midi.noteOFF(i%127, 127);
#endif
      if (255 <= i)
        i = 0;
      else i++;
#ifdef DM
      midi.noteON(i%127, 127);
#endif
    }

#ifdef DC
    while (USBSerial.available()) {
        Serial.write(USBSerial.read());
    }
    while (Serial.available()) {
        USBSerial.write(Serial.read());
    }
#endif
}
