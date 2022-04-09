#pragma once

#include <ESP32Servo.h>

class ServoWrapperEsp32 : public Servo {
public:
    static bool timerAllocated;

    explicit ServoWrapperEsp32(byte pin, const int delta = 0) : Servo() {
        if (!timerAllocated) {
            ESP32PWM::allocateTimer(0);
            timerAllocated = true;
        }
        Servo::attach(pin);
    }
};

bool ServoWrapperEsp32::timerAllocated = false;

