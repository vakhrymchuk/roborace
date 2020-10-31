#ifndef ROBORACE_TURBO_H
#define ROBORACE_TURBO_H

#include "Strategy.h"

class Turbo : public Strategy {

private:

    ValueInt *turboModeDisableDist = new ValueInt(120);

    ValueInt *turboMaxTurn = new ValueInt(2);

    ValueInt *speed = new ValueInt(56);

//    Adaptation *forwardAcceleration = new Adaptation(4, 15, 0);


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
        power = speed->value;

        int sum = sensors->leftDistance + sensors->left45Distance /*+ sensors->forwardLeftDistance*/
                  - sensors->rightDistance - sensors->right45Distance /*- sensors->forwardRightDistance*/;

        angle = map(sum, -120, 120, -turboMaxTurn->value, turboMaxTurn->value);

//        if (sensors->minForwardDistance > 150) {
//            power = speed->value + 4;
//        }


//        power += (int) map(sensors->maxForwardDistance,
//                           turboModeDist->value, 400,
//                           0, forwardAcceleration->adaptedValue());


//        angle = getAngleSign(sensors->rightDistance, sensors->leftDistance);

//        angle = angle * (int) map(sensors->minForwardDistance,
//                                  turboModeDist->value, 150,
//                                  turboMaxTurn->value, 0);
//            angle = angle * turboMaxTurn->value;
//        if (sensors->rightDistance + sensors->right45Distance >
//            sensors->leftDistance + sensors->left45Distance) {
//            angle = -turboMaxTurn->value;
//        } else {
//            angle = turboMaxTurn->value;
//        }

//        angle = limitMaxAngle(angle, turboMaxTurn->value);

    }

private:

    bool isTurboFinish(SensorsHolder *sensors) const {
        return sensors->minForwardDistance < turboModeDisableDist->value;
    }
};


#endif
