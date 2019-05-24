#include <EEPROM.h>
int burnAddress=0;
int address = 0;  //0~511, total 512 block can store
byte valueByte;   //max 255

void listvalueByte(){ //ATMEGA 8 - 128 have 512 byte miniHD space
  Serial.println("");
  for(int i=0;i<5;i++){
    Serial.print("NO.");
    Serial.print(i);
    Serial.print(" valueByte=");
    Serial.print(EEPROM.read(i), DEC);
    Serial.print("; ");
  }
  delay(15);
}
/*
void clearROM()
{
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);  
  delay(30);
}
*/

void setup()   {
  Serial.begin(9600);    
  valueByte = EEPROM.read(burnAddress);
  valueByte += 1;
  EEPROM.write(burnAddress, valueByte);  
}
void loop()                    
{
  listvalueByte();
}
