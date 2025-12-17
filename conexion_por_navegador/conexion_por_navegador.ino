#include <SoftwareSerial.h>

SoftwareSerial esp(2, 3); 
int led = 8;

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  esp.begin(9600);

  cmd("AT+RST", 2000);
  cmd("AT+CWMODE=1", 1000);
  
  // Aumentamos a 8 segundos porque las redes de campus son lentas
  cmd("AT+CWJAP=\"POCO F4 GT\",\"arev2005\"", 8000); 
  
  // COMANDO CORREGIDO: Es CIFSR (Status Requerido)
  Serial.println(F("\n--- MI DIRECCION IP ES: ---"));
  cmd("AT+CIFSR", 3000); 
  
  cmd("AT+CIPMUX=1", 1000);
  cmd("AT+CIPSERVER=1,80", 1000);
  Serial.println(F("---------------------------\n"));
}

void loop() {
  if (esp.available()) {
    if (esp.find("+IPD,")) {
      delay(300);
      int id = esp.read() - 48;
      String peticion = esp.readStringUntil('\r');
      
      if (peticion.indexOf("GET /ON") != -1) digitalWrite(led, HIGH);
      if (peticion.indexOf("GET /OFF") != -1) digitalWrite(led, LOW);

      String estado = digitalRead(led) ? "ENCENDIDO" : "APAGADO";
      String res = "<html><center><h1>LED: " + estado + "</h1>";
      res += "<a href='/ON'><button>ENCENDER</button></a>";
      res += "<a href='/OFF'><button>APAGAR</button></a></center></html>";

      esp.print("AT+CIPSEND=" + String(id) + "," + String(res.length()) + "\r\n");
      delay(200); 
      esp.print(res);
      delay(200);
      esp.print("AT+CIPCLOSE=" + String(id) + "\r\n");
    }
  }
}

void cmd(String at, int t) {
  esp.println(at);
  unsigned long inicio = millis();
  while (millis() - inicio < t) {
    while (esp.available()) {
      Serial.write(esp.read());
    }
  }
}