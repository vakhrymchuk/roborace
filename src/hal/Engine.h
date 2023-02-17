#pragma once

#include <Servo.h>
#include <PIDController.h>
#include <Interval.h>

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
        pid.tune(4.5, 0.0001, 0.0);
        pid.limit(-200, 400);

        pidBack.begin();
//        pidBack.minimize(10.0);
        pidBack.tune(20.0, 0.0001, 0.0);
        pidBack.limit(-200, 400);
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
                pid.setpoint(speed);
                if (lastSpeed < 0) pid.compute(speed);
                power = 60 + (int) pid.compute(speedActual);
            } else {
                pidBack.setpoint(-speed);
                if (lastSpeed >= 0) pid.compute(-speed);
                power = -(180 + (int) pidBack.compute(speedActual));
            }

            servo.writeMicroseconds(DEFAULT_PULSE_WIDTH + power);
            lastSpeed = speed;
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
