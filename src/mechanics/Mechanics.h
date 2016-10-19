#ifndef ROBORACE_MECHANICS_H
#define ROBORACE_MECHANICS_H

#include "Engine.h"

#define TURN_SERVO_PIN SERVO_PIN_B // 10
#define ENGINE_PIN SERVO_PIN_A // 9

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
    static const int TURN_MAX_ANGLE = 35;

    ValueInt *turnMaxAngle = new ValueInt(TURN_MAX_ANGLE);
    ValueInt *turnCentralPosition = new ValueInt(93);

    Mechanics() {
        stop();
    }

    Engine *engine = new Engine(new ServoWrapperPWM(ENGINE_PIN));
    ServoWrapperPWM *turnServo = new ServoWrapperPWM(TURN_SERVO_PIN);

    void stop() {
        run(0, 0);
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

#endif
