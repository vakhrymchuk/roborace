#pragma once

#include "Strategy.h"


class LeftWall : public Strategy {
public:

    virtual Strategy *init(Strategy *callback, unsigned int minMs = 0, int param = 0) final {
        Strategy::init(callback, minMs);
        speed.init();
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final {
        if (minTimeout->isReady() || sensors->minForwardDistance > 100) {
            return callback->init(this, 2000);
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) {
        int sideDistToWall = 55;
        int borderCenter = 5;
        int borderFullTurn = 20;

        int forwardDistToWall = 100;
        int forwardDistFullTurn = 80;

        if (sensors->f00d < forwardDistToWall &&
            sensors->l90d < sideDistToWall + 2 * borderFullTurn) {
            angle = mapConstrain(sensors->f00d,
                                 forwardDistFullTurn, forwardDistToWall,
                                 Mechanics::FULL_RIGHT, 0);
        } else if (sensors->l90d > sideDistToWall + borderCenter) {
            angle = mapConstrain(sensors->l90d,
                                 sideDistToWall + borderCenter, sideDistToWall + borderFullTurn,
                                 0, Mechanics::FULL_LEFT);
        } else if (sensors->l90d < sideDistToWall - borderCenter) {
            angle = mapConstrain(sensors->l90d,
                                 sideDistToWall - borderCenter, sideDistToWall - borderFullTurn,
                                 0, Mechanics::FULL_RIGHT);
        } else {
            angle = 0;
        }

        power = speed.adaptedValue();
        if (sensors->minForwardDistance > 120) {
            power += 20;
        }

        if (sensors->minForwardDistance < 15 || (back && sensors->f00d < 35)) {
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

    Adaptation speed = Adaptation(new Param(60), 50, 70, 10, 4);

};
