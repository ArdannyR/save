#include <Servo.h>
int pins= 2;

Servo servoMotor;

void setup(){
  servoMotor.attach(pins);
}

void loop(){
  servoMotor.write(0);
  delay(2000);
  
  servoMotor.write(90);
  delay(2000);
  
  servoMotor.write(180);
  delay(2000);
}