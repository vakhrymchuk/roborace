#ifndef ROBORACE_TURBO_H
#define ROBORACE_TURBO_H

#include "Strategy.h"

class Turbo : public Strategy {

public:

    static const int MAX_DIST = 200;

    ValueInt *turboModeDisableDist = new ValueInt(110);

    ValueInt *turboMaxTurn = new ValueInt(4);

    ValueInt *speed = new ValueInt(60);

    Adaptation *forwardAcceleration = new Adaptation(20, 20, 4);


public:

    Strategy *forward;


    virtual Strategy *init(Strategy *callback, unsigned int minMs, int param = 0) final {
        Strategy::init(callback, minMs);
//        forwardAcceleration->init();
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final {
        if (minTimeout->isReady()) {
            if (isTurboFinish(sensors)) {
                return callback->init(this);
            }
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final {


        int sum = sensors->leftDistance / 2 + sensors->left45Distance + sensors->forwardLeftDistance / 10
                  - sensors->rightDistance / 2 - sensors->right45Distance - sensors->forwardRightDistance / 10;

        angle = map(sum, -100, 100, -turboMaxTurn->value, turboMaxTurn->value);
        angle = constrain(angle, -turboMaxTurn->value, turboMaxTurn->value);

//        if (sensors->minForwardDistance > 150) {
//            power = speed->value + 4;
//        }


        power = speed->value;
        power += (int) map(sensors->maxForwardDistance,
                           turboModeDisableDist->value, MAX_DIST,
                           0, forwardAcceleration->adaptedValue());

    }

private:

    bool isTurboFinish(SensorsHolder *sensors) const {
        return sensors->minForwardDistance < turboModeDisableDist->value;
    }
};


#endif
