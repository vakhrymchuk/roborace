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

    ValueInt *backwardSpeed = new ValueInt(90);

    virtual Strategy *init(Strategy *callback, unsigned int minMs) final {
        Strategy::init(callback, minMs);
//        minTimeout->start(500);
        maxTimeout->start(2000);
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final {
        if (minTimeout->isReady()) {
            if (maxTimeout->isReady() || isBackFinish(sensors)) {
                return callback->init(this);
            }
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final {
        if (stopwatch->isLessThan(500) || stopwatch->isMoreThan(1200)) {
            angle = 0;
        } else {
            if (angle == 0) {
                angle = -getAngleSign(sensors->rightDistance, sensors->leftDistance);
                angle = limitMinAngle(angle, 10);
            }
        }

        if (stopwatch->isLessThan(400)) {
            power = 0;
        } else if (stopwatch->isMoreThan(1200)) {
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
        return (sensors->minForwardDistance > 40) || (sensors->minForwardDistance > 15 && sensors->maxDistance > 60);
    }
};

#endif