#ifndef CONFIG_H
#define CONFIG_H

// Motor Control
#define MOTOR_A_PIN 21
#define MOTOR_B_PIN 22
#define MOTOR_A_CHANNEL 8
#define MOTOR_B_CHANNEL 9
#define PWM_FREQ 5000
#define PWM_RES 12

// Steering Servo
#define SERVO_PIN 4
#define SERVO_MIN_US 500
#define SERVO_MAX_US 2500

// Pixy2 Line Tracking
#define TARGET_X 39
#define MAX_TURN_ANGLE 45

// PID Constants
#define KP 1.0
#define KI 0.5
#define KD 0.03

#endif
