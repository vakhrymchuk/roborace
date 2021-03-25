#pragma once

#include <TimingSensor.h>

class TimingSerialSensor : public TimingSensor, DistanceSensorValue {
public:
    explicit TimingSerialSensor() : TimingSensor(nullptr) {}

    using DistanceSensorValue::getDistance;

    void setDistance(unsigned short distance) {
        value = distance;
        updateValue(distance);
    }
};