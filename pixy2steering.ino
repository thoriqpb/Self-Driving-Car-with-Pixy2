#include <ESP32Servo.h>
#include <Pixy2.h>
#define IN2 21  // Motor A input 2 (IN1 is GND)
#define IN4 22  // Motor B input 2 (IN3 is GND)

Pixy2 pixy;
Servo myServo;

const int servoPin = 4;
const int x_center = 39;        // Target position
const int max_turn_angle = 45;  // Max wheel turning angle

void setup() {
  pinMode(IN2, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Initialize the motor speed to prevent error on driver motor
  analogWrite(IN2, 0);  
  analogWrite(IN4, 0);  

  Serial.println("Moving Forward at Half Speed...");
  Serial.begin(115200);
  pixy.init();
  myServo.attach(servoPin, 500, 2500);  // Minimum and Maximum Pulse Width
}

void loop() {
  pixy.line.getMainFeatures();
  if (pixy.line.numVectors > 0) {
    int x_s = pixy.line.vectors[0].m_x0;
    int y_s = pixy.line.vectors[0].m_y0;
    int x_e = pixy.line.vectors[0].m_x1;
    int y_e = pixy.line.vectors[0].m_y1;

    // Calculate x position at reference y (e.g., midpoint)
    int y_r = (y_s + y_e) / 2;
    int x_r = x_s + ((x_e - x_s) * (y_r - y_s)) / (y_e - y_s);

    // Deviation from center
    int delta_x = x_r - x_center;

    // Map deviation to servo angle
    int turn_angle = constrain(90 - (delta_x * max_turn_angle / 40), 0, 180);

    myServo.write(turn_angle);
    controlMotors(delta_x);

    Serial.print("x_s: ");
    Serial.print(x_s);
    Serial.print(" | x_e: ");
    Serial.print(x_e);
    Serial.print(" | x_r: ");
    Serial.print(x_r);
    Serial.print(" | delta_x: ");
    Serial.print(delta_x);
    Serial.print(" | Servo: ");
    Serial.print(turn_angle);
  } else {
    analogWrite(IN2, 0);  
    analogWrite(IN4, 0);  
  }
}

void controlMotors(int delta_x) {
  int speed = map(abs(delta_x), 0, 40, 255, 50);  // More deviation → lower speed
  speed = constrain(speed, 150, 255);              // Ensure speed stays in valid range

  analogWrite(IN2, speed);
  analogWrite(IN4, speed);

  Serial.print(" | Motor Speed: ");
  Serial.println(speed);
}
