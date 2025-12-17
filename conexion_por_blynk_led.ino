#define BLYNK_TEMPLATE_ID "TMPL2SEQy2yY8"
#define BLYNK_TEMPLATE_NAME "Practica Home"
#define BLYNK_AUTH_TOKEN "k6OTr5UjzRPIFwL9DmVEIm1XxurNKnNb"
#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SoftwareSerial.h>

// WIFI

char ssid[] = "POCO F4 GT";
char pass[] = "arev2005";
int ledPin = 8;

// Comunicaciones con ESP8266
SoftwareSerial EspSerial(2,3); //RX,TX
ESP8266 wifi(&EspSerial);
BlynkTimer timer;

void setup() {
  Serial.begin(9600);
  EspSerial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass);
  pinMode(ledPin, OUTPUT);

}

void loop() {
  Blynk.run();
  timer.run();

}