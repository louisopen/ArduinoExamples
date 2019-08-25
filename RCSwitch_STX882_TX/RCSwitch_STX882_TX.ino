//*-- Sensor data by 433MHz --*//
#define SENSOR_ID0 0x33330000 //0xFFFF0000 address(16bits) + 0x0000FFFF data(16bits) for Ain6,Ain7
#define SENSOR_ID1 0x33320000 //0xFFFE0000 address(15bits) + 0x00010000 sign(1bit) + 0x0000FFFF data(16bits)  for temp,hum
#define SENSOR_ID2 0x33333000 //0xFFFFF000 address(20bits) + 0x0000FFFF data(12bits)
#define SENSOR_ID3 0x33333330 //0xFFFFFFF0 address(28bits) + 0x0000FFFF data(4bits)
#define OnBoardLED 13         //D13 for transmit
//#define DataBit 24            //24 bits shift out
#define DataBit 32            //32 bits shift out

//*-- ASK433 module --*//
#include <RCSwitch.h>
RCSwitch myRFTX = RCSwitch();    
#define RF_tx 2               //D2 for RF serial 
//RCSwitch mySwitch = RCSwitch(); 

//*-- DHT22 --*//
#include <Wire.h>OnBoardLED
#include "DHT.h"
#define _dhtpin   4           //D4
#define _dhttype  DHT22       //DHT 22 (AM2302), AM2321
//#define _dhttype DHT21      //DHT 21 (AM2301)
DHT dht22( _dhtpin, _dhttype );

#define _ain7     7           //A7
#define _ain6     6           //A6

#include <avr/wdt.h>          //For Watching Dog
#include <avr/sleep.h>        //For Sleep operation

unsigned long buf_first=0;    //long 類型有32bit，24/32才夠用或是位元旋轉才夠。
//unsigned long buf_second=0;   //long 類型有32bit，24/32才夠用或是位元旋轉才夠。

/**********************************************************/
/**********************************************************/
ISR(WDT_vect)   //watchdog interrupt
{
  wdt_disable(); // disable watchdog
}
/**********************************************************/
/**********************************************************/
void setup(){
  myRFTX.enableTransmit(2);    //发送端接D2号口（或其它口）  
  myRFTX.setProtocol(2);       //非Remote Control (default is 1, will work for most outlets)
  //myRFTX.setPulseLength(320); //Optional set pulse length(uS) 
  myRFTX.setRepeatTransmit(6); //Optional set number of transmission repetitions
  //mySwitch.enableReceive(0); //接收端接中斷INT0(D2),中斷INT1(D3)

  Serial.begin(9600);
  pinMode(OnBoardLED, OUTPUT );
  dht22.begin();
  pinMode(_ain6, INPUT);      //Arduino A0~A7 analog input(with NOPULL)
  pinMode(_ain7, INPUT);      //Arduino A0~A7 analog input(with NOPULL)
  Serial.println("Initial successful ID=0x5555 12bits");
}
/**********************************************************/
/**********************************************************/
void loop(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht22.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht22.readTemperature();    //+-0.5 (range -40~+80)  
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht22.readTemperature(true);
  if( isnan(h) || isnan(t)|| isnan(f))  //確認取回的溫溼度數據可用,若不!取ADC (若是沒有裝置DHT22)
  {
    Serial.print( "ReadADC:" );
    //Serial.println( "Failed to read form DHT22" );
    buf_first = SENSOR_ID0;
    unsigned long val6 = analogRead(_ain6);   //本應該unsigned int, 5V/1024=4.88mV (Rs=4.3K 1%)
    unsigned long val7 = analogRead(_ain7);   //本應該unsigned int, 5V/1024=4.88mV (Rs=4.3K 1%)

    buf_first = buf_first |(val6 << 8);  //10bit 5V/1024=4.88mV per. count
    buf_first &=0xFFFFFF00;
    buf_first = buf_first |(val7 >> 2);  //10bit to 8bit 5V/256=19mV per. count
  }
  else
  {
    Serial.print( "ReadDHT:" );
    buf_first = SENSOR_ID1;
    //DHT22 溫濕度數據儲存
    buf_first = buf_first |((uint8_t)t * 2) << 8;
    buf_first &=0xFFFFFF00;
    buf_first = buf_first |(uint8_t)h;
    if(t<0)
    {
      buf_first = buf_first | 0x00010000;   //mask & clear "-" sign flag
    }
  }
  Serial.println(buf_first,HEX);
  
  //buf = buf+1;
  digitalWrite(OnBoardLED,HIGH);    //RF data 發送期間亮起
  myRFTX.send(buf_first, DataBit);  //24/32bits Transmit & received test OK, 
  //See Example: TypeA_With DIP-Switches */
  //myRFTX.switchOff("11111", "00010");
  //Same switch as above, but using decimal code */
  //myRFTX.send(5393, 32);                              //24/32bits OK
  //Same switch as above, but using binary code */
  //myRFTX.send("00000000000000000001010100010001");    //24/32bits OK
  //Same switch as above, but tri-state code */ 
  //myRFTX.sendTriState("00000FFF0F0F");    //value=5393 
  //In this example it's family 'b', group #3, device #2 */
  //myRFTX.switchOn('b', 3, 2);
  digitalWrite(OnBoardLED,LOW);
  //delay(1000);
  goToSleep();
  //Serial.println("Wake up sleep");
}
/**********************************************************/
/**********************************************************/
void goToSleep()  //Go to sleep process
{ 
  byte old_ADCSRA = ADCSRA;
  ADCSRA = 0;     //disable ADC
  MCUSR = 0;      //clear various "reset" flags     
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
  
  //從睡眠模式醒來
  sleep_disable();
  
  ADCSRA = old_ADCSRA;
  
  dht22.begin();
  pinMode(_ain6, INPUT);    //Arduino A0~A7 analog input(with NOPULL)
  pinMode(_ain7, INPUT);    //Arduino A0~A7 analog input(with NOPULL)
}
