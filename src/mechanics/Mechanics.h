#pragma once

#include "Engine.h"
#include "VoltageDivider.h"
#include "value/Param.h"
#include "ServoSmooth.h"

#define TURN_SERVO_PIN GPIO_NUM_12
#define ENGINE_PIN GPIO_NUM_14
#define BATTERY_VOLTAGE_PIN GPIO_NUM_35

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
    Param *servoTurnDelta = new Param(10, "servo-turn-delta", "mechanics");

    VoltageDivider battery = VoltageDivider(BATTERY_VOLTAGE_PIN, 10);

    Engine *engine = new Engine(new ServoWrapperEsp32(ENGINE_PIN));

    ServoSmooth *turnServo = new ServoSmooth(TURN_SERVO_PIN, servoTurnDelta, turnCentralPosition);

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
