#pragma once

#include "Engine.h"
#include "VoltageDivider.h"

#define TURN_SERVO_PIN GPIO_NUM_12
#define ENGINE_PIN GPIO_NUM_14

/**
 * ___  ___          _                 _
 * |  \/  |         | |               (_)
 * | .  . | ___  ___| |__   __ _ _ __  _  ___ ___
 * | |\/| |/ _ \/ __| '_ \ / _` | '_ \| |/ __/ __|
 * | |  | |  __/ (__| | | | (_| | | | | | (__\__ \
 * \_|  |_/\___|\___|_| |_|\__,_|_| |_|_|\___|___/
 *
 */
class Mechanics {
public:
    /** Максимальный угол поворота */
    static const int TURN_MAX_ANGLE = 38;
    static const int FULL_RIGHT = -TURN_MAX_ANGLE;
    static const int FULL_LEFT = TURN_MAX_ANGLE;

    ValueInt *turnMaxAngle = new ValueInt(TURN_MAX_ANGLE);
    ValueInt *turnCentralPosition = new ValueInt(94); // 93

    VoltageDivider battery = VoltageDivider(GPIO_NUM_2, 10);

    Mechanics() {
        stop();
    }

    Engine *engine = new Engine(new ServoWrapper(ENGINE_PIN));
    ServoWrapper *turnServo = new ServoWrapper(TURN_SERVO_PIN);

    void stop() {
        run(0, 0);
    }

    void init() const {
        engine->init();
    }

    void run(int angle, int power) {
        turnWheels(angle);
        engine->forward(power);
    }

    void turnWheels(int angle) {
        int a = constrain(angle, -turnMaxAngle->value, turnMaxAngle->value);
        turnServo->write(turnCentralPosition->value + a);
    }

    void forward(int power) {
        engine->forward(power);
    }

    void backward(int power) {
        engine->backward(power);
    }
};
