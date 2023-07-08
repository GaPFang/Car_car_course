#include <SPI.h>
#include <MFRC522.h>
#include <string.h>
#include "bluetooth.h"
/*===========================define pin & create module object================================*/
// BlueTooth
// BT connect to Serial1 (Hardware Serial)
// Mega               HC05
// Pin  (Function)    Pin
// 18    TX       ->  RX
// 19    RX       <-  TX
// TB6612, 請按照自己車上的接線寫入腳位(左右不一定要跟註解寫的一樣)
#define AIN1      2 //定義 A1 接腳（右）
#define AIN2      3 //定義 A2 接腳（右）
#define PWMA    11//定義 ENA (PWM調速) 接腳
#define BIN1      5 //定義 B1 接腳（左）
#define BIN2      6 //定義 B2 接腳（左）
#define PWMB    12//定義 ENB (PWM調速) 接腳
// 循線模組, 請按照自己車上的接線寫入腳位
int IRpin_LL = 32;
int IRpin_L  = 34;
int IRpin_M  = 36;
int IRpin_R  = 38;
int IRpin_RR = 40;
// RFID, 請按照自己車上的接線寫入腳位
#define RST_PIN      9        // 讀卡機的重置腳位
#define SS_PIN       53       // 晶片選擇腳位
MFRC522 mfrc522(SS_PIN, RST_PIN);  // 建立MFRC522物件
/*===========================define pin & create module object===========================*/

/*============setup============*/
int state = false;

void setup()
{
   //bluetooth initialization
   Serial1.begin(9600);
   //Serial window
   Serial.begin(9600);
   //RFID initial
   SPI.begin();
   mfrc522.PCD_Init();
   //TB6612 pin
   pinMode(AIN1, OUTPUT);
   pinMode(AIN2, OUTPUT);
   pinMode(BIN1, OUTPUT);
   pinMode(BIN2, OUTPUT);
   pinMode(PWMA, OUTPUT);
   pinMode(PWMB, OUTPUT);
   //tracking pin
   pinMode(IRpin_LL, INPUT); 
   pinMode(IRpin_L, INPUT);
   pinMode(IRpin_M, INPUT);
   pinMode(IRpin_R, INPUT);
   pinMode(IRpin_RR, INPUT);
#ifdef DEBUG
  Serial.println("Start!");
#endif
}

int l_speed = 0;//205
int r_speed = 0;//220

void loop() {
  if (Serial1.available()) {
    int tmp = Serial1.read();
    if (tmp == 's') {
      state = true;
    } else if (tmp == 'e') {
      state = false;
    }
  }
  if (state) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMA, 0);
    analogWrite(PWMB, 0);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
  }
}