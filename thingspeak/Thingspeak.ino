#include <SoftwareSerial.h>

SoftwareSerial EspSerial(2,3);
String host = "api.thingspeak.com";
String apiKey = "DY4ALPNFXO9UNA7U";

void sendCmd(String cmd, int timeout){
  EspSerial.println(cmd);
  delay(timeout);
  while(EspSerial.available()){
    Serial.write(EspSerial.read());
  }
}

void sendData(float data){
  String peticion = "GET /update?api_key=" +apiKey+
                    "&field1=" + String(data)+
                    " HTTP/1.1\r\nHost: " +host+ "\r\n\r\n";
  sendCmd("AT+CIPSTART=\"TCP\",\"" +host+ "\",80",2000);
  sendCmd("AT+CIPSEND=" +String(peticion.length()),1000);
  EspSerial.print(peticion);
  delay(2000);
  sendCmd("AT+CIPCLOSE",1000);
}

void setup(){
  Serial.begin(9600);
  EspSerial.begin(9600);
  
  sendCmd("AT+CWMODE=1",1000);
  sendCmd("AT+CWJAP=\"POCO F4 GT\",\"arev2005\"",10000);
}

void loop(){
  float data = 68;
  sendData(data);
  delay(2000);
}
