#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <ESP32Servo.h>
#include "config.h"

Servo steeringServo;

void setupServo() {
  steeringServo.attach(SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
}

#endif
