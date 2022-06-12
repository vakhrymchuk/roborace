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
        pid.tune(600.0, 1.5, 0.0);
        pid.limit(-500, 600);

        pidBack.begin();
        pidBack.tune(500.0, 2.0, 0.0);
        pidBack.limit(-20, 400);

        ticks = 0;
    }

    void run(float speed) {
        if (interval.isReady()) {

            speed = constrain(speed, -4.0, 5.0);

            int compute;
            int power;
            if (speed >= 0) {
                pid.setpoint(speed);
                compute = (int) pid.compute(calcSpeed());
                power = 50 + compute;
            } else {
                pidBack.setpoint(-speed);
                compute = (int) pidBack.compute(calcSpeed());
                power = -(160 + compute);
            }

            servo.writeMicroseconds(DEFAULT_PULSE_WIDTH + power);
        }
    }

    double getSpeed() const {
        return speedActual;
    }

private:
    static volatile unsigned int ticks;

    Interval interval = Interval(PID_INTERVAL);
    PIDController pid;
    PIDController pidBack;

    Servo servo;

    double speedActual = 0;

    static void tick() {
        ticks++;
    }

    double calcSpeed() {
        speedActual = ticks / TICKS_PER_INTERVAL_PER_METER;
        ticks = 0;
        return speedActual;
    }
};

volatile unsigned int Engine::ticks = 0;
