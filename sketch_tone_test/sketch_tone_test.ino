int buzzerPin=9;  //D9 conectted to a buzzer

void setup() {
 pinMode(buzzerPin, OUTPUT);
}

void loop() {
  //alarmBeep(buzzerPin);
  //alarmClockBeep(buzzerPin);
  ringTone(buzzerPin);
}
///////////////////////////////////////////////////////////
void alarmBeep(int pin) {
  tone(pin, 1000, 1000);
  delay(2000); 
}
///////////////////////////////////////////////////////////
void alarmClockBeep(int pin) {
  tone(pin, 1000, 125);
  delay(125);
  //noTone(pin);
  tone(pin, 1000, 125);
  delay(125);
  //noTone(pin);
  tone(pin, 1000, 125);
  delay(125);
  //noTone(pin);
  tone(pin, 1000, 125); 
  delay(125);
  //noTone(pin);
  delay(1500); 
} 
///////////////////////////////////////////////////////////
void ringTone(int pin) {
  for (int i=0; i<10; i++) { //repeat 10 times
    tone(pin, 1000);
    delay(50);
    tone(pin, 500);
    delay(50);
    }
  noTone(pin);
  delay(2000);
} 
