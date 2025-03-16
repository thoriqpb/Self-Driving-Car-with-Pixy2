#ifndef PIXY2_UTILS_H
#define PIXY2_UTILS_H

#include <Pixy2.h>
#include "config.h"

Pixy2 pixy;

struct LineData {
  int x_start;
  int x_end;
  int x_mid;
  int deviation;
};

void setupPixy() {
  pixy.init();
}

bool getLineData(LineData &data) {
  pixy.line.getMainFeatures();
  if (pixy.line.numVectors > 0) {
    data.x_start = pixy.line.vectors[0].m_x0;
    data.x_end = pixy.line.vectors[0].m_x1;
    int y_start = pixy.line.vectors[0].m_y0;
    int y_end = pixy.line.vectors[0].m_y1;

    int y_mid = (y_start + y_end) / 2;
    data.x_mid = data.x_start + ((data.x_end - data.x_start) * (y_mid - y_start)) / (y_end - y_start);
    data.deviation = data.x_mid - TARGET_X;
    return true;
  }
  return false;
}

#endif
