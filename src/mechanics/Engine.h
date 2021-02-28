#ifndef ROBORACE_ENGINE_H
#define ROBORACE_ENGINE_H

#include "ServoWrapper.h"
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

    static const int ENGINE_INIT_DELAY = 1500;

    explicit Engine(ServoWrapper *servo) : servo(servo) {
        stop();
        finishInit = millis() + ENGINE_INIT_DELAY;
    }

    void init() const {
        stop();
    }

    void forward(int power) {
        run(power);
    }

    void backward(int power) {
        run(-power);
    }

    void stop() const {
        run(0);
    }

    int getSpeed() {
        return engineHelper->getSpeed();
    }

    EngineHelper *engineHelper = new EngineHelper();

private:
    ServoWrapper *servo;
    unsigned long finishInit;

    void run(int power) const {
        if (millis() < finishInit) power = 0;

        servo->writeMicroseconds(DEFAULT_PULSE_WIDTH + engineHelper->get(power));
    };

};

#endif
