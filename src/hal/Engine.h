#pragma once

#include <ESP32Servo.h>
#include <PIDController.h>
#include <Interval.h>
#include "Stopwatch.h"

#define ENCODER_PIN 34
#define POWER_SERVO_PIN 25

class Engine
{
public:
    static const int PID_INTERVAL = 40;
    static constexpr int TICKS_ON_METER = 282;
    static constexpr double TICKS_PER_INTERVAL_PER_METER = TICKS_ON_METER * PID_INTERVAL * 0.001;

    Engine()
    {

        int num = (byte)digitalPinToInterrupt(ENCODER_PIN);
        attachInterrupt(num, tick, CHANGE);

        servo.attach(POWER_SERVO_PIN);
        servo.writeMicroseconds(DEFAULT_PULSE_WIDTH);

        pid.tune(0.05, 0.02, 0);
        pid.limit(-60, 60);

        pidBack.tune(0.5, 0.003, 0.0);
        pidBack.limit(-100, 100);
    }

    void stop()
    {
        servo.writeMicroseconds(DEFAULT_PULSE_WIDTH);
    }

    void run(int speed)
    {
        if (interval.isReady() /*|| speed != lastSpeed*/)
        {

            speed = constrain(speed, -400, 500);

            calcSpeed();

            int power = 0;
            if (speed > 0)
            {
                if (lastSpeed < 0)
                {
                    directionChanged.start();
                }
                if (directionChanged.isLessThan(400))
                {
                    power = 80;
                    pid.resetLastTime();
                }
                else
                {
                    pid.setpoint(speed);
                    power = 80 + speed / 10 + (int)pid.compute(speedActual);
                }
            }
            else
            {
                if (lastSpeed >= 0)
                {
                    directionChanged.start();
                }
                if (directionChanged.isLessThan(400))
                {
                    power = -80;
                    pidBack.resetLastTime();
                }
                else
                {
                    pidBack.setpoint(-speed);
                    power = -(80 + (int)pidBack.compute(speedActual));
                }
            }

            servo.writeMicroseconds(DEFAULT_PULSE_WIDTH + power);
            lastSpeed = speed;
        }
    }

    int getSpeed() const
    {
        return speedActual;
    }

    double getOverallDistance() const
    {
        return (double)overallTicks / TICKS_ON_METER;
    }

    void resetOverallDistance()
    {
        overallTicks = 0;
    }

    PIDController pid;
    PIDController pidBack;

private:
    static volatile unsigned int ticks;

    Interval interval = Interval(PID_INTERVAL);

    Servo servo;

    int speedActual = 0;
    int lastSpeed = 0;
    Stopwatch directionChanged;
    unsigned long overallTicks = 0;
    unsigned long tickRefreshMs = 0;

    static void tick()
    {
        ticks++;
    }

    void calcSpeed()
    {
        overallTicks += ticks;
        unsigned long duration = getIntervalDuration();
        speedActual = (int)(100L * ticks * 1000 / (TICKS_ON_METER * duration));
        ticks = 0;
    }

    unsigned long getIntervalDuration()
    {
        unsigned long now = millis();
        unsigned long duration = now - tickRefreshMs;
        tickRefreshMs = now;
        return duration;
    }
};

volatile unsigned int Engine::ticks = 0;
