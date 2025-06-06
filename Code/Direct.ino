// my final code
#include <ESP32Servo.h>
#include <Pixy2.h>

// =============== Hardware Configuration ===============
#define MOTOR_PIN1 25    // Motor A IN1
#define MOTOR_PIN2 26    // Motor A IN2
#define MOTOR_PIN3 27    // Motor B IN1
#define MOTOR_PIN4 32    // Motor B IN2

const int SERVO_PIN = 33;
const int SERVO_MIN_US = 500;
const int SERVO_MAX_US = 2500;

// =============== Pixy2 Line Tracking ===============
const int TARGET_X = 39;        // Centerline position
const int MAX_TURN_ANGLE = 45;  // Maximum steering angle

// =============== PID Constants ===============
float Kp = 1;
float Ki = 0.7;
float Kd = 0.02;

// =============== Global Variables ===============
Pixy2 pixy;
Servo steeringServo;
Servo myServo14;    // Vertical neck servo
Servo myServo15;    // Horizontal neck servo

float integral = 0;
float prev_error = 0;
unsigned long last_time = 0;

float smooth_neck_angle = 90;  // Start at center

// =============== Setup ===============
void setup() {
  Serial.begin(115200);
  Serial.println("Initializing self-driving car...");

  pixy.init();

  steeringServo.attach(SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
  myServo14.attach(14, 500, 2500);
  myServo15.attach(15, 500, 2500);

  // Motor pins
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  pinMode(MOTOR_PIN3, OUTPUT);
  pinMode(MOTOR_PIN4, OUTPUT);

  stopMotors();
  Serial.println("System ready!");
}

// =============== Main Loop ===============
void loop() {
  myServo14.write(25);  // Fixed vertical neck (optional, can change later)

  pixy.line.getMainFeatures();

  if (pixy.line.numVectors > 0) {
    processLineData();
  } else {
    stopMotors();
  }
}

// =============== Process Line Data ===============
void processLineData() {
  int x0 = pixy.line.vectors[0].m_x0;
  int y0 = pixy.line.vectors[0].m_y0;
  int x1 = pixy.line.vectors[0].m_x1;
  int y1 = pixy.line.vectors[0].m_y1;

  int y_mid = (y0 + y1) / 2;
  int x_mid = x0 + ((x1 - x0) * (y_mid - y0)) / (y1 - y0);

  int deviation = x_mid - TARGET_X;
  int steering_angle = calculateSteeringAngle(deviation);

    // Control steering
  // Control steering
  steeringServo.write(steering_angle);

  // Control motors
  // controlMotors(abs(deviation));

  // ===== Horizontal Neck Tracking Follows Steering =====
  float alpha = 0.3;  // Smoothing factor (0.0 - 1.0)
  smooth_neck_angle = smooth_neck_angle + alpha * (steering_angle - smooth_neck_angle);

  myServo15.write((int)smooth_neck_angle);

  printDiagnostics(x0, x1, x_mid, deviation, steering_angle);
}

// =============== PID Calculation ===============
int calculateSteeringAngle(int error) {
  unsigned long now = millis();
  float dt = (now - last_time) / 1000.0;

  float P = Kp * error;
  if (abs(error) < 10) integral += error * dt;
  if (abs(error) < 3) integral = 0;
  integral = constrain(integral, -5, 5);
  float I = Ki * integral;

  float derivative = (error - prev_error) / dt;
  float D = Kd * derivative;

  int output = 90 - (P + I + D);
  output = constrain(output, 0, 180);

  prev_error = error;
  last_time = now;

  return output;
}

// =============== Motor Control ==================
void controlMotors(int deviation) {
  int speed = map(deviation, 0, 40, 100, 150);  // PWM range (tune as needed)
  speed = constrain(speed, 100, 150);

  // Motor A: Forward
  analogWrite(MOTOR_PIN1, speed);
  digitalWrite(MOTOR_PIN2, LOW);

  // Motor B: Forward
  analogWrite(MOTOR_PIN3, speed);
  digitalWrite(MOTOR_PIN4, LOW);

  Serial.print("Motor Speed: ");
  Serial.println(speed);
}

void stopMotors() {
  // delay(500);
  analogWrite(MOTOR_PIN1, 0);
  digitalWrite(MOTOR_PIN2, LOW);
  analogWrite(MOTOR_PIN3, 0);
  digitalWrite(MOTOR_PIN4, LOW);

  Serial.println("Motors stopped");
}

// =============== Diagnostics ==================
void printDiagnostics(int x0, int x1, int x_mid, int dev, int angle) {
  Serial.print("Start X: "); Serial.print(x0);
  Serial.print(" | End X: "); Serial.print(x1);
  Serial.print(" | Mid X: "); Serial.print(x_mid);
  Serial.print(" | Deviation: "); Serial.print(dev);
  Serial.print(" | Steering: "); Serial.print(angle);
  Serial.print(" | Neck Angle: "); Serial.println(smooth_neck_angle);
}