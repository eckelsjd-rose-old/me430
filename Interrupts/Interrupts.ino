// uno
/*
#define REG_DDR_PB_PRINT DDRD
#define REG_PORT_PB_PRINT PORTD
#define REG_PIN_PB_PRINT PIND
#define BIT_PB_PRINT 3
#define REG_DDR_PB_COUNT DDRD
#define REG_PORT_PB_COUNT PORTD
#define REG_PIN_PB_COUNT PIND
#define BIT_PB_COUNT 2
*/

// Mega2560
#define REG_DDR_PB_PRINT DDRE
#define REG_PORT_PB_PRINT PORTE
#define REG_PIN_PB_PRINT PINE
#define BIT_PB_PRINT 5
#define REG_DDR_PB_COUNT DDRE
#define REG_PORT_PB_COUNT PORTE
#define REG_PIN_PB_COUNT PINE
#define BIT_PB_COUNT 4

// interrupt service routine
volatile uint16_t isrCounter = 0;

// Main event flags
volatile uint8_t mainEventFlags = 0;
#define PB_FLAG 0x01
#define FLAG2 0x02
#define FLAG3 0x04 // etc
#define FLAG4 0x08

void setup()
{
  Serial.begin(9600);
  
  // pins as inputs
  REG_DDR_PB_PRINT &= ~_BV(BIT_PB_PRINT);
  REG_DDR_PB_COUNT &= ~_BV(BIT_PB_COUNT);
  
  // input pull-up resistors
  REG_PORT_PB_PRINT |= _BV(BIT_PB_PRINT);
  REG_PORT_PB_COUNT |= _BV(BIT_PB_COUNT);
  
  // setup the interrupt
  
  // Uno
  /*
  EIMSK |= _BV(INT0); // built in INT0 constant
  EICRA |= _BV(ISC01); // FALLING edge setup
  EICRA &= ~_BV(ISC00); // FALLING edge setup
  */
  
  // Mega
  EIMSK |= _BV(INT4); // turns on INT4 external interrupt
  EICRB |= _BV(ISC41); // FALLING edge setup
  EICRB &= ~_BV(ISC40); // FALLING edge setup
  
  sei(); // turn on interrupts
}

void loop()
{
  if ( bit_is_clear(REG_PIN_PB_PRINT,BIT_PB_PRINT) ) {
    Serial.println(String("Counter = ") + String(isrCounter));
    delay(200);
  }

  if ( mainEventFlags & PB_FLAG) {
    delay(30);
    if ( bit_is_clear(REG_PIN_PB_COUNT, BIT_PB_COUNT) ) {
        isrCounter++;
    }

    mainEventFlags &= ~PB_FLAG;
  }

  // go to sleep
  
}


// macro interrupt function
// Uno
/*
ISR(INT0_vect) {
  isrCounter++;
}
*/

// Mega
ISR(INT4_vect) {
  mainEventFlags |= PB_FLAG;
}
