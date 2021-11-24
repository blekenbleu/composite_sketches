/*
 ; Try to simultaneously run CDC, MIDI and gamepad
 ; Using available ESP32-S2 endpoints
 ; and of course Serial from CP210x
 ; 
 ; author: blekenbleu; hacked from all_in_one by chegewara
 */
#include "Arduino.h"
#include "cdcusb.h"
#include "hidgamepad.h"
#include "midiusb.h"

CDCusb USBSerial;
HIDgamepad gamepad;
#if 0
#define D3 1
MIDIusb midi;
#endif

class MyCDCCallbacks : public CDCCallbacks
{
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

    void onData()
    {
        int len = USBSerial.available();
        Serial.printf("\nnew data, len %d\n", len);
        uint8_t buf[len] = {};
        USBSerial.read(buf, len);
        Serial.write(buf, len);
    }
};

class Device: public USBCallbacks
{
    void onMount() { Serial.write("Mounti\n"); }
    void onUnmount() { Serial.write("Unmount\n"); }
    void onSuspend(bool remote_wakeup_en) { Serial.write("Suspend\n"); }
    void onResume() { Serial.write("Resume\n"); }
};

class MyHIDCallbacks : public HIDCallbacks
{
    void onData(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
    {
        Serial.printf("ID: %d, type: %d, size: %d\n", report_id, (int)report_type, bufsize);
        for (size_t i = 0; i < bufsize; i++) {
            Serial.printf("%d\n", buffer[i]);
        }
    }
};

void setup()
{
    char port[] = "3in1";
    Serial.begin(115200);

    Serial.write("USBSerial.setCallbacks()\n");
    USBSerial.setCallbacks(new MyCDCCallbacks());
    
    Serial.write("EspTinyUSB::registerDeviceCallbacks()\n");
    EspTinyUSB::registerDeviceCallbacks(new Device());

    gamepad.begin();

#ifdef D3
    midi.begin(port);
#endif
}

void loop()
{
    static uint8_t i = 0;
    static uint32_t start_ms = 0;

    // change gamepad once per second
    if (millis() - start_ms >= 286) {
      start_ms += 286;
      uint32_t buttons = i;

      // 8 bits to 32
      buttons |= (buttons << 8);
      buttons |= (buttons << 16);
      gamepad.sendAll(buttons, i, i, i, i, i, i, i%5);

#ifdef D3
      midi.noteOFF(i%127, 127);
#endif
      if (255 <= i)
        i = 0; 
      else i++;
#ifdef D3
      midi.noteON(i%127, 127);
#endif
    }
    
    while (USBSerial.available()) {
        Serial.write(USBSerial.read());
    }
    while (Serial.available()) {
        USBSerial.write(Serial.read());
    }
}
