#include "config.h"
#include "motor_control.h"
#include "servo_control.h"
#include "pid_controller.h"
#include "pixy2_utils.h"
#include "diagnostics.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  setupPixy();
  setupServo();
  setupMotors();
}

void loop() {
  LineData line_data;
  
  if (getLineData(line_data)) {
    int steering_angle = calculateSteeringAngle(line_data.deviation);
    steeringServo.write(steering_angle);
    controlMotors(abs(line_data.deviation));
    printDiagnostics(line_data.x_start, line_data.x_end, line_data.x_mid, line_data.deviation, steering_angle);
  } else {
    stopMotors();
  }
}
