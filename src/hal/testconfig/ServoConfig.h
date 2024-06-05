#pragma once

#include <ESP32Servo.h>
#include <Interval.h>
#include "Stopwatch.h"

// #define SERVO_PIN 26
#define SERVO_PIN 27

class ServoConfig
{
public:
    ServoConfig()
    {
        servo.attach(SERVO_PIN);
        servo.writeMicroseconds(DEFAULT_PULSE_WIDTH);
        Serial.begin(115200);
        Serial.println("Enter speed from 0 to 180, 90 is default");
    }

    void loop()
    {
        if (Serial.available())
        {
            int angle = Serial.parseInt();
            Serial.readString();
            Serial.print("angle=");
            Serial.println(angle);
            if (angle >= 0 && angle <= 180)
                servo.write(angle);
        }
    }

private:
    Servo servo;
};
