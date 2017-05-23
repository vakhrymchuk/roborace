#ifndef ROBORACE_BACKWARD_H
#define ROBORACE_BACKWARD_H

#include "Strategy.h"

/**
 * ______  ___  _____  _   ___    _  ___  ____________
 * | ___ \/ _ \/  __ \| | / / |  | |/ _ \ | ___ \  _  \
 * | |_/ / /_\ \ /  \/| |/ /| |  | / /_\ \| |_/ / | | |
 * | ___ \  _  | |    |    \| |/\| |  _  ||    /| | | |
 * | |_/ / | | | \__/\| |\  \  /\  / | | || |\ \| |/ /
 * \____/\_| |_/\____/\_| \_/\/  \/\_| |_/\_| \_|___/
 */
class Backward : public Strategy {
public:

    ValueInt *backwardSpeed = new ValueInt(80);

    virtual Strategy *init(unsigned int minMs = 0) final override {
        Strategy::init(minMs);
//        minTimeout->start(500);
        maxTimeout->start(2000);
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final override {
        if (minTimeout->isReady()) {
            if (maxTimeout->isReady() || isBackFinish(sensors)) {
                return forward->init();
            }
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final override {
        if (stopwatch->isLessThan(200) || stopwatch->isMoreThan(1200)) {
            angle = 0;
        } else {
            if (angle == 0) {
                angle = -getAngle(sensors->rightDistance, sensors->leftDistance);
                angle = minAngle(angle, 10);
            }
        }

        if (stopwatch->isMoreThan(1200)) {
            power = backwardSpeed->value + 20;
        } else {
            power = backwardSpeed->value;
        }
        power *= -1;
    }

    Strategy *forward;

private:

    Timeout *maxTimeout = new Timeout();

    bool isBackFinish(SensorsHolder *sensors) const {
        return (sensors->minForwardDistance > 35) || (sensors->minDistance > 25 && sensors->maxDistance > 60);
    }
};

#endif