#pragma once

#include <PWMServo.h>

class ServoWrapperPWM : public PWMServo {
public:
    ServoWrapperPWM(byte pin) : PWMServo() {
        PWMServo::attach(pin);
    }
};
