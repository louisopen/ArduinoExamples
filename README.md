## Arduino 一些測試範例
### EEPROM SleepAndWDT TimerInterrupt WatchDog (Arduino samples)
### sketch_tone_test (Sound Alarm or Beep)
> Ring tone

> Alarm

> Beep

### StepperMotor
> Stepper motor driver by Stepper library

每次走一小步stepping motor 進行無限迴圈,迴圈中進行串口指令讀取"鍵"值,按鍵"1"每按一次反轉一次並低速運轉,按鍵"2"低速運轉,按鍵"3"中速運轉,按鍵"4"高速運轉,按鍵" "停止運轉

### SteppingMotor
> Stepper motor deriver by IO only

### RCSwitch application (Get ADC & DHT22 applications RX/TX base ASK 433MHz)
>Base on RCSwitch.h library

>RCSwitch_SRX887_RX  for examples

>SensorNetwork_RCSwitch_STX882_TX for a TX application, it is a sensor net work on wireless 433MHz,it work on sleep mode so very low power base on Arduino Pro mini 5V/16MHz
傳感器取自DHT22模組,若是沒有連接此模組,本程序會自A6 A7針腳取得類比電壓(我設計一個3K的提升電阻),取得之資訊後利用其STX822 433MHz模組將此資料單向傳送出去,本程序運行在低耗電版本的Arduino Pro mini 5V/16MHz

本程序休眠時間4秒(Sleep time around 4 second in this program)