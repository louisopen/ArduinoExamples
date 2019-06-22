//*-- Sensor data by 433MHz --*//
#define SENSOR_ID  0x55
#define LED 13        //D13

//*-- ASK433 module --*//
#include <RCSwitch.h>
RCSwitch myRFTX = RCSwitch();     //定义发送端
//RCSwitch mySwitch = RCSwitch(); //定义接收端

//*-- DHT22 --*//
#include <Wire.h>
#include "DHT.h"
#define _dhtpin   4     //D4
#define _dhttype  DHT22
DHT dht22( _dhtpin, _dhttype );

#define _ain7     7     //A7
#define _ain6     6     //A6

unsigned long buf = 0;  //long 類型有32bit，24/32才夠用或是位元旋轉才夠。
//char strbuf[9];       //For show

void setup(){
  myRFTX.enableTransmit(2);   //发送端接D2号口（或其它口）
  //Optional set protocol (default is 1, will work for most outlets)
  myRFTX.setProtocol(2);      //非Remote Control
  //Optional set pulse length(uS)
  //myRFTX.setPulseLength(320);
  //Optional set number of transmission repetitions
  myRFTX.setRepeatTransmit(6);
 
  //mySwitch.enableReceive(0);//接收端接中斷INT0(D2),中斷INT1(D3)

  Serial.begin(9600);//打开串口调试
  dht22.begin();
  pinMode(_ain7, INPUT);   // Arduino A0~A7 analog input(with NOPULL)
  pinMode(LED, OUTPUT );
}

void loop(){
  buf = 0x55000000; 
  unsigned long val7 = analogRead(_ain7);   //本應該unsigned int, 5V/1024=4.88mV
  buf = buf |(val7 << 14);  //10bit to 8bit 5V/256=19mV
  buf = buf & 0xFFFF0000;   //mask & clear 
  
  float h = dht22.readHumidity();
  float t = dht22.readTemperature();
  //確認取回的溫溼度數據可用
  if( isnan(h) || isnan(t) )
  {
    Serial.println( "Failed to read form DHT22" );
  }
  else
  {
      //DHT22 溫濕度數據儲存
      buf = buf |((uint8_t)t << 8);
      buf = buf |(uint8_t)h;
  }
  
  //sprintf(strbuf, "%08x", buf);
  Serial.println(buf,HEX);
  
  //buf = buf+1;
  digitalWrite(LED,HIGH);
  //发送端发送数据，i为数字long，24/32为最大位数。经测试，当数字的位数大于24时，就会停止发送,不要連續發送至少間隔1sec
  myRFTX.send(buf, 32);                                 //24/32bits OK
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
  digitalWrite(LED,LOW);
  delay(1000);
}
