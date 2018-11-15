#ifndef ROBORACE_LEFTWALL_H
#define ROBORACE_LEFTWALL_H

#include "Strategy.h"


class LeftWall : public Strategy {
public:

    virtual Strategy *init(Strategy *callback, unsigned int minMs = 0) final override {
        Strategy::init(callback, minMs);
        speed.init();
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final override {
        if (minTimeout->isReady() || sensors->minForwardDistance > 100) {
            return callback->init(this, 2000);
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) {
        int sideDistToWall = 30;
        int borderCenter = 3;
        int borderFullTurn = 15;

        int forwardDistToWall = 100;
        int forwardDistFullTurn = 60;

        if (sensors->forwardLeftDistance < forwardDistToWall) {
            angle = mapConstrain(sensors->forwardLeftDistance,
                                 forwardDistFullTurn, forwardDistToWall,
                                 Mechanics::FULL_RIGHT, 0);
        } else if (sensors->leftDistance > sideDistToWall + borderCenter) {
            angle = mapConstrain(sensors->leftDistance,
                                 sideDistToWall + borderCenter, sideDistToWall + borderFullTurn,
                                 0, Mechanics::FULL_LEFT);
        } else if (sensors->leftDistance < sideDistToWall - borderCenter) {
            angle = mapConstrain(sensors->leftDistance,
                                 sideDistToWall - borderCenter, sideDistToWall - borderFullTurn,
                                 0, Mechanics::FULL_RIGHT);
        } else {
            angle = 0;
        }

        power = speed.adaptedValue();

        if (sensors->minForwardDistance < 25 || (back && sensors->forwardLeftDistance < 35)) {
            angle *= -1;
            power = -power;
            back = true;
        } else if (back) {
            back = false;
            speed.init();
        }
    }

    Strategy *forward;
    bool back = false;

    Adaptation speed = Adaptation(80, 20, 4);

};

#endif
