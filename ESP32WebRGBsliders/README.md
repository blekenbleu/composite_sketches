## ESP32WebRGBsliders
  Based on [ESP32_WebRGB_ColorPicker](../ESP32_WebRGB_ColorPicker) with HTML motivated by
  [RANDOM NERD TUTORIALS](https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/)  
  .. thanks to `xmlhttprequest` hint from @sierses   

This sketch makes available HTML which, when loaded by a browser, is not reloaded between slider changes.  
Only sliders V0 - V2 are used by this sketch;   
resulting WS2812 colors may not match browser values until after changing those 3 sliders.  
Based on results with this sketch,  
an ESP32-S2-Saola-1 WS2812 has less than 9 usefully different brightness levels for each primary.  
   
For ESP32-S2 WS2812 implementation details, see [esp32s2LED](https://github.com/blekenbleu/esp32s2LED).  

For context, see: [Arduino and ESP32-S2-Saola-1](https://blekenbleu.github.io/ESP32)  
