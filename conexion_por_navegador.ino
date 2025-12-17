#include <SoftwareSerial.h>

// RX del Arduino va al TX del ESP8266
// TX del Arduino va al RX del ESP8266
SoftwareSerial esp(2, 3); 

// USAMOS EL MÓVIL PORQUE ES 2.4GHz
const char SSID[] = "POCO F4 GT"; 
const char PASS[] = "arev2005";   

int ledPin = 8; // Tu LED en el pin 8

void setup() {
  Serial.begin(9600); 
  esp.begin(9600);    // IMPORTANTE: Ya lo configuramos a 9600 antes

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println(F("--- INICIANDO SISTEMA ARDUINO + ESP8266 ---"));
  delay(2000);

  // 1. Resetear el módulo para limpiar errores previos
  enviarComando("AT+RST", "OK", 3000);

  // 2. Poner en modo Estación (para conectar a WiFi)
  enviarComando("AT+CWMODE=1", "OK", 1000);

  // 3. Conectar al WiFi (Paso crítico)
  String comandoWifi = "AT+CWJAP=\"";
  comandoWifi += SSID;
  comandoWifi += "\",\"";
  comandoWifi += PASS;
  comandoWifi += "\"";
  
  Serial.println("Conectando a WiFi (puede tardar)...");
  enviarComando(comandoWifi, "OK", 8000); // 8 seg de espera

  // 4. Mostrar IP
  Serial.println("Obteniendo IP...");
  enviarComando("AT+CIFSR", "OK", 2000);

  // 5. Configurar para múltiples conexiones
  enviarComando("AT+CIPMUX=1", "OK", 1000);

  // 6. Iniciar Servidor en puerto 80
  enviarComando("AT+CIPSERVER=1,80", "OK", 1000);

  Serial.println(F("\n>>> SERVIDOR LISTO. Escuchando... <<<"));
}

void loop() {
  // Leemos lo que manda el ESP8266
  if (esp.available()) {
    // Buscamos si alguien nos habla ("+IPD")
    if (esp.find("+IPD,")) {
      delay(500); // Esperar a que lleguen todos los datos
      
      // Leer el ID de la conexión (quien nos habla)
      int conexionID = esp.read() - 48; 
      
      // Buscar qué nos piden
      String buffer = "";
      long inicio = millis();
      while (millis() - inicio < 2000) {
        if (esp.available()) {
          char c = esp.read();
          buffer += c;
          if (buffer.indexOf("HTTP") > 0) break; // Ya leímos la cabecera
        }
      }

      Serial.print("Peticion recibida ID ");
      Serial.println(conexionID);

      // --- CONTROL DEL LED ---
      if (buffer.indexOf("GET /ON") != -1) {
        digitalWrite(ledPin, HIGH);
        Serial.println("ACCION: Encender LED");
      } 
      else if (buffer.indexOf("GET /OFF") != -1) {
        digitalWrite(ledPin, LOW);
        Serial.println("ACCION: Apagar LED");
      }

      // --- RESPONDER AL NAVEGADOR ---
      responderCliente(conexionID);
    }
  }
}

// Función auxiliar para enviar la página web
void responderCliente(int id) {
  String html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  html += "<!DOCTYPE HTML><html><body>";
  html += "<h1>Control Arduino + ESP8266</h1>";
  html += "<p>Estado LED: ";
  html += digitalRead(ledPin) ? "ENCENDIDO" : "APAGADO";
  html += "</p>";
  html += "<br><a href='/ON'><button style='padding:10px;background:green;color:white'>ENCENDER</button></a>";
  html += "<br><br><a href='/OFF'><button style='padding:10px;background:red;color:white'>APAGAR</button></a>";
  html += "</body></html>";

  // 1. Decirle al ESP cuántos caracteres vamos a mandar
  esp.print("AT+CIPSEND=");
  esp.print(id);
  esp.print(",");
  esp.println(html.length());

  delay(200); // Espera breve

  // 2. Mandar el HTML
  esp.print(html);
  
  delay(500); // Dar tiempo a enviar

  // 3. Cerrar conexión
  esp.print("AT+CIPCLOSE=");
  esp.println(id);
}

// Función auxiliar para mandar comandos AT y ver respuesta
void enviarComando(String comando, char* respuestaEsperada, int tiempoEspera) {
  esp.println(comando);
  long int time = millis();
  while ((time + tiempoEspera) > millis()) {
    while (esp.available()) {
      char c = esp.read();
      Serial.write(c);
    }
  }
  Serial.println(); // Salto de línea visual
}