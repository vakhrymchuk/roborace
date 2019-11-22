#ifndef ROBORACE_TURBO_H
#define ROBORACE_TURBO_H

#include "Strategy.h"

class Turbo : public Strategy {
public:

    ValueInt *turboModeDist = new ValueInt(140);

    //    ValueInt *turboTurn = new ValueInt(5);
    ValueInt *turboMaxTurn = new ValueInt(5);

    ValueInt *speed = new ValueInt(120);


    Adaptation *forwardAcceleration = new Adaptation(0, 15, 0);



    virtual Strategy *init(Strategy *callback, unsigned int minMs) final override {
        Strategy::init(callback, minMs);
        forwardAcceleration->init();
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final override {
        if (minTimeout->isReady()) {
            if (isTurboFinish(sensors)) {
                return callback->init(this);
            }
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final override {
        power = speed->value;
//        power += (int) map(sensors->maxForwardDistance,
//                           turboModeDist->value, 400,
//                           0, forwardAcceleration->adaptedValue());


        angle = getAngleSign(sensors->rightDistance, sensors->leftDistance);

//        angle = angle * (int) map(sensors->minForwardDistance,
//                                  turboModeDist->value, 150,
//                                  turboMaxTurn->value, 0);
//            angle = angle * turboMaxTurn->value;
        angle = limitMaxAngle(angle, turboMaxTurn->value);

    }

    Strategy *forward;

private:

    bool isTurboFinish(SensorsHolder *sensors) const {
        return sensors->minForwardDistance < turboModeDist->value;
    }
};


#endif
