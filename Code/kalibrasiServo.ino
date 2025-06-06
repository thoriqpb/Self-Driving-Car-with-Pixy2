#include <ESP32Servo.h>

// Create three Servo objects
Servo myServo33; // steer
Servo myServo14; // tengah
Servo myServo15; //

void setup() {
    // Attach each servo to its GPIO pin with proper pulse width
    myServo33.attach(33, 500, 2500); // Pin 33
    myServo14.attach(14, 500, 2500); // Pin 14
    myServo15.attach(15, 500, 2500); // Pin 15
}

void loop() {
    // Move servo 15 gradually from 0 to 180 degrees
    for(int pos = 45; pos <= 135; pos += 1) { // goes from 0 to 180 degrees
        myServo15.write(pos);
        myServo14.write(pos);                // tell servo to go to position
        myServo33.write(pos);                // tell servo to go to position
                // tell servo to go to position
        delay(15);                           // waits 15ms for the servo to reach the position
    }
    
    // Then move it back from 180 to 0 degrees
    for(int pos = 135; pos >= 45; pos -= 1) { // goes from 180 to 0 degrees
        myServo15.write(pos);
        myServo14.write(pos);                // tell servo to go to position
        myServo33.write(pos);                 // tell servo to go to position
        delay(15);                           // waits 15ms for the servo to reach the position
    }
}