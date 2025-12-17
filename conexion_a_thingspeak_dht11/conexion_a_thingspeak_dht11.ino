#include <SoftwareSerial.h>
#include <DHT.h> 

#define DHTPIN 13     
#define DHTTYPE DHT11 

SoftwareSerial esp8266(2, 3); // RX, TX
DHT dht(DHTPIN, DHTTYPE);

String ssid     = "POCO F4 GT";
String password = "arev2005";
String apiKey   = "DY4ALPNFXO9UNA7U";
String host     = "api.thingspeak.com";

void setup() {
  Serial.begin(9600);          
  esp8266.begin(115200);      
  dht.begin();                 

  delay(2000);
  Serial.println("Iniciando conexión con DHT11 (Adafruit) y ESP-01...");

  // Configura la velocidad del ESP01 a 9600 para que sea estable con SoftwareSerial
  sendCommand("AT+UART_DEF=9600,8,1,0,0", 2000);
  esp8266.begin(9600);

  connectWiFi();
}

void loop() {
  // Esperar 2 segundos entre mediciones (requerido por DHT11)
  // Aunque tienes un delay de 15s al final, el sensor necesita tiempo interno.
  
  // Lectura directa (sin dht.read() previo)
  float temperatura = dht.readTemperature(); 
  float humedad     = dht.readHumidity();    

  // Comprobar si la lectura falló (isnan = is not a number)
  if (isnan(temperatura) || isnan(humedad)) {
    Serial.println("Error: Fallo al leer del sensor DHT!");
    delay(2000);
    return; // Volver a intentar
  }

  // Si llegamos aquí, la lectura fue exitosa
  Serial.println("Lectura del sensor:");
  Serial.print("   🌡 Temperatura aire: ");
  Serial.print(temperatura);
  Serial.println(" °C");

  Serial.print("   💧 Humedad: ");
  Serial.print(humedad);
  Serial.println(" %");

  enviarDatos(temperatura, humedad);
  
  // ThingSpeak permite actualizaciones cada 15 segundos aprox en cuentas gratuitas
  delay(15000);  
}

void connectWiFi() {
  sendCommand("AT+RST", 2000);
  sendCommand("AT+CWMODE=1", 1000);
  sendCommand("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"", 10000); // Aumenté el tiempo de espera para conectar al WiFi

  Serial.println("Intentando conectar al WiFi...");
}

void enviarDatos(float temperatura, float humedad) {
  String peticion = "GET /update?api_key=" + apiKey + 
                    "&field1=" + String(temperatura) + 
                    "&field2=" + String(humedad) + 
                    " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";

  sendCommand("AT+CIPSTART=\"TCP\",\"" + host + "\",80", 2000);
  sendCommand("AT+CIPSEND=" + String(peticion.length()), 1000);

  esp8266.print(peticion);
  
  delay(2000);
  sendCommand("AT+CIPCLOSE", 1000);
  Serial.println("\n--- Datos enviados a ThingSpeak! ---");
}

String sendCommand(String cmd, int timeout) {
  esp8266.println(cmd);
  delay(timeout);
  while(esp8266.available()){
    Serial.write(esp8266.read());
  }
}
