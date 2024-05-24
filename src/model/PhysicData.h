#pragma once

#include "LaserScan.h"

class PhysicalData
{
public:
    LaserScan &scan;
    int speed;
    int yaw;
    int pitch;
    int roll;
    int absoluteAngle;
};
