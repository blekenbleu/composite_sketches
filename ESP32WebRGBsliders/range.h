const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <style>

input {
    border-style: none;
    size:1;
}

input[type=range] {
    min:0;
    max:100;
    -webkit-appearance: none;
    appearance: none;
    position:relative;
    width: 70%;
    height: 10px;
    left:2%;
    margin-right:2%;
    background: #c0c0c0;
    opacity: 0.7;		/* transparency (for mouse-over effects on hover) */
    -webkit-transition: .2s;	/* 0.2 seconds transition on hover */
    transition: opacity .2s;
/*  oninput: "pV(this.id)"; */
}

input[type=range]:hover {
    opacity: 1;			/* Fully shown on mouse-over */
}
    
    </style>

</head>
<body onload="oV()">

    <h3 style="color:blue;">Live slider[s]</h3>
    <i>return values on mouse lifts; see:
 <a href="https://github.com/blekenbleu/otherESP32S2examples/tree/main/ESP32WebRGBsliders">ESP32WebRGBsliders</a></i><br>

    <FORM action="/get" method="get" id="sliders">
    <P>
    <input type="range" value="5" id="s0" oninput="pV(this.id)" onchange="gV(this.id)"> V0: <input id="os0" /><br>
    <input type="range" value="2" id="s1" oninput="pV(this.id)" onchange="gV(this.id)"> V1: <input id="os1" /><br>
    <input type="range" value="8" id="s2" oninput="pV(this.id)" onchange="gV(this.id)"> V2: <input id="os2" /><br>
    <input type="range" value="50" id="s3" oninput="pV(this.id)" onchange="gV(this.id)"> V3: <input id="os3" /><br>
    <input type="range" value="50" id="s4" oninput="pV(this.id)" onchange="gV(this.id)"> V4: <input id="os4" /><br>
    <input type="range" value="50" id="s5" oninput="pV(this.id)" onchange="gV(this.id)"> V5: <input id="os5" /><br>
    <input type="range" value="50" id="s6" oninput="pV(this.id)" onchange="gV(this.id)"> V6: <input id="os6" /><br>
    <input type="range" value="50" id="s7" oninput="pV(this.id)" onchange="gV(this.id)"> V7: <input id="os7" /><br>
    </P>
    <input id="color" name="color"  />
    </FORM>

    <script>
        function pV(pin) {
            var vi = document.getElementById(pin);
            var vo = document.getElementById("o"+pin);
            vo.value = vi.value;
        }

        function oV() { for (i = 0; i < 8; i++) pV("s"+i); }

        function gV(rn) {
            var i = document.getElementById(rn);
            var c = document.getElementById("color");
            c.value = "/get?color="+i.value+"&id="+rn;
            var xhr = new XMLHttpRequest();
            xhr.open("GET", c.value, true);
            xhr.send();
        }
    </script>

</body>
</html>
)rawliteral";
