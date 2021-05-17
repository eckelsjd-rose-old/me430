// Note: Arduino pin 13 = Mega 2560 PORTB7
//               pin 6 = PORTH3

#define REG_DDR_LED DDRB
#define REG_PORT_LED PORTB
#define REG_PIN_LED PINB

#define BIT_LED 7

void setup()
{
  // pinMode(13,OUTPUT);
  // DDRB |= 0b00100000; // Set pin 13 as an output (bit 5)
  // DDRB |= 0x20;
  // DDRB |= (1 << 5);
  // DDRB |= _BV(5); // built in macro for the previous line
  
  REG_DDR_LED |= _BV(BIT_LED);
  
}

void loop()
{
  // digitalWrite(13, HIGH);
  REG_PORT_LED |= _BV(BIT_LED);
  
  delay(1000); // Wait for 1000 millisecond(s)
  
  // digitalWrite(13, LOW);
  REG_PORT_LED &= ~_BV(BIT_LED);
  
  delay(1000); // Wait for 1000 millisecond(s)
}
