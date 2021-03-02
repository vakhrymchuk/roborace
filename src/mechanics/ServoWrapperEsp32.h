#pragma once

#include <ESP32Servo.h>

class ServoWrapperEsp32 : public Servo {
public:
    explicit ServoWrapperEsp32(byte pin) : Servo() {
        if (!timerAllocated) {
            ESP32PWM::allocateTimer(0);
            timerAllocated = true;
        }
        Servo::attach(pin);
    }

private:
    bool timerAllocated = false;

};

