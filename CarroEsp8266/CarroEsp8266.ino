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
    <title>Control del Carro</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #f0f0f0;
            margin-top: 50px;
        }
        h1 {
            color: #333;
        }
        .button {
            display: inline-block;
            width: 140px;
            height: 140px;
            margin: 15px;
            font-size: 28px;
            font-weight: bold;
            color: white;
            background-color: #007BFF;
            border: none;
            border-radius: 20px;
            box-shadow: 0 6px #0056b3;
            cursor: pointer;
            user-select: none;
            transition: background-color 0.2s, box-shadow 0.2s, transform 0.1s;
        }
        .button:active {
            background-color: #0056b3;
            box-shadow: 0 3px #003f7f;
            transform: translateY(4px);
        }
        .grid {
            display: grid;
            grid-template-columns: 1fr 1fr 1fr;
            gap: 15px;
            justify-items: center;
            max-width: 450px;
            margin: auto;
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