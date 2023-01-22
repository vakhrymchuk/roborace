#pragma once

#include <Servo.h>
#include <PIDController.h>
#include <Interval.h>

#define ENCODER_PIN 2
#define POWER_SERVO_PIN 9

class Engine {
public:

    static const int PID_INTERVAL = 40;
    static constexpr double TICKS_ON_METER = 282;
    static constexpr double TICKS_PER_INTERVAL_PER_METER = TICKS_ON_METER * PID_INTERVAL * 0.001;

    Engine() {

        int num = (byte) digitalPinToInterrupt(ENCODER_PIN);
        attachInterrupt(num, tick, CHANGE);

        servo.attach(POWER_SERVO_PIN);
        servo.writeMicroseconds(DEFAULT_PULSE_WIDTH);

        pid.begin();
        pid.tune(300.0, 1.5, 0.0);
        pid.limit(-300, 300);

        pidBack.begin();
        pidBack.tune(450.0, 2.0, 0.0);
        pidBack.limit(-200, 500);
    }

    void stop() {
        servo.writeMicroseconds(DEFAULT_PULSE_WIDTH);
    }

    void run(float speed) {
        if (interval.isReady() /*|| speed != lastSpeed*/) {

            speed = constrain(speed, -4.0, 5.0);

            int power;
            if (speed >= 0) {
                pid.setpoint(speed);
                power = 60 + (int) pid.compute(calcSpeed());
            } else {
                pidBack.setpoint(-speed);
                power = -(150 + (int) pidBack.compute(calcSpeed()));
            }

            servo.writeMicroseconds(DEFAULT_PULSE_WIDTH + power);
            lastSpeed = speed;
        }
    }

    double getSpeed() const {
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

    double speedActual = 0;
    double lastSpeed = 0;
    unsigned long overallTicks = 0;

    static void tick() {
        ticks++;
    }

    double calcSpeed() {
        overallTicks += ticks;
        speedActual = ticks / TICKS_PER_INTERVAL_PER_METER;
        ticks = 0;
        return speedActual;
    }
};

volatile unsigned int Engine::ticks = 0;
