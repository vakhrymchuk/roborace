#pragma once

#include "ServoWrapperEsp32.h"

class ServoSmooth : public ServoWrapperEsp32 {
public:
    explicit ServoSmooth(byte pin, const Param *delta, const Param *turnCentralPosition) :
            ServoWrapperEsp32(pin),
            delta(delta),
            turnCentralPosition(turnCentralPosition) {
    }


    void write(int value) {
        if (currentPosition != -1 && delta->value > 0) {

            int minAngle = currentPosition - delta->value;
            int maxAngle = currentPosition + delta->value;

            if (currentPosition < turnCentralPosition->value) {
                maxAngle = maxInt(maxAngle, turnCentralPosition->value);
            } else if (currentPosition > turnCentralPosition->value) {
                minAngle = minInt(minAngle, turnCentralPosition->value);
            }

            value = constrain(value, minAngle, maxAngle);
        }
        currentPosition = value;
        Servo::write(value);
    }

private:
    int currentPosition = -1;
    const Param *delta;
    const Param *turnCentralPosition;

    static int maxInt(int x, int y) {
        return x > y ? x : y;
    }

    static int minInt(int x, int y) {
        return x < y ? x : y;
    }

};

