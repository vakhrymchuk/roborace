#pragma once

#include "Strategy.h"

class Turbo : public Strategy {

public:

    static const int MAX_DIST = 200;

    Param *turboModeDisableDist = new Param(110, "turbo-disable-dist", "turbo");
    Param *turboMaxTurn = new Param(4, "turbo-angle-max-turn", "turbo");
    Param *turboSpeed = new Param(60, "turbo-speed", "turbo");
    Param *acceleration = new Param(20, "turbo-accel", "turbo");
    Adaptation *forwardAcceleration = new Adaptation(acceleration, 0, 30, 20, 4);


public:

    Strategy *forward = nullptr;


    virtual Strategy *init(Strategy *callback, unsigned int minMs, int param = 0) final {
        Strategy::init(callback, minMs);
        forwardAcceleration->init();
        return this;
    }

    Strategy *check(SensorsHolder *sensors) final {
        if (minTimeout->isReady()) {
            if (isTurboFinish(sensors)) {
                return callback->init(this);
            }
        }
        return this;
    }

    void calc(SensorsHolder *sensors) final {


        int sum = sensors->l90d / 2 + sensors->l30d + sensors->l60d / 10
                  - sensors->r90d / 2 - sensors->r30d - sensors->r60d / 10;

        angle = (int) map(sum, -100, 100, -turboMaxTurn->value, turboMaxTurn->value);
        angle = constrain(angle, -turboMaxTurn->value, turboMaxTurn->value);

//        if (sensors->minForwardDistance > 150) {
//            power = turboSpeed->value + 4;
//        }


        power = turboSpeed->value;
        power += (int) map(sensors->maxForwardDistance,
                           turboModeDisableDist->value, MAX_DIST,
                           0, forwardAcceleration->adaptedValue());

    }

private:

    bool isTurboFinish(SensorsHolder *sensors) const {
        return sensors->minForwardDistance < turboModeDisableDist->value;
    }
};
