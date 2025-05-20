#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

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


// Creamos el servidor en el puerto 80
ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando...");

  // Modo AP
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("Dirección IP del AP: ");
  Serial.println(IP);  // Debería ser 192.168.4.1 por defecto

  // Rutas del servidor
  server.on("/", handleRoot);
  server.on("/forward", [](){ handleCommand("adelante"); });
  server.on("/backward", [](){ handleCommand("atrás"); });
  server.on("/left", [](){ handleCommand("izquierda"); });
  server.on("/right", [](){ handleCommand("derecha"); });
  server.on("/stop", [](){ handleCommand("detener"); });

  server.begin();
  Serial.println("Servidor web iniciado");
}

void loop() {
  server.handleClient();  // Escucha peticiones
}

void handleCommand(String cmd) {
  Serial.print("Comando recibido: ");
  Serial.println(cmd);
  server.send(200, "text/plain", "OK");
}