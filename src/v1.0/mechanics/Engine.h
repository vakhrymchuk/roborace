#pragma once

#include "Servo.h"
#include "Timeout.h"
#include "SpeedCorrector.h"

/**
 * Run a collector engine as a servo.
 */
class Engine {
public:

    static const int ENGINE_INIT_DELAY = 1500;

    explicit Engine(Servo *servo) : servo(servo) {
        stop();
        finishTimeout.start(ENGINE_INIT_DELAY);
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

    int getSpeed() const {
        return speedCorrector->getSpeed();
    }

    SpeedCorrector *speedCorrector = new SpeedCorrector();

private:
    Servo *servo;
    Timeout finishTimeout;

    void run(int power) const {
        if (!finishTimeout.isReady()) power = 0;

        servo->writeMicroseconds(DEFAULT_PULSE_WIDTH + speedCorrector->get(power));
    };

};
