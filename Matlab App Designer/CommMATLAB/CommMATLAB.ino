#include <Servo.h>

#define PIN_PUSHBUTTON 2
#define PIN_LED 3
#define PIN_JOYSTICK_X 0
#define PIN_JOYSTICK_Y 1
#define PIN_SERVO 9

Servo serv;
String inputString = "";
bool stringComplete = false;

int joystickX = 0;
int old_joystickX = 0;
int joystickY = 0;
int old_joystickY = 0;
#define JOYSTICK_THRESHOLD 10

void setup() {
  Serial.begin(9600);
  pinMode(PIN_PUSHBUTTON,INPUT_PULLUP);
  pinMode(PIN_LED,OUTPUT);
  serv.attach(PIN_SERVO);
  inputString.reserve(200);
}

void loop() {
  if ( !digitalRead(PIN_PUSHBUTTON) ) {
    Serial.println("B");
    delay(500);
  }
  
  // Joystick stuff
  joystickX = analogRead(PIN_JOYSTICK_X);
  if ( abs(joystickX - old_joystickX) > JOYSTICK_THRESHOLD) {
    Serial.println(String("X ") + joystickX);
    old_joystickX = joystickX;
  }
  
  joystickY = analogRead(PIN_JOYSTICK_Y);
  if ( abs(joystickY - old_joystickY) > JOYSTICK_THRESHOLD) {
    Serial.println(String("Y ") + joystickY);
    old_joystickY = joystickY;
  }
  
  
  if (stringComplete) {
    inputString.toUpperCase();
    inputString.trim(); // gets rid of Carriage return CR (ASCII 13)
    
    if (inputString.equals("LED ON")) {
      digitalWrite(PIN_LED,HIGH);
    }
    if (inputString.equals("LED OFF")) {
      digitalWrite(PIN_LED,LOW);
    }
    if (inputString.startsWith("SERVO ")) {
      int deg = inputString.substring(6).toInt();
      serv.write(deg);
    }
    
    inputString = "";
    stringComplete = false;
  }
  
  delay(50);
  
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char) Serial.read();
    if (inChar == '\n') {   // line feed (ASCII 10)
      stringComplete = true;
    } else {
      inputString += inChar;
    }  
  }  
}