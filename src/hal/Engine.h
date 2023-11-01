#pragma once

#include <Servo.h>
#include <PIDController.h>
#include <Interval.h>
#include "Stopwatch.h"

#define ENCODER_PIN 2
#define POWER_SERVO_PIN 9

class Engine {
public:

    static const int PID_INTERVAL = 40;
    static constexpr int TICKS_ON_METER = 282;
    static constexpr double TICKS_PER_INTERVAL_PER_METER = TICKS_ON_METER * PID_INTERVAL * 0.001;

    Engine() {

        int num = (byte) digitalPinToInterrupt(ENCODER_PIN);
        attachInterrupt(num, tick, CHANGE);

        servo.attach(POWER_SERVO_PIN);
        servo.writeMicroseconds(DEFAULT_PULSE_WIDTH);

        pid.begin();
//        pid.minimize(10.0);
        pid.tune(6.0, 2.0 / PID_INTERVAL, 0.0);
        pid.limit(-100, 200);

        pidBack.begin();
//        pidBack.minimize(10.0);
        pidBack.tune(3.0, 1.5 / PID_INTERVAL, 0.0);
        pidBack.limit(-100, 100);


//        Serial.println("P,I,speed,actual");
    }

    void stop() {
        servo.writeMicroseconds(DEFAULT_PULSE_WIDTH);
    }

    void run(int speed) {
        if (interval.isReady() /*|| speed != lastSpeed*/) {

            speed = constrain(speed, -400, 500);

            calcSpeed();

            int power;
            if (speed >= 0) {
                if (lastSpeed < 0) {
                    directionChanged.start();
                }
                if (directionChanged.isLessThan(400)) {
                    power = 60;
                    pid.resetLastTime();
                } else {
                    pid.setpoint(speed);
                    power = 60 + (int) pid.compute(speedActual);
//                    if(speedActual > speed + 20) power = 0;
                }
            } else {
                if (lastSpeed >= 0) {
                    directionChanged.start();
                }
                if (directionChanged.isLessThan(400)) {
                    power = -80;
                    pidBack.resetLastTime();
                } else {
                    pidBack.setpoint(-speed);
                    power = -(80 + (int) pidBack.compute(speedActual));
                }
            }

            servo.writeMicroseconds(DEFAULT_PULSE_WIDTH + power);
            lastSpeed = speed;

//            Serial.print(abs(speed));
//            Serial.print(',');
//            Serial.print(speedActual);
//            Serial.println();
        }
    }

    int getSpeed() const {
        return speedActual;
    }

    double getOverallDistance() const {
        return (double) overallTicks / TICKS_ON_METER;
    }

    void resetOverallDistance() {
        overallTicks = 0;
    }

private:
    static volatile unsigned int ticks;

    Interval interval = Interval(PID_INTERVAL);
    PIDController pid;
    PIDController pidBack;

    Servo servo;

    int speedActual = 0;
    int lastSpeed = 0;
    Stopwatch directionChanged;
    unsigned long overallTicks = 0;
    unsigned long tickRefreshMs = 0;

    static void tick() {
        ticks++;
    }

    void calcSpeed() {
        overallTicks += ticks;
        unsigned long duration = getIntervalDuration();
        speedActual = (int) (100L * ticks * 1000 / (TICKS_ON_METER * duration));
        ticks = 0;
    }

    unsigned long getIntervalDuration() {
        unsigned long now = millis();
        unsigned long duration = now - tickRefreshMs;
        tickRefreshMs = now;
        return duration;
    }
};

volatile unsigned int Engine::ticks = 0;
