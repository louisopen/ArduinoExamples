#include <Stepper.h>

const int stepsPerRevolution = 2048;  
// change this to fit the number of steps per revolution for your motor
 
// initialize the stepper library on pins 8 through 11:
//Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);
 
void setup() {
  myStepper.setSpeed(5);  //set the speed at 60 rpm: 
  Serial.begin(9600);   //initialize the serial port:
}
 
void loop() {
  // step one revolution  in one direction:
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(500);
 
  // step one revolution in the other direction:
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(500);
}

/*
void setup() {
  // nothing to do
}
void loop() {
  myStepper.setSpeed(1); // 1 rpm
  myStepper.step(2038); // do 2038 steps -- corresponds to one revolution in one minute
  delay(1000); // wait for one second
  myStepper.setSpeed(6); // 6 rpm
  myStepper.step(-2038); // do 2038 steps in the other direction with faster speed -- corresponds to one revolution in 10 seconds
}
*/