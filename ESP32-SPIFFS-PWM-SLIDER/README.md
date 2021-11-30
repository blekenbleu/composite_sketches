### ESP32 Web Server (WebSocket)-PWM with Multiple Sliders
From: [microdigsoft.com](https://microdigisoft.com/esp32-web-server-websocket-pwm-with-multiple-sliders)

#### PWM controlled LEDs with Websocket protocol between webpage and Arduino server
![WebSockets](https://i0.wp.com/microdigisoft.com/wp-content/uploads/2021/10/ESP32-SPIFFS-PWM-SLIDER-2-3.png)  

```
Sketch uses 720826 bytes (54%) of program storage space. Maximum is 1310720 bytes.
Global variables use 32936 bytes (10%) of dynamic memory,
 leaving 294744 bytes for local variables. Maximum is 327680 bytes.
```

HTML, CSS, and JavaScript files are stored
[using ESP32 Web Server-SPI Flash File System (SPIFFS)](https://microdigisoft.com/esp32-web-server-spi-flash-file-system-spiffs)  

![SPIFFS](https://i0.wp.com/microdigisoft.com/wp-content/uploads/2021/10/ESP32-SPIFFS-PWM-SLIDER-1-1.png)

Special library required:  [Arduino_JSON](https://github.com/arduino-libraries/Arduino_JSON) 

See also:
- [Memory Corruption in ESP32 WebSocket Server](https://community.platformio.org/t/memory-corruption-in-esp32-websocket-server/21558)  
    - bug in `handleWebSocketMessage()`  
- [ESP32 Robot Car Using Websockets](https://www.donskytech.com/esp32-robot-car-using-websockets)  
- [ESP32 WebSocket Server using Arduino IDE - Control GPIOs and Relays](https://microcontrollerslab.com/esp32-websocket-server-arduino-ide-control-gpios-relays)
- [ESP32 WebSocket Server: Control Outputs (Arduino IDE)](https://randomnerdtutorials.com/esp32-websocket-server-arduino)
