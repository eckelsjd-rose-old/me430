#include <LiquidCrystal.h>

#define DEBOUNCE_MS 30
volatile  uint16_t delayCounter = 0;

// Green timer to have interrupts every 0.001 seconds via Timer 2
// Set TIMSK2 to have OCIE2A high - enables timer 2 int
// Prescaler options:
// 1 --> 0.001 / 1/16000000 --> 16000 ticks for 0.001 seconds
// 8 --> 0.001 / (8/16000000) --> 2000 ticks
// 32 --> 0.001 / (32/16000000) --> 500 ticks
// 64 --> 250 ticks to 0.001 seconds (less than 255)--> use this one
// 128 --> 125
// 256 --> 62.5
// 1024 -->
#define TIMER_2_START 0
#define TIMER_2_END 250

// Yellow timer 0.1 seconds Timer 1
// Timer 1 is 16 bits
// Use prescaler of 64 -> 0.1 / (64/16000000) --> 25000 ticks for 0.1 seconds
#define TIMER_1_START 0
#define TIMER_1_END 25000 

// uno
/*
#define REG_DDR_PB_GREEN DDRD
#define REG_PORT_PB_GREEN PORTD
#define REG_PIN_PB_GREEN PIND
#define BIT_PB_GREEN 2

#define REG_DDR_PB_YELLOW DDRD
#define REG_PORT_PB_YELLOW PORTD
#define REG_PIN_PB_YELLOW PIND
#define BIT_PB_YELLOW 3

#define REG_DDR_PB_RESET DDRD
#define REG_PORT_PB_RESET PORTD
#define REG_PIN_PB_RESET PIND
#define BIT_PB_RESET 4

#define REG_DDR_LED_GREEN DDRD
#define REG_PORT_LED_GREEN PORTD
#define REG_PIN_LED_GREEN PIND
#define BIT_LED_GREEN 6

#define REG_DDR_LED_YELLOW DDRD
#define REG_PORT_LED_YELLOW PORTD
#define REG_PIN_LED_YELLOW PIND
#define BIT_LED_YELLOW 7
*/

// Mega 2560
#define REG_DDR_PB_GREEN DDRD
#define REG_PORT_PB_GREEN PORTD
#define REG_PIN_PB_GREEN PIND
#define BIT_PB_GREEN 2

#define REG_DDR_PB_YELLOW DDRD
#define REG_PORT_PB_YELLOW PORTD
#define REG_PIN_PB_YELLOW PIND
#define BIT_PB_YELLOW 3

#define REG_DDR_PB_RESET DDRD
#define REG_PORT_PB_RESET PORTD
#define REG_PIN_PB_RESET PIND
#define BIT_PB_RESET 0

#define REG_DDR_LED_GREEN DDRH
#define REG_PORT_LED_GREEN PORTH
#define REG_PIN_LED_GREEN PINH
#define BIT_LED_GREEN 3

#define REG_DDR_LED_YELLOW DDRH
#define REG_PORT_LED_YELLOW PORTH
#define REG_PIN_LED_YELLOW PINH
#define BIT_LED_YELLOW 4

// Main event flags
volatile uint8_t mainEventFlags = 0;
#define PB_GREEN_FLAG 0x01
#define PB_YELLOW_FLAG 0x02
#define PB_RESET_FLAG 0x04


// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

// Use Timer 2 (green timer) with 0.001 interrupts
bool isGreenTimerRunning = false;
volatile uint16_t greenCounterMilliSec = 0;

// Use Timer 1 (yellow timer) with 0.1 interrupt
bool isYellowTimerRunning = false;
volatile uint16_t yellowCounterTenthSec = 0;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  // inputs and outputs
  REG_DDR_PB_GREEN &= ~_BV(BIT_PB_GREEN); // input
  REG_PORT_PB_GREEN |= _BV(BIT_PB_GREEN); // pullup resistor
  
  REG_DDR_PB_YELLOW &= ~_BV(BIT_PB_YELLOW);
  REG_PORT_PB_YELLOW |= _BV(BIT_PB_YELLOW);
  
  REG_DDR_PB_RESET &= ~_BV(BIT_PB_RESET);
  REG_PORT_PB_RESET |= _BV(BIT_PB_RESET);
  
  REG_DDR_PB_RESET &= ~_BV(BIT_PB_RESET);
  REG_PORT_PB_RESET |= _BV(BIT_PB_RESET);
  
  REG_DDR_LED_GREEN |= _BV(BIT_LED_GREEN); // output
  REG_DDR_LED_YELLOW |= _BV(BIT_LED_YELLOW);
  
  // setup the interrupts
  /* Uno
  EIMSK |= _BV(INT0); // green pushbutton interrupt
  EICRA |= _BV(ISC01); // FALLING edge setup
  EICRA &= ~_BV(ISC00); 
  
  EIMSK |= _BV(INT1); // yellow pushbutton interrupt
  EICRA |= _BV(ISC11); // FALLING edge setup
  EICRA &= ~_BV(ISC10);
  */
  
  // Mega
  EIMSK |= _BV(INT2); // green pb
  EICRA |= _BV(ISC21); 
  EICRA &= ~_BV(ISC20); 
  
  EIMSK |= _BV(INT3); // yellow pb
  EICRA |= _BV(ISC31); 
  EICRA &= ~_BV(ISC30);
  
  EIMSK |= _BV(INT0); // reset pb
  EICRA |= _BV(ISC01); 
  EICRA &= ~_BV(ISC00); 

  // Timer 2 interrupt setup
  TIMSK2 |= _BV(OCIE2A);
  TCCR2A = 0; // Clear things that Arduino did
  // Prescaler of 64 -> 250 ticks to 0.001 seconds
  TCCR2B |= _BV(CS22);
  TCCR2B &= ~_BV(CS21);
  TCCR2B &= ~_BV(CS20);
  TCNT2 = TIMER_2_START;
  OCR2A = TIMER_2_END;
  
  // Timer 1 setup
  TIMSK1 |= _BV(OCIE1A);
  TCCR1A = 0;
  TCCR1B &= ~_BV(CS12);
  TCCR1B |= _BV(CS11);
  TCCR1B |= _BV(CS10);
  TCNT1 = TIMER_1_START;
  OCR1A = TIMER_1_END;
  
  sei(); // turn on interrupts
}

void loop() {
  
  // handle green button press
  if ( mainEventFlags & PB_GREEN_FLAG) {
    // delay(30); // debounce
    blockingDelay(DEBOUNCE_MS);
    
    if ( bit_is_clear(REG_PIN_PB_GREEN, BIT_PB_GREEN)) {
      
      // Toggle timer
      isGreenTimerRunning = !isGreenTimerRunning;
      REG_PORT_LED_GREEN ^= _BV(BIT_LED_GREEN);
      
    }
    
    mainEventFlags &= ~PB_GREEN_FLAG;
    
  }
  
  // handle yellow button press
  if ( mainEventFlags & PB_YELLOW_FLAG) {
    // delay(30); // debounce
    blockingDelay(DEBOUNCE_MS);
    
    if ( bit_is_clear(REG_PIN_PB_YELLOW, BIT_PB_YELLOW)) {

      // Toggle timer
      isYellowTimerRunning = !isYellowTimerRunning;
      REG_PORT_LED_YELLOW ^= _BV(BIT_LED_YELLOW);
    }
    
    mainEventFlags &= ~PB_YELLOW_FLAG;
    
  }
  
  // handle reset button press
  if ( mainEventFlags & PB_RESET_FLAG) {
    //delay(30); // debounce
    blockingDelay(DEBOUNCE_MS);
    
    if ( bit_is_clear(REG_PIN_PB_RESET, BIT_PB_RESET)) {
        isGreenTimerRunning = false;
        greenCounterMilliSec = 0;
        
        isYellowTimerRunning = false;
        yellowCounterTenthSec = 0;
    }
    
    mainEventFlags &= ~PB_RESET_FLAG;
    
  }
  
  updateLcd();
}

void blockingDelay(uint16_t delay_ms) {
  delayCounter = delay_ms;
  while (delayCounter > 0) {
    // do nothing
  }
}

void updateLcd() {
  lcd.setCursor(0,0);
  uint8_t greenSeconds = greenCounterMilliSec/1000;
  uint8_t greenTenthSec = (greenCounterMilliSec / 100) % 10;
  lcd.print(greenSeconds);
  lcd.print(".");
  lcd.print(greenTenthSec);
  lcd.print("    ");
  
  lcd.setCursor(0,1);
  
  if (isYellowTimerRunning) {
    lcd.print("           ");
  } else {
    uint8_t yellowSeconds = yellowCounterTenthSec/10;
    uint8_t yellowTenthSec = yellowCounterTenthSec%10;
    lcd.print(yellowSeconds);
    lcd.print(".");
    lcd.print(yellowTenthSec);
    lcd.print("   ");
  }
}

// macro interrupt functions
/* Uno
ISR(INT0_vect) {
  mainEventFlags |= PB_GREEN_FLAG;
}

ISR(INT1_vect) {
  mainEventFlags |= PB_YELLOW_FLAG;
}
*/

// Mega
ISR(INT2_vect) {
  mainEventFlags |= PB_GREEN_FLAG;
}

ISR(INT3_vect) {
  mainEventFlags |= PB_YELLOW_FLAG;
}

ISR(INT0_vect) {
  mainEventFlags |= PB_RESET_FLAG;
}

// Timer Interrupts
ISR(TIMER2_COMPA_vect) {
  TCNT2 = TIMER_2_START;
  if (isGreenTimerRunning) {
    greenCounterMilliSec++;
  }
  
  if (delayCounter > 0) {
    delayCounter--;
  }
}

ISR(TIMER1_COMPA_vect) {
  TCNT1 = TIMER_1_START;
  if (isYellowTimerRunning) {
    yellowCounterTenthSec++;
  }
}
 