#include <SoftwareSerial.h>

SoftwareSerial esp(2, 3); // RX, TX
const char SSID[] = "CAMPUS_EPN", PASS[] = "politecnica**";

void setup() {
  Serial.begin(9600);
  esp.begin(9600);
  pinMode(8, OUTPUT);

  enviarAT("AT+RST", 2000);      
  enviarAT("AT+CWJAP=\"" + String(SSID) + "\",\"" + String(PASS) + "\"", 5000); 
  enviarAT("AT+CIPMUX=1", 1000); 
  enviarAT("AT+CIPSERVER=1,80", 1000); 
  Serial.println("Listo");
}

void loop() {
  if (esp.available()) {
    if (esp.find("+IPD,")) { 
      delay(500);
      int id = esp.read() - '0'; 
      
      String peticion = esp.readString();
      if (peticion.indexOf("GET /ON") != -1) digitalWrite(8, HIGH);
      if (peticion.indexOf("GET /OFF") != -1) digitalWrite(8, LOW);

      responder(id);
    }
  }
}

void responder(int id) {
  String html = "<h1>LED</h1><a href='/ON'>ON</a> <a href='/OFF'>OFF</a>";
  
  esp.print("AT+CIPSEND=");
  esp.print(id);
  esp.print(",");
  esp.println(html.length());
  
  delay(500);
  if (esp.find(">")) {
    esp.print(html);
    delay(500);
    esp.print("AT+CIPCLOSE=");
    esp.println(id);
  }
}

void enviarAT(String cmd, int t) {
  esp.println(cmd);
  delay(t);
  while (esp.available()) Serial.write(esp.read());
}