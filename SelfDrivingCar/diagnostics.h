#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

void printDiagnostics(int x_s, int x_e, int x_mid, int dev, int angle) {
  Serial.print("Start X: "); Serial.print(x_s);
  Serial.print("\t | End X: "); Serial.print(x_e);
  Serial.print("\t | Mid X: "); Serial.print(x_mid);
  Serial.print("\t  | Deviation: "); Serial.print(dev);
  Serial.print("\t  | Steering: "); Serial.print(angle);
  Serial.print("\t | Motor PWM: "); Serial.println(speed);
}

#endif
