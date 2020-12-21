#include <Servo.h>

Servo myServo;
int potValue;
int degrees;

void setup() {
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  
  // Outputs for debugging LEDs
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  
  myServo.attach(9);
}

void loop() {
  // Outputs for the debugging LEDs
  digitalWrite(4, !digitalRead(2));
  digitalWrite(5, !digitalRead(3));
  analogWrite(6, analogRead(0)/4); // analogWrite is 0 to 255 (8-bit)
  
  if(!digitalRead(2)) {
    myServo.write(0);
  } else if (!digitalRead(3)) {
    myServo.write(180);
  } else {
    potValue = analogRead(0); // 0 to 1023
    degrees = map(potValue, 0, 1023, 0, 180);
    myServo.write(degrees);
  }
  
  delay(10);
}