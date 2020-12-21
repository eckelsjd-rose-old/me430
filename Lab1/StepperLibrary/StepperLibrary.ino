#include <Stepper.h>

const int stepsPerRevolution = 2048; // 32 steps * 64:1 gearhead
// const int stepsPerRevolution = 60; // lie

Stepper myStepper(stepsPerRevolution,8,10,9,11);

void setup(){
  myStepper.setSpeed(9); // 5 rpm
}

void loop() {
  myStepper.step(stepsPerRevolution);
  delay(500);
  
  myStepper.step(-stepsPerRevolution);
  delay(500);
  

}