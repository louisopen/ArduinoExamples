/*
*/
#include <avr/sleep.h>
#include <avr/wdt.h>

ISR(WDT_vect)   //watchdog interrupt
{
  wdt_disable(); // disable watchdog
}
void gotoSleep()  //Go to sleep process
{ 
  byte old_ADCSRA = ADCSRA;
  ADCSRA = 0;     //disable ADC
  MCUSR = 0;       
  //allow changes, disable reset
  WDTCSR = bit (WDCE) | bit (WDE);
  //set interrupt mode and an interval 
  WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
  wdt_reset();
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
  noInterrupts();           
  sleep_enable();
 
  //turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 
  interrupts();             
  sleep_cpu();  
  sleep_disable();
  
  ADCSRA = old_ADCSRA;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Initialisation complete.");
}

void loop()
{
  digitalWrite(13, !digitalRead(13));
  gotoSleep();
}
