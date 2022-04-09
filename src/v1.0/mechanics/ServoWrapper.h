#pragma once

#include "Servo.h"

class ServoWrapper : public Servo {
public:
    ServoWrapper(byte pin, int centralPosition) : Servo() {
        Servo::attach(pin);
    }
};
