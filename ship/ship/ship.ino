/***************************************************************************/
// File       [final_project.ino]
// Author     [Erik Kuo]
// Synopsis   [Code for managing main process]
// Functions  [setup, loop, Search_Mode, Hault_Mode, SetState]
// Modify     [2020/03/27 Erik Kuo]
/***************************************************************************/

#define DEBUG // debug flag

#include <string.h>
#include <Servo.h>   //載入函式庫，這是內建的，不用安裝

Servo myservo;
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
// int IRpin_LL = 32;

int left_light = A14;
int center_light = A13;
int right_light = A12;
// RFID, 請按照自己車上的接線寫入腳位
/*===========================define pin & create module object===========================*/

/*============setup============*/
void setup()
{
  //bluetooth initialization
  Serial1.begin(9600);
  //Serial window
  Serial.begin(9600);
  //TB6612 pin
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  //tracking pin
  pinMode(left_light, INPUT);
  pinMode(center_light, INPUT);
  pinMode(right_light, INPUT);
  myservo.attach(9);
#ifdef DEBUG
  Serial.println("Start!");
#endif
}
/*============setup============*/

/*=====Import header files=====*/
#include "bluetooth.h"
#include <math.h>
/*=====Import header files=====*/

/*===========================initialize variables===========================*/
int l2=0,l1=0,m0=0,r1=0,r2=0; //紅外線模組的讀值(0->white,1->black)
int _Tp=90; //set your own value for motor power
int state=0; //set state to false to halt the car, set state to true to activate the car
BT_CMD _cmd = NOTHING; //enum for bluetooth message, reference in bluetooth.h line 2
byte idSize = 0;
int s[1000];
int count = 0;
int j = 0;
int l_speed = 50;//205
int r_speed = 50;//220
long long a_speed = 220;
long long b_speed = 220;
int banana = 210;
int apple = 100;
int accum = 1;
const int max_speed = 60;
int target_speed;
const double center_angle = 95;
const double max_angle = 40;
double current_angle = 0;
int current_speed = 0;
double target_angle = 0;
double AM, GM;
/*===========================initialize variables===========================*/

/*===========================declare function prototypes===========================*/
void Search();// search graph
void SetState(int l2,int l1,int m0,int r1,int r2);// switch the state
/*===========================declare function prototypes===========================*/

/*===========================define function===========================*/
void loop()
{
  // test();
  // send_byte(rfid(idSize), idSize);
  // myservo.write(center_angle);
  if (Serial1.available()) {
    state = Serial1.read();
  }
  if (state == 'c') {
    control_Mode();
  }
  else if (state == 'l') {
    light_mode();
  } else {
    stop();
  }
}

int sign = 1;

void control_Mode() {
  int x, y;
  while (true) {
    int tmp;
    if (Serial1.available()) {
      tmp = Serial1.read();
    } else continue;
    if (tmp == 's' || tmp == 'l') {
      state = tmp;
      return;
    }
    if (tmp == 'c') continue;
    tmp -= '1';
    x = tmp % 3 - 1;
    y = (tmp - x) / 3 - 1;
    if (x == 0) {
      target_angle = 0;
    } else {
      target_angle = x / abs(x) * max_angle / (1 + abs(y));
    }
    if (y == 1) {
      if (current_speed < max_speed) {
        current_speed += 8;
      }
    } else if (y == -1) {
      if (current_speed > -max_speed) {
        current_speed -= 8;
      }
    }
    if (current_speed >= 0) {
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, LOW);
      digitalWrite(BIN2, HIGH);
      analogWrite(PWMA, 1.1 * current_speed * (1 - target_angle / 60));
      analogWrite(PWMB, 0.9 * current_speed * (1 + target_angle / 60));
    } else {
      digitalWrite(AIN1, LOW);
      digitalWrite(AIN2, HIGH);
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
      analogWrite(PWMA, 1.1 * (-current_speed) * (1 - target_angle / 60));
      analogWrite(PWMB, 0.9 * (-current_speed) * (1 + target_angle / 60));
    }
    // if (current_angle < target_angle) {
    //   current_angle += 20;
    // } else if (current_angle > target_angle) {
    //   current_angle -= 20;
    // }
    myservo.write(center_angle + target_angle);
    delay(10);
    }
}

void light_mode() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  int left = analogRead(left_light);
  int center = analogRead(center_light);
  int right = analogRead(right_light);
  // AM = (left + right) / 2;
  // GM = sqrt(left * right);
  target_angle = max_angle * (left - right) / (left + right);
  // // target_angle = max_angle * (AM - GM) / (AM + GM);
  // // if (left < right) target_angle *= (-1);
  // if (target_angle > current_angle) {
  //   current_angle += 7;
  // } else {
  //   current_angle -= 7;
  // }
  if (current_speed < max_speed) {
    current_speed += 8;
  }
  analogWrite(PWMA, 1.1 * current_speed * (1 - target_angle / 60));
  analogWrite(PWMB, 0.9 * current_speed * (1 + target_angle / 60));
  myservo.write(center_angle + target_angle);
  delay(10);
}

int tmp = 'e';
// void test() {
//   while(true) {
//     if (Serial1.available()) tmp = Serial1.read();
//     if (tmp == 'e') stop();
//     if (tmp == 's') {
//       digitalWrite(AIN1, HIGH);
//       digitalWrite(AIN2, LOW);
//       digitalWrite(BIN1, LOW);
//       digitalWrite(BIN2, HIGH);
//       if (current_speed < max_speed) {
//         current_speed += 3;
//       }
//       if (current_angle > -max_angle) {
//         current_angle -= 3;
//       }
//       analogWrite(PWMA, current_speed);
//       analogWrite(PWMB, current_speed);
//       myservo.write(center_angle + current_angle);
//       Serial.println(current_speed);
//     }
//     delay(300);
//   }
// }

void test() {
  while(true) {
    if (Serial1.available()) tmp = Serial1.read();
    if (tmp == 'e') stop();
    if (tmp == 's') {
      digitalWrite(AIN1, HIGH);
      digitalWrite(AIN2, LOW);
      digitalWrite(BIN1, HIGH);
      digitalWrite(BIN2, LOW);
      int left = analogRead(left_light);
      int center = analogRead(center_light);
      int right = analogRead(right_light);
      AM = (left + right) / 2;
      GM = sqrt(left * right);
      target_angle = max_angle * (left - right) / (left + right);
      // target_angle = max_angle * (AM - GM) / (AM + GM);
      // if (left < right) target_angle *= (-1);
      if (target_angle > current_angle) {
        current_angle += 7;
      } else {
        current_angle -= 7;
      }
      if (current_speed < max_speed) {
        current_speed += 3;
      }
      analogWrite(PWMA, 1.1 * current_speed * (1 - target_angle / 90));
      analogWrite(PWMB, 0.9 * current_speed * (1 + target_angle / 90));
      myservo.write(center_angle + target_angle);
      delay(10);
    }
    delay(300);
  }
}

void SetState(int l2,int l1,int m0,int r1,int r2)
{
  // TODO:
  // 1. Get command from bluetooth
   
  // 2. Change state if need
  
}

// void Search()
// {
//   // TODO: let your car search graph(maze) according to bluetooth command from computer(python code)
// }

// void forward(double power) {
//   if (power > 0) {
//     a_speed += banana;
//     b_speed += 250;
//     analogWrite(PWMA, banana - 15);
//     analogWrite(PWMB, 255);
//     accum++;
//   } else if (power < 0) {
//     a_speed += 255;
//     b_speed += banana - 5;
//     analogWrite(PWMA, 255);
//     analogWrite(PWMB, banana - 20);
//     accum++; 
//   } else {
//     analogWrite(PWMA, a_speed / accum);
//     analogWrite(PWMB, b_speed / accum);
//   }
//   digitalWrite(AIN1, HIGH);
//   digitalWrite(AIN2, LOW);
//   digitalWrite(BIN1, HIGH);
//   digitalWrite(BIN2, LOW);
// }

// void backward() {
//   digitalWrite(AIN1, LOW);
//   digitalWrite(AIN2, HIGH);
//   digitalWrite(BIN1, LOW);
//   digitalWrite(BIN2, HIGH);
// }

// void turn_left() {
//   analogWrite(PWMA, 255);
//   analogWrite(PWMB, 88);
//   digitalWrite(AIN1, HIGH);
//   digitalWrite(AIN2, LOW);
//   digitalWrite(BIN1, HIGH);
//   digitalWrite(BIN2, LOW);
//   delay(900);
//   while (!(digitalRead(IRpin_M) || digitalRead(IRpin_R) || digitalRead(IRpin_RR))) {}
// }

// void turn_right() {
//   analogWrite(PWMA, 110);
//   analogWrite(PWMB, 255);
//   digitalWrite(AIN1, HIGH);
//   digitalWrite(AIN2, LOW);
//   digitalWrite(BIN1, HIGH);
//   digitalWrite(BIN2, LOW);
//   delay(900);
//   while (!(digitalRead(IRpin_LL) || digitalRead(IRpin_L) || digitalRead(IRpin_M))) {}
// }

// void u_turn_right() {
//   analogWrite(PWMA, 210);
//   analogWrite(PWMB, 210);
//   digitalWrite(AIN1, HIGH);
//   digitalWrite(AIN2, LOW);
//   digitalWrite(BIN1, LOW);
//   digitalWrite(BIN2, HIGH);
//   delay(250);
//   while (!( digitalRead(IRpin_LL) || digitalRead(IRpin_L) || digitalRead(IRpin_M))) {}
// }

void stop() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  current_speed = 0;
  current_angle = 0;
}

// /*===========================define function===========================*/