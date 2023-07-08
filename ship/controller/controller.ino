#include <stdio.h>

#define PIN_ANALOG_X 0
#define PIN_ANALOG_Y 1
#define BUTTON_UP 2
#define BUTTON_RIGHT 3
#define BUTTON_DOWN 4
#define BUTTON_LEFT 5
#define BUTTON_E 6
#define BUTTON_F 7

int state = 's';

void setup() {
 Serial.begin(9600);
 Serial1.begin(9600);
 pinMode(BUTTON_UP, INPUT);
 digitalWrite(BUTTON_UP, HIGH);
 pinMode(BUTTON_RIGHT, INPUT);
 digitalWrite(BUTTON_RIGHT, HIGH);
 pinMode(BUTTON_DOWN, INPUT);
 digitalWrite(BUTTON_DOWN, HIGH);
 pinMode(BUTTON_LEFT, INPUT);
 digitalWrite(BUTTON_LEFT, HIGH);
 pinMode(BUTTON_E, INPUT);
 digitalWrite(BUTTON_E, HIGH);
 pinMode(BUTTON_F, INPUT);
 digitalWrite(BUTTON_F, HIGH);
}
int tmp_x, tmp_y;
int x, y;
int pos;

void loop() {
 if(digitalRead(BUTTON_UP) == LOW) {
   state = 's';
   Serial.println(state);
 }
 else if(digitalRead(BUTTON_RIGHT) == LOW) {
   state = 'c';
   Serial.println(state);
 }
 else if(digitalRead(BUTTON_DOWN) == LOW) {
   state = 'l';
   Serial.println(state);
 }
 if (state == 'c') {
  tmp_x = analogRead(PIN_ANALOG_X) - 512;
  tmp_y = analogRead(PIN_ANALOG_Y) - 512;
  if (tmp_x < 200 && tmp_x > -200) {
    x = 1;
  } else {
    x = tmp_x < 0 ? 0 : 2;
  }
  if (tmp_y < 200 && tmp_y > -200) {
    y = 1;
  } else {
    y = tmp_y < 0 ? 0 : 2;
  }
  pos = '1' + x + y * 3;
  Serial.println(pos);
 }
 delay(500);
}