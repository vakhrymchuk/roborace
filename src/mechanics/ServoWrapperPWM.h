#ifndef ROBORACE_SERVOWRAPPERPWM_H
#define ROBORACE_SERVOWRAPPERPWM_H

#include <PWMServo.h>

class ServoWrapperPWM : public PWMServo {
public:
    ServoWrapperPWM(byte pin) : PWMServo() {
        PWMServo::attach(pin);
    }
};

#endif
