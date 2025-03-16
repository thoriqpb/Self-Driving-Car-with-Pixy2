#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>
#include "config.h"

int speed;

void setupMotors() {
  ledcSetup(MOTOR_A_CHANNEL, PWM_FREQ, PWM_RES);
  ledcSetup(MOTOR_B_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(MOTOR_A_PIN, MOTOR_A_CHANNEL);
  ledcAttachPin(MOTOR_B_PIN, MOTOR_B_CHANNEL);
}

void controlMotors(int deviation) {
  speed = map(deviation, 0, 40, 4095, 4000);
  speed = constrain(speed, 4000, 4095);
  ledcWrite(MOTOR_A_CHANNEL, speed);
  ledcWrite(MOTOR_B_CHANNEL, speed);
}

void stopMotors() {
  ledcWrite(MOTOR_A_CHANNEL, 0);
  ledcWrite(MOTOR_B_CHANNEL, 0);
}

#endif
