const int sensorPin = A0; 
int valorHumedad = 0;    

void setup() {
  Serial.begin(9600);   
}

void loop() {
  valorHumedad = analogRead(sensorPin);

  Serial.print("Lectura cruda: ");
  Serial.print(valorHumedad);

  if (valorHumedad >= 800) {
    Serial.println(" - Estado: Suelo SECO");
  } 
  else if (valorHumedad < 800 && valorHumedad >= 400) {
    Serial.println(" - Estado: Suelo HÚMEDO");
  } 
  else {
    Serial.println(" - Estado: Suelo EMPAPADO / EN AGUA");
  }
  
  delay(1000); 
}