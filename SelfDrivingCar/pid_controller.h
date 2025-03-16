#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include "config.h"

float integral = 0;
float prev_error = 0;
unsigned long last_time = 0;

int calculateSteeringAngle(int error) {
  unsigned long current_time = millis();
  float dt = (current_time - last_time) / 1000.0f;

  // PID Calculations
  float P = KP * error;
  
  if (abs(error) < 10) integral += error * dt;
  if (abs(error) < 3) integral = 0;
  integral = constrain(integral, -5, 5);
  float I = KI * integral;
  
  float D = KD * ((error - prev_error) / dt);
  
  int output = 90 - (P + I + D);
  output = constrain(output, 0, 180);

  prev_error = error;
  last_time = current_time;
  
  return output;
}

#endif
