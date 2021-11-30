var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onload);

const status = {
 isOpen: false,

 set change(x) {this.isOpen = x;}
};

function onload(event) {
    var keys = ["slider1", "slider2", "slider3"];
    for (var i = 0; i < keys.length; i++){
        var mySlider = document.getElementById(keys[i]);
        document.getElementById("slider"+ (i+1).toString()).value = mySlider.value;
        document.getElementById("sliderValue"+ (i+1).toString()).innerHTML = mySlider.value;
    }
    initWebSocket();
}

function getValues(){
  if (status.isOpen)
    websocket.send("getValues");
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    if (status.isOpen) {
      websocket.onopen = onOpen;
      websocket.onclose = onClose;
      websocket.onmessage = onMessage;
    }
}

function onOpen(event) {
    console.log('Connection opened');
    status.change = true;
    getValues();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function updateSliderPWM(element) {
    var sliderNumber = element.id.charAt(element.id.length-1);
    var sliderValue = document.getElementById(element.id).value;
    document.getElementById("sliderValue"+sliderNumber).innerHTML = sliderValue;
    //if (3 == sliderNumber) status.change = !status.isOpen;
    console.log(sliderNumber.toString()+"="+sliderValue.toString());
    if (status.isOpen)
      websocket.send(sliderNumber+"s"+sliderValue.toString());
}

function onMessage(event) {
    console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);

    for (var i = 0; i < keys.length; i++){
        var key = keys[i];
        document.getElementById(key).innerHTML = myObj[key];
        document.getElementById("slider"+ (i+1).toString()).value = myObj[key];
    }
}
