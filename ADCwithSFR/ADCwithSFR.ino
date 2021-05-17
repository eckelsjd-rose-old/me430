#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  // TODO: Setup the ADC using ADCSRA and ADMUX
  // 1. Enable the ADC
  ADCSRA |= _BV(ADEN);
  // 2. Set the ref voltage for what 1023 equals
  //    - use the AVCC with external capacitor at AREF pin option
  ADMUX |= _BV(REFS0);
  ADMUX &= ~_BV(REFS1);
  
}

void loop() {
  lcd.setCursor(0,0);
  lcd.print("ADC 0 = ");
  lcd.print(getADC0());
  lcd.print("    ");
  
  lcd.setCursor(0,1);
  lcd.print("ACD 15 = ");
  lcd.print(getADC15());
  lcd.print("      ");
  
}

uint16_t getADC0() {
  // analogRead(0);
  
  // Select the pin using the MUX3 to MUX0 bits
  ADCSRB &= ~_BV(MUX5);
  ADMUX &= ~_BV(MUX4);
  ADMUX &= ~_BV(MUX3);
  ADMUX &= ~_BV(MUX2);
  ADMUX &= ~_BV(MUX1);
  ADMUX &= ~_BV(MUX0);
  
  // Start the conversion by setting a bit
  ADCSRA |= _BV(ADSC);
  
  // Do nothing until conversion is done
  while ( !bit_is_clear(ADCSRA,ADSC) ) {
    // do nothing
  }
  // return the result
  return ADC;
}

uint16_t getADC15() {
  // analogRead(3);
  
  // Select the pin using the MUX3 to MUX0 bits
  ADCSRB |= _BV(MUX5);
  ADMUX &= ~_BV(MUX4);
  ADMUX &= ~_BV(MUX3);
  ADMUX |= _BV(MUX2);
  ADMUX |= _BV(MUX1);
  ADMUX |= _BV(MUX0);
  
  // Start the conversion by setting a bit
  ADCSRA |= _BV(ADSC);
  
  // Do nothing until conversion is done
  while ( !bit_is_clear(ADCSRA,ADSC) ) {
    // do nothing
  }
  // return the result
  return ADC;
  
  
}
 