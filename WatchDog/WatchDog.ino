#include <avr/wdt.h>
void setup()
{
   pinMode(13,OUTPUT);
   wdt_enable(WDTO_2S); //开启看门狗，并设置溢出时间为两秒
}
  
void loop()
{
   digitalWrite(13,HIGH);
   delay(100);
   digitalWrite(13,LOW);
   delay(100);
   wdt_reset(); //喂狗操作，使看门狗定时器复位
}
