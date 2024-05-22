#pragma once

#include <Servo.h>
#include <Interval.h>
#include "Stopwatch.h"

#define POWER_SERVO_PIN 9

class EngineServoConfig
{
public:
    EngineServoConfig()
    {
        servo.attach(POWER_SERVO_PIN);
        servo.writeMicroseconds(DEFAULT_PULSE_WIDTH);
        Serial.begin(115200);
        Serial.println("Enter speed from 700 to 2300, 1500 is default");
    }

    void loop()
    {
        if(Serial.available()) {
            int speed = Serial.parseInt();
            Serial.print("speed=");
            Serial.println(speed);
            if(speed >= 700 && speed <= 2300)
              servo.writeMicroseconds(speed);
            Serial.readString();
        }
    }

private:
    Servo servo;

    int speed = 0;
};
