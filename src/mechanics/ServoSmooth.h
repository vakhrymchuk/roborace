#pragma once

#include "ServoWrapperEsp32.h"

class ServoSmooth : public ServoWrapperEsp32 {
public:
    explicit ServoSmooth(byte pin, const Param *delta) : ServoWrapperEsp32(pin), delta(delta) {
    }

    void write(int value) {
        if (currentPosition != -1 && delta->value > 0) {
            value = constrain(value, currentPosition - delta->value, currentPosition + delta->value);
        }
        currentPosition = value;
        Servo::write(value);
    }

private:
    int currentPosition = -1;
    const Param *delta;

};

