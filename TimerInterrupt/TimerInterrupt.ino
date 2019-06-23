
//每經過一段時間，就呼叫某回呼函式。
#include <Timer.h>

Timer tcb;

void writeToSerial(){
  static  unsigned long count = 0;
  Serial.println(count); // 從0開始輸出，每次加1
  count++;
}

void setup() {
  Serial.begin(115200);
  tcb.every(1000, writeToSerial); // 每經過1000毫秒，就會呼叫writeToSerial
}

void loop() {
  tcb.update();
}
