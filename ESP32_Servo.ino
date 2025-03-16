#include <ESP32Servo.h>

Servo myServo; 

void setup() {
    myServo.attach(4, 500, 2500); 
}

void loop() {
    myServo.write(0);
    delay(500);
    myServo.write(90);
    delay(500);
    myServo.write(180);
    delay(500);
    myServo.write(90);
    delay(500);        
}
