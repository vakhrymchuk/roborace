#ifndef ROBORACE_TURBO_H
#define ROBORACE_TURBO_H

#include "Strategy.h"

class Turbo : public Strategy {

private:

    ValueInt *turboModeDisableDist = new ValueInt(110);

    ValueInt *turboMaxTurn = new ValueInt(5);

    ValueInt *speed = new ValueInt(80);

    Adaptation *forwardAcceleration = new Adaptation(0, 15, 0);


public:

    Strategy *forward;


    virtual Strategy *init(Strategy *callback, unsigned int minMs, int param = 0) final {
        Strategy::init(callback, minMs);
        forwardAcceleration->init();
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
        power = speed->value;
//        power += (int) map(sensors->maxForwardDistance,
//                           turboModeDist->value, 400,
//                           0, forwardAcceleration->adaptedValue());


//        angle = getAngleSign(sensors->rightDistance, sensors->leftDistance);

//        angle = angle * (int) map(sensors->minForwardDistance,
//                                  turboModeDist->value, 150,
//                                  turboMaxTurn->value, 0);
//            angle = angle * turboMaxTurn->value;
        if (sensors->rightDistance > sensors->leftDistance) {
            angle = -turboMaxTurn->value;
        } else {
            angle = turboMaxTurn->value;
        }



//        angle = limitMaxAngle(angle, turboMaxTurn->value);

    }

private:

    bool isTurboFinish(SensorsHolder *sensors) const {
        return sensors->minForwardDistance < turboModeDisableDist->value;
    }
};


#endif
