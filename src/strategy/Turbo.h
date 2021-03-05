#pragma once

#include "Strategy.h"

class Turbo : public Strategy {

public:

    static const int MAX_DIST = 200;

    Param *turboModeDisableDist = new Param(110, "turbo-disable-dist", "turbo");
    Param *turboMaxTurn = new Param(4, "turbo-angle-max-turn", "turbo");
    Param *turboSpeed = new Param(60, "turbo-speed", "turbo");
    Param *acceleration = new Param(20, "turbo-accel", "turbo");
    Adaptation *forwardAcceleration = new Adaptation(acceleration, 20, 4);


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


        int sum = sensors->leftDistance / 2 + sensors->left45Distance + sensors->forwardLeftDistance / 10
                  - sensors->rightDistance / 2 - sensors->right45Distance - sensors->forwardRightDistance / 10;

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
