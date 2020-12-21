int rPot;
int gPot;
int bPot;

void setup() {
  Serial.begin(9600); // COM speed with computer
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(2,INPUT_PULLUP);
}

void loop() {
  
  rPot = analogRead(0); // read from analog pin 0
  gPot = 0;
  bPot = analogRead(1);
  
  if (digitalRead(2)) {
    rgb(rPot/4,gPot/4,bPot/4); // turn LED on
  } else {
    rgb(0,0,0);
  }
  
  //char buffer[500];
  //sprintf(buffer,"R = %d, G = %d, B = %d",rPot,gPot,bPot);
  //Serial.println(buffer);
  //memset(buffer,0,sizeof(buffer));
  Serial.print("R=");
  Serial.print(rPot/4);
  Serial.print(" G=");
  Serial.print(gPot/4);
  Serial.print(" B=");
  Serial.println(bPot/4);
  
  
  delay(100); // 1 sec
  
}

void rgb(int r, int g, int b){
  analogWrite(9,r);
  analogWrite(10,g);
  analogWrite(11,b);
}