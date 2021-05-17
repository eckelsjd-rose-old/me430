#include <LiquidCrystal.h>

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

volatile uint8_t greenCounter = 0;
volatile uint8_t yellowCounter = 0;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("Joshua Eckels");
  
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
  
  sei(); // turn on interrupts
}

void loop() {
  
  // handle green button press
  if ( mainEventFlags & PB_GREEN_FLAG) {
    delay(30); // debounce
    if ( bit_is_clear(REG_PIN_PB_GREEN, BIT_PB_GREEN)) {
      greenCounter++;
      
      // turn green on
      REG_PORT_LED_GREEN |= _BV(BIT_LED_GREEN);
      // turn yellow off
      REG_PORT_LED_YELLOW &= ~_BV(BIT_LED_YELLOW);
    }
    
    mainEventFlags &= ~PB_GREEN_FLAG;
    
  }
  
  // handle yellow button press
  if ( mainEventFlags & PB_YELLOW_FLAG) {
    delay(30); // debounce
    if ( bit_is_clear(REG_PIN_PB_YELLOW, BIT_PB_YELLOW)) {
      yellowCounter++;
      
      // turn green off
      REG_PORT_LED_GREEN &= ~_BV(BIT_LED_GREEN);
      // turn yellow on
      REG_PORT_LED_YELLOW |= _BV(BIT_LED_YELLOW);
    }
    
    mainEventFlags &= ~PB_YELLOW_FLAG;
    
  }
  
  // handle reset button press
  if ( mainEventFlags & PB_RESET_FLAG) {
    delay(30); // debounce
    if ( bit_is_clear(REG_PIN_PB_RESET, BIT_PB_RESET)) {
      yellowCounter = 0;
      greenCounter = 0;
      
      // turn green off
      REG_PORT_LED_GREEN &= ~_BV(BIT_LED_GREEN);
      // turn yellow off
      REG_PORT_LED_YELLOW &= ~_BV(BIT_LED_YELLOW);
      lcd.setCursor(0,1);
      lcd.print("G=0  Y=0  T=");
    }
    
    mainEventFlags &= ~PB_RESET_FLAG;
    
  }
  
  updateLcd();
}

void updateLcd() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  lcd.print("G=");
  lcd.print(greenCounter);
  
  lcd.setCursor(5,1);
  lcd.print("Y=");
  lcd.print(yellowCounter);
  
  lcd.setCursor(10,1);
  lcd.print("T=");
  lcd.print(millis() / 1000);
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
 