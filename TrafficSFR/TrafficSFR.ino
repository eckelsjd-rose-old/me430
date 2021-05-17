// #define PIN_LED_RED 6
// #define PIN_LED_YELLOW 5
// #define PIN_LED_GREEN 4
// #define PIN_BUTTON 2

#define REG_RED_DDR DDRH   // pin 6 = PH3
#define REG_RED_PORT PORTH
#define REG_RED_PIN PINH
#define BIT_RED 3

#define REG_YEL_DDR DDRE   // pin 5 = PE3
#define REG_YEL_PORT PORTE
#define REG_YEL_PIN PINE
#define BIT_YEL 3

#define REG_GREEN_DDR DDRG   // pin 4 = G5
#define REG_GREEN_PORT PORTG
#define REG_GREEN_PIN PING
#define BIT_GREEN 5

#define REG_PB_DDR DDRE // pushbutton, pin 2 = E4
#define REG_PB_PORT PORTE
#define REG_PB_PIN PINE
#define BIT_PB 4


void setup() {
  //pinMode(PIN_LED_RED,OUTPUT);
  //pinMode(PIN_LED_YELLOW, OUTPUT);
  //pinMode(PIN_LED_GREEN, OUTPUT);
  
  REG_RED_DDR |= _BV(BIT_RED);
  REG_YEL_DDR |= _BV(BIT_YEL);
  REG_GREEN_DDR |= _BV(BIT_GREEN);
  
  //pinMode(PIN_BUTTON,INPUT_PULLUP);
  
  REG_PB_DDR &= ~_BV(BIT_PB); // default behavior
  REG_PB_PORT |= _BV(BIT_PB); // input pullup
}

void loop() {
  //if ( digitalRead(PIN_BUTTON) == 0) {
  //if ( !((REG_PB_PIN >> BIT_PB) & 0x01) ) {
  if ( bit_is_clear(REG_PB_PIN,BIT_PB) ) {
    REG_RED_PORT &= ~_BV(BIT_RED);
    REG_YEL_PORT &= ~_BV(BIT_YEL);
    REG_GREEN_PORT |= _BV(BIT_GREEN);
    delay(500);
    
    REG_RED_PORT &= ~_BV(BIT_RED);
    REG_YEL_PORT |= _BV(BIT_YEL);
    REG_GREEN_PORT &= ~_BV(BIT_GREEN);
    delay(500);
    
  }
  
  //digitalWrite(PIN_LED_RED,HIGH);
  //digitalWrite(PIN_LED_YELLOW,LOW);
  //digitalWrite(PIN_LED_GREEN,LOW);
  REG_RED_PORT |= _BV(BIT_RED);
  REG_YEL_PORT &= ~_BV(BIT_YEL);
  REG_GREEN_PORT &= ~_BV(BIT_GREEN);
  
  
  delay(100);
}