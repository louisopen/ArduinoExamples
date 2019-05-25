#include <Stepper.h>
#define IN1  8  //IN1 >> D8 of Arduino IO
#define IN2  9  //IN2 >> D9 of Arduino IO
#define IN3  10 //IN3 >> D10 of Arduino IO
#define IN4  11 //IN4 >> D11 of Arduino IO
//STEP MOTOR 5V(28BYJ-48)這是我們在空調機內機的風向馬達
//串口按鍵控制"1" 正反轉(低速)
//串口按鍵控制"2" 低速 
//串口按鍵控制"3" 中速
//串口按鍵控制"4" 高速
//串口按鍵控制" " 停止
//change this to fit the number of steps per revolution for your motor
const int stepsPerRevolution = 2048;  //the speed range 1~16 rpm
 
//initialize the stepper library on Arduino pins D8 through D11:
Stepper myStepper(stepsPerRevolution, IN1,IN3,IN2,IN4);

void setup() {
  myStepper.setSpeed(4); //Set speed (rpm) max.16 rpm
  Serial.begin(9600);     //initialize the serial port:
  Serial.println("Counter clock.");
}

boolean Clockwise=true;
boolean motor=true;
void loop() {   //do loop
  if(motor==true)
  {  
      if(Clockwise==true)
      {
        myStepper.step(100);
      }
      else
      {
        myStepper.step(-100);   
      }
  }
      
  if(Serial.available() > 0)  {
    char c = Serial.read();
    if(c == '1')  {   //Function Clockwise/Counter clock
      delay(1500);
      motor = true;
      if(Clockwise==true)
      {
        Clockwise=false;
        myStepper.setSpeed(4); //Set speed (rpm)
        Serial.println(String("")+"Clockwise. --" + c);  
        myStepper.step(-100);
      }
      else
      {
        Clockwise=true;
        myStepper.setSpeed(4); //Set speed (rpm)
        Serial.println(String("")+"Counter clock. --" + c);  
        myStepper.step(100);   
      }
    }else {
      if(c == '2')  {  
        Serial.println(String("")+"Low Speed. --" + c); 
        myStepper.setSpeed(8); //Set speed (rpm)
        motor = true;
      }else {       
        if(c == '3')  {  
          Serial.println(String("")+"Mid Speed. --" + c);  
          myStepper.setSpeed(12); //Set speed (rpm)
          motor = true;
        }else {
          if(c == '4')  {  
            Serial.println(String("")+"High Speed. --" + c);  
            myStepper.setSpeed(16); //Set speed (rpm)
            motor = true;
          }else {         
            if(c == ' ')  {  
              Serial.println(String("")+"STOP the motor. --" + c); 
              //myStepper.step(0);
              //GPIO state
              digitalWrite(IN1, LOW);   //Off the current output.
              digitalWrite(IN2, LOW);   //Off the current output.
              digitalWrite(IN3, LOW);   //Off the current output.
              digitalWrite(IN4, LOW);   //Off the current output.
              Clockwise = true;
              motor = false;
            }
          }
        }
      }
    }
  }
}
