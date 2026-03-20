#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";

const int blueLed = 2;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ===== MORSE =====
String morseCode(char c) {
  switch (toupper(c)) {
    case 'A': return ".-"; case 'B': return "-...";
    case 'C': return "-.-."; case 'D': return "-..";
    case 'E': return "."; case 'F': return "..-.";
    case 'G': return "--."; case 'H': return "....";
    case 'I': return ".."; case 'J': return ".---";
    case 'K': return "-.-"; case 'L': return ".-..";
    case 'M': return "--"; case 'N': return "-.";
    case 'O': return "---"; case 'P': return ".--.";
    case 'Q': return "--.-"; case 'R': return ".-.";
    case 'S': return "..."; case 'T': return "-";
    case 'U': return "..-"; case 'V': return "...-";
    case 'W': return ".--"; case 'X': return "-..-";
    case 'Y': return "-.--"; case 'Z': return "--..";
    case '0': return "-----"; case '1': return ".----";
    case '2': return "..---"; case '3': return "...--";
    case '4': return "....-"; case '5': return ".....";
    case '6': return "-...."; case '7': return "--...";
    case '8': return "---.."; case '9': return "----.";
    case ' ': return " ";
    default: return "";
  }
}

// ===== CONTROL =====
String currentMessage = "";
bool newMessage = false;
int dotTime = 200;

// ===== TASK MORSE =====
void morseTask(void *parameter) {
  while (true) {

    if (newMessage) {
      newMessage = false;

      for (int i = 0; i < currentMessage.length(); i++) {

        if (newMessage) break;

        char c = currentMessage[i];

        // trimite progres la browser
        ws.textAll("POS:" + String(i));

        String code = morseCode(c);

        for (int j = 0; j < code.length(); j++) {

          if (newMessage) break;

          if (code[j] == '.') {
            digitalWrite(blueLed, HIGH);
            vTaskDelay(dotTime / portTICK_PERIOD_MS);
            digitalWrite(blueLed, LOW);
          }
          else if (code[j] == '-') {
            digitalWrite(blueLed, HIGH);
            vTaskDelay((dotTime * 3) / portTICK_PERIOD_MS);
            digitalWrite(blueLed, LOW);
          }

          vTaskDelay(dotTime / portTICK_PERIOD_MS);
        }

        vTaskDelay((dotTime * 3) / portTICK_PERIOD_MS);
      }

      ws.textAll("DONE");
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// ===== WEBSOCKET =====
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {

  if (type == WS_EVT_DATA) {

    String msg = "";
    for (size_t i = 0; i < len; i++) {
      msg += (char)data[i];
    }

    int sep = msg.indexOf('|');

    if (sep > 0) {
      currentMessage = msg.substring(0, sep);
      dotTime = msg.substring(sep + 1).toInt();
    } else {
      currentMessage = msg;
    }

    newMessage = true;
  }
}

// ===== HTML UI =====
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Morse Live</title>
  <style>
    body {
      font-family: Arial;
      text-align: center;
      background: #0f172a;
      color: #fff;
    }

    h2 { color: #38bdf8; }

    input {
      padding: 12px;
      font-size: 16px;
      width: 250px;
      border-radius: 8px;
      border: none;
    }

    button {
      padding: 12px 20px;
      font-size: 16px;
      border-radius: 8px;
      border: none;
      background: #38bdf8;
      color: #000;
      cursor: pointer;
    }

    .msgBox {
      margin-top: 30px;
      font-size: 28px;
      letter-spacing: 5px;
    }

    .char { padding: 2px; }

    .active {
      color: #22c55e;
      border-bottom: 3px solid #22c55e;
    }

    .done {
      color: #64748b;
    }
  </style>
</head>
<body>

<h2>ESP32 Morse 🔵 LIVE</h2>

<input type="text" id="msg" placeholder="Scrie mesaj">
<br><br>

<label>Viteza:</label>
<input type="range" id="speed" min="50" max="500" value="200">
<span id="val">200</span> ms
<br><br>

<button onclick="sendMessage()">Trimite</button>

<div class="msgBox" id="display"></div>

<script>
var ws = new WebSocket('ws://' + location.hostname + '/ws');

let slider = document.getElementById("speed");
let val = document.getElementById("val");
let display = document.getElementById("display");

let currentText = "";

slider.oninput = function() {
  val.innerHTML = this.value;
}

function sendMessage() {
  let msg = document.getElementById("msg").value;
  let speed = slider.value;

  currentText = msg;
  renderText(-1);

  ws.send(msg + "|" + speed);
}

function renderText(activeIndex) {
  display.innerHTML = "";

  for (let i = 0; i < currentText.length; i++) {
    let span = document.createElement("span");
    span.classList.add("char");
    span.innerText = currentText[i];

    if (i < activeIndex) {
      span.classList.add("done");
    } else if (i === activeIndex) {
      span.classList.add("active");
    }

    display.appendChild(span);
  }
}

ws.onmessage = function(event) {
  let data = event.data;

  if (data.startsWith("POS:")) {
    let index = parseInt(data.split(":")[1]);
    renderText(index);
  }

  if (data === "DONE") {
    renderText(-1);
  }
};
</script>

</body>
</html>
)rawliteral";

// ===== SETUP =====
void setup() {
  pinMode(blueLed, OUTPUT);
  digitalWrite(blueLed, LOW);

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.begin();

  xTaskCreate(
    morseTask,
    "Morse Task",
    4096,
    NULL,
    1,
    NULL
  );
}

void loop() {
}