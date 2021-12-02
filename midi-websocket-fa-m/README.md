# [midi-websocket](https://github.com/fa-m/midi-websocket)

## How to install

### Node.js server:   
   Install npm dependencies with `npm install` and execute `npm start` after installation, so the Server will be started.

### JavaScript client:  
* Open the `client/index.html` and change the following line, if needed.

    const connection = new WebSocket('ws://localhost:9000');
    
* Connect a USB-Midi device to your computer/device and open `client/index.html` in your browser. (only tested in Google Chrome!)

    
## Usage

Everything sent by a USB-device will be broadcasted to clients connected via the websocket.

## Links

*  [Tutorial Midi Device in HTML5](https://code.tutsplus.com/tutorials/introduction-to-web-midi--cms-25220)

### blekenbleu revisions
* added connection.close() to stop Chrome browser crashing on page close
* attempt to convince [mnet MIDIHUb]() to pass MIDI, but no joy..
    * mnet.log reports:
```
      WebSocketIO id: 2 closed
      WebSocketIO id: 3 closed
```
   .. so presumably need to associate those id's with WebSocket messages.

