#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const int IN1 = 13; // D7 - motor derecho +
const int IN2 = 2;  // D4 - motor derecho -
const int IN3 = 14; // D5 - motor izquierdo +
const int IN4 = 12; // D6 - motor izquierdo -
const int BUZZER_PIN = 5; // D1
const int LED_FRONT_PIN = 16; // D0 - LED frontal

// Credenciales del AP
const char* ssid = "CarroESP8266";
const char* password = "controlremoto";
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Control del Carro</title>
  <style>
    body {
      font-family: 'Segoe UI', sans-serif;
      text-align: center;
      background-color: #f2f2f2; /* Fondo claro como en la app */
      margin: 0;
      padding: 20px;
    }

    h1 {
      color: #333333;
    }

    .grid {
      display: grid;
      grid-template-columns: 1fr 1fr 1fr;
      gap: 12px;
      justify-items: center;
      align-items: center;
      max-width: 100%;
      margin: auto;
    }

    .button {
      width: 26vw;
      height: 26vw;
      max-width: 100px;
      max-height: 100px;
      font-size: 7vw;
      font-weight: bold;
      color: white;
      background: linear-gradient(135deg, #007BFF, #00BFFF); /* Azul app */
      border: none;
      border-radius: 18px;
      box-shadow: 0 4px 10px rgba(0, 123, 255, 0.4);
      cursor: pointer;
      transition: all 0.2s ease;
    }

    .button:active {
      background: linear-gradient(135deg, #005FCC, #009ACD); /* más oscuro al presionar */
      transform: scale(0.96);
      box-shadow: 0 2px 6px rgba(0, 123, 255, 0.2);
    }

    @media (min-width: 600px) {
      .button {
        width: 90px;
        height: 90px;
        font-size: 28px;
      }
    }
  </style>
</head>
<body>
  <h1>Control del Carro</h1>
  <div class="grid">
    <div></div>
    <button class="button" onclick="sendCommand('forward')">↑</button>
    <div></div>

    <button class="button" onclick="sendCommand('left')">←</button>
    <button class="button" onclick="sendCommand('stop')">■</button>
    <button class="button" onclick="sendCommand('right')">→</button>

    <div></div>
    <button class="button" onclick="sendCommand('backward')">↓</button>
    <div></div>
  </div>

  <script>
    function sendCommand(cmd) {
      fetch('/' + cmd)
        .then(response => console.log("Comando enviado:", cmd))
        .catch(err => console.error("Error al enviar comando", err));
    }
  </script>
</body>
</html>
)rawliteral";


ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando...");

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(LED_FRONT_PIN, OUTPUT);
  digitalWrite(LED_FRONT_PIN, LOW); // Apagar el LED al inicio

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Modo AP
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("Dirección IP del AP: ");
  Serial.println(IP);  

  server.on("/", handleRoot);
  server.on("/forward", [](){ handleCommand("forward"); });
  server.on("/backward", [](){ handleCommand("backward"); });
  server.on("/left", [](){ handleCommand("left"); });
  server.on("/right", [](){ handleCommand("right"); });
  server.on("/stop", [](){ handleCommand("stop"); });
  server.on("/beep", [](){ handleCommand("beep"); });
  server.on("/nobeep", [](){ handleCommand("nobeep"); });
  server.on("/ledon", [](){ handleCommand("ledon"); });
  server.on("/ledoff", [](){ handleCommand("ledoff"); });

  server.begin();
  Serial.println("Servidor web iniciado");
}

void loop() {
  server.handleClient();  
}

void handleCommand(String cmd) {
  Serial.print("Comando recibido: ");
  Serial.println(cmd);

  if (cmd == "forward") {
    // Invertimos lógica para que avance correctamente
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); // Motor derecho adelante
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); // Motor izquierdo adelante
  } else if (cmd == "backward") {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); // Motor derecho atrás
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); // Motor izquierdo atrás
  } else if (cmd == "left") {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); // Motor derecho adelante
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); // Motor izquierdo atrás
  } else if (cmd == "right") {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); // Motor derecho atrás
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); // Motor izquierdo adelante
  } else if (cmd == "stop") {
    digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  }

  if (cmd == "beep") {
    digitalWrite(BUZZER_PIN, HIGH);

  } else if (cmd == "nobeep") {
    digitalWrite(BUZZER_PIN, LOW);
  }

  if (cmd == "ledon") {
    digitalWrite(LED_FRONT_PIN, HIGH);
  } else if (cmd == "ledoff") {
    digitalWrite(LED_FRONT_PIN, LOW);
  }

  server.send(200, "text/plain", "OK");
}