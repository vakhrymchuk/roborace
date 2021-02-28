#pragma once

#include <ESP32Servo.h>

class ServoWrapper : public Servo {
public:
    explicit ServoWrapper(byte pin) : Servo() {
        ESP32PWM::allocateTimer(0);
        Servo::attach(pin);
    }
};

