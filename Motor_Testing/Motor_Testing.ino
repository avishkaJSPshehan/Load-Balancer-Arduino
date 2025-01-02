#include <Servo.h>

Servo Left_Servo;
Servo Right_Servo;

void setup() {
  Left_Servo.attach(9);
  Right_Servo.attach(10);
  Serial.begin(9600);
}

void loop() {

  for(int angle = 0; angle <= 180; angle++){
    Left_Servo.write(angle);
    Right_Servo.write(angle);
    delay(100);
    Serial.println(angle);
  }


}
