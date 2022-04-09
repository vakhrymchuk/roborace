#pragma once

#include "Engine.h"
#include "VoltageDivider.h"
#include "v1.0/value/Param.h"
#include "ServoWrapper.h"

#define BACK_SERVO_PIN 11
#define TURN_SERVO_PIN 10
#define ENGINE_PIN 9
#define BATTERY_VOLTAGE_PIN A7

class Mechanics {
public:
    /** Максимальный угол поворота */
    static const int TURN_MAX_ANGLE = 40;
    static const int FULL_RIGHT = -TURN_MAX_ANGLE;
    static const int FULL_LEFT = TURN_MAX_ANGLE;

    Param *servoEnabled = new Param(1, "servo-enabled", "mechanics");
    Param *powerEnabled = new Param(1, "power-enabled", "mechanics");

    Param *turnMaxAngle = new Param(TURN_MAX_ANGLE, "servo-max-turn", "mechanics");
    Param *turnCentralPosition = new Param(90, "servo-center", "mechanics");
//    Param *servoTurnDelta = new Param(10, "servo-turn-delta", "mechanics");

    VoltageDivider battery = VoltageDivider(BATTERY_VOLTAGE_PIN, 10);

    Engine *engine = new Engine(new ServoWrapper(ENGINE_PIN, 1500));

    ServoWrapper *turnServo = new ServoWrapper(TURN_SERVO_PIN, 90);

    Mechanics() {
        stop();
    }

    void stop() {
        run(0, 0);
    }

    void init() const {
        engine->init();
    }

    void run(int angle, int power) {
        turnWheels(angle);
        if (!powerEnabled->value) power = 0;
        engine->forward(power);
    }

private:
    void turnWheels(int angle) const {
        if (!servoEnabled->value) angle = 0;

        int a = constrain(angle, -turnMaxAngle->value, turnMaxAngle->value);
        turnServo->write(turnCentralPosition->value + a);
    }

};
