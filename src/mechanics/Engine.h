#ifndef ROBORACE_ENGINE_H
#define ROBORACE_ENGINE_H

#include "PWMServo.h"
#include "EngineHelper.h"

/**
 * Run a collector engine as a servo.
 *
 *  _____            _
 * |  ___|          (_)
 * | |__ _ __   __ _ _ _ __   ___
 * |  __| '_ \ / _` | | '_ \ / _ \
 * | |__| | | | (_| | | | | |  __/
 * \____/_| |_|\__, |_|_| |_|\___|
 *              __/ |
 *             |___/
 */
class Engine {
public:

    Engine(PWMServo *servo) : servo(servo) {
        stop();
    }

    void forward(int power) {
        run(power);
    }

    void backward(int power) {
        run(-power);
    }

    void stop() {
        run(0);
    }

    int getSpeed() {
        return engineHelper->getSpeed();
    }

    EngineHelper *engineHelper = new EngineHelper();

private:
    PWMServo *servo;

    void run(int power) {
        servo->writeMicroseconds(DEFAULT_PULSE_WIDTH + engineHelper->get(power));
    };

};

#endif
