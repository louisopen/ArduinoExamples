#include <RCSwitch.h>
//RCSwitch myRFTX = RCSwitch();//定义发送端
RCSwitch mySwitch = RCSwitch();//定义接收端
unsigned long i = 0; //long 32bit，不能用int，不然发不了完整的24位数据。

void setup(){
  //myRFTX.enableTransmit(2);//发送端接6号口（或其它口）
  //Optional set protocol (default is 1, will work for most outlets) */
  //myRFTX.setProtocol(2);
  //Optional set pulse length.*/
  //myRFTX.setPulseLength(320);
  //Optional set number of transmission repetitions.*/
  //myRFTX.setRepeatTransmit(15);
  
  mySwitch.enableReceive(0);//接收端接中断0（中断0为Arduino的D2口，中断1为Arduino的D3口）
  Serial.begin(9600);//打开串口调试
  pinMode( 13, OUTPUT );
  Serial.println("Setup.");
}

void loop(){

  //发送端发送数据，i为数字，24为最大位数。经测试，当数字的位数大于24时，就会停止发送,不要連續發送至少間隔1sec
  //i = i+1;
  //myRFTX.send(i, 24);
  //See Example: TypeA_With DIP-Switches */
  //myRFTX.switchOff("11111", "00010");
  //Same switch as above, but using decimal code */
  //myRFTX.send(5393, 24);
  //Same switch as above, but using binary code */
  //myRFTX.send("000000000001010100010100");
  //Same switch as above, but tri-state code */ 
  //myRFTX.sendTriState("00000FFF0F0F");
  //In this example it's family 'b', group #3, device #2 */
  //myRFTX.switchOn('b', 3, 2);
  //delay(500);

  if (mySwitch.available()) {
    digitalWrite(13,HIGH);
    Serial.print("Got it: ");
    Serial.print(mySwitch.getReceivedBitlength());
    Serial.print("bits / Protocol: ");
    Serial.print(mySwitch.getReceivedProtocol());
    Serial.print(" / Data: ");
    unsigned long value = mySwitch.getReceivedValue();//接收端获取数据
    if (value != 0) { 
      Serial.print(value);
      Serial.print("=>0x"); 
      Serial.println(value,HEX);
    }
    mySwitch.resetAvailable();//接收端重设活动状态。估计上次接收后，就把该状态改了，所以这里要重新设置。
    digitalWrite(13,LOW);
  }
  delay(500);
}
