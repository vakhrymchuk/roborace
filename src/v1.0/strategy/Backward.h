#pragma once

#include "Strategy.h"


class Backward : public Strategy {
private:
    int rotation = 0;
public:

    Param *backwardSpeed = new Param(66);

    virtual Strategy *init(Strategy *callback, unsigned int minMs, int param = 0) final {
        Strategy::init(callback, minMs);
//        minTimeout->start(500);
        maxTimeout->start(2000);
        rotation = param;
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final {
        if (minTimeout->isReady()) {
            if (maxTimeout->isReady() || isBackFinish(sensors)) {
                return callback->init(this, 500);
            }
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final {
        if (stopwatch->isLessThan(100)) {
            angle = 0;
        } else if (stopwatch->isLessThan(3000)) {
            if (rotation != 0) {
                angle = -rotation;
            } else {
                angle = -30 * getAngleSign(sensors->r90d, sensors->l90d);
            }
        } else {
            angle = 0;
        }

        if (stopwatch->isLessThan(100)) {
            power = 0;
        } else if (stopwatch->isMoreThan(1000)) {
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
        return sensors->minForwardDistance > 25 && sensors->maxDistance > 50;
    }
};
