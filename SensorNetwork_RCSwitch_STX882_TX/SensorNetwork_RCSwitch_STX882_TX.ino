//*-- Sensor data by 433MHz --*//
#define SENSOR_ID0 0x33300000 //0xFFFE0000 address(15bits) + 0x0000FFFF data(16bits) for Ain6,Ain7
#define SENSOR_ID1 0x33320000 //0xFFFE0000 address(15bits) + 0x00010000 sign bit(1bit) + 0x0000FFFF data(16bits) for Temp & Hum
#define SENSOR_ID2 0x33340000 //0xFFFE0000 address(15bits) + 0x0000FFFF data(16bits)
#define SENSOR_ID3 0x333330   //0xFFFFF0   address(20bits) + 0xF data(4bits) for keyfob
//#define DataBit 24            //24 bits shift out standar application for Key-chan or keyfob
#define DataBit 32            //32 bits shift out
#define OnBoardLED 13         //D13 for transmit

//*-- ASK433 module --*//
#include <RCSwitch.h>
RCSwitch myRFTX = RCSwitch();    
#define RF_tx 2               //D2 for RF serial 
//RCSwitch mySwitch = RCSwitch(); 

//*-- DHT22 --*//
#include <Wire.h>OnBoardLED
#include "DHT.h"
#define _dhtpin   A3          //D4
#define _dhttype  DHT22       //DHT 22 (AM2302), AM2321
//#define _dhttype DHT21      //DHT 21 (AM2301)
DHT dht22( _dhtpin, _dhttype );

#define _ain7     7           //A7
#define _ain6     6           //A6

#include <avr/wdt.h>          //For Watching Dog
#include <avr/sleep.h>        //For Sleep operation
#include <avr/power.h>        //For Sleep operation

unsigned long buf_first=0;    //long 類型有32bit，24/32才夠用或是位元旋轉才夠。
//unsigned long buf_second=0;   //long 類型有32bit，24/32才夠用或是位元旋轉才夠。
volatile int flag_timer=0;
/**********************************************************/
/**********************************************************/
ISR(WDT_vect)   //watchdog interrupt
{
  wdt_disable(); // disable watchdog
}
/**********************************************************/
ISR(TIMER1_OVF_vect){
   if(flag_timer == 0) //設定旗標,與主迴圈交換資訊
   {
     flag_timer = 1;
   }
}
/**********************************************************/
void setup_timer(){ //timer for wakeup from sleep mode
  /* Normal timer operation.*/
  TCCR1A = 0x00;   
  /* Clear the timer counter register.
   * You can pre-load this register with a value in order to 
   * reduce the timeout period, say if you wanted to wake up
   * ever 4.0 seconds exactly. */
  TCNT1=0x0000;   //清除計算器 
  /* Configure the prescaler for 1:1024, giving us a 
   * timeout of 4.09 seconds. */
  //TCCR1B = 0x01;  //clk/1 設置中斷計數值 Timer1's
  //TCCR1B = 0x02;  //clk/8 設置中斷計數值 Timer1's
  //TCCR1B = 0x03;  //clk/64 設置中斷計數值 Timer1's
  //TCCR1B = 0x04;  //clk/256 設置中斷計數值 Timer1's 1.02 seconds
  TCCR1B = 0x05;  //clk/1024 設置中斷計數值 Timer1's 4.09 seconds
  /* Enable the timer overlow interrupt. */
  TIMSK1=0x01;    //清除中斷旗標
}
/**********************************************************/
void init_application(){
  myRFTX.enableTransmit(2);    //发送端接D2号口（或其它口）  
  myRFTX.setProtocol(2);       //非Remote Control (default is 1, will work for most outlets)
  //myRFTX.setPulseLength(320); //Optional set pulse length(uS) 
  myRFTX.setRepeatTransmit(6); //Optional set number of transmission repetitions
  //mySwitch.enableReceive(0); //接收端接中斷INT0(D2),中斷INT1(D3)

  dht22.begin();
}
/**********************************************************/
/**********************************************************/
/**********************************************************/
/**********************************************************/
void setup(){
  Serial.begin(9600);
  pinMode(OnBoardLED, OUTPUT );
  Serial.println("Initial successful ID=0x3333(12bits):function+Sign(4bits):data(16bits)");

  setup_timer();
  
  init_application();
}
/**********************************************************/
/**********************************************************/
void loop(){
  if(flag_timer==1)
  {
    flag_timer = 0;
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht22.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht22.readTemperature();    //+-0.5 (range -40~+80)  
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht22.readTemperature(true);
    if( isnan(h) || isnan(t)|| isnan(f))  //確認取回的溫溼度數據可用,若不!取ADC (若是沒有裝置DHT22)
    {
      Serial.println("");
      Serial.print( "ReadADC:" );
      //Serial.println( "Failed to read form DHT22" );
      buf_first = SENSOR_ID0;
      unsigned long val6 = analogRead(_ain6);   //本應該unsigned int, 5V/1024=4.88mV (Rs=4.3K 1%)
      unsigned long val7 = analogRead(_ain7);   //本應該unsigned int, 5V/1024=4.88mV (Rs=4.3K 1%)

      //buf_first = buf_first |(val6 << 8);  //10bit 5V/1024=4.88mV per. count 
      buf_first |= (val6 << 7);  //10bit to 9bit 5V/512 (1024)=9.76mV/per. count  (只能取最高9位)
      buf_first &=0xFFFFFF00;
      buf_first |= (val7 >> 2);  //10bit to 8bit 5V/256=19mV/per. count  取最高8位
    }
    else
    {
      Serial.print( "ReadDHT:" );
      buf_first = SENSOR_ID1;
      //DHT22 溫濕度數據儲存
      uint8_t x=0; 
      if(t<0)
      {      
        x= 0.0 - t*10; 
        //Serial.print( x );
        //Serial.print("  ");
        buf_first |= (x%512) << 7;    //取最高9位
        buf_first |= 0x00010000;   //mask & clear "-" sign flag
      }
      else
      {
        x=t*10; 
        //Serial.print( x );
        //Serial.print("  ");
        buf_first |= (x%512) << 7;    //取最高9位         
      }
      buf_first &= 0xFFFFFF80;
      buf_first |= (uint8_t)h & 0x7F; //取最高7位   
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
    wdt_reset();    //Clear Watching dog
    //delay(1000);
    enterSleep();
    //Serial.println("Wake up sleep");
    init_application();
  }
}
/**********************************************************/
/**********************************************************/
/**********************************************************/
void enterSleep(void){
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  /* Disable all of the unused peripherals. This will reduce power
   * consumption further and, more importantly, some of these
   * peripherals may generate interrupts that will wake our Arduino from
   * sleep!   */
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer2_disable();
  power_twi_disable();  
  /* Now enter sleep mode. */
  sleep_mode(); 
  
  /* The program will continue from here after the timer timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  /* Re-enable the peripherals. */
  power_all_enable();
}
/**********************************************************/
/**********************************************************/
