#ifndef ROBORACE_RIGHTWALL_H
#define ROBORACE_RIGHTWALL_H

#include "Strategy.h"

/**
 * ______ _       _     _   _    _       _ _
 * | ___ (_)     | |   | | | |  | |     | | |
 * | |_/ /_  __ _| |__ | |_| |  | | __ _| | |
 * |    /| |/ _` | '_ \| __| |/\| |/ _` | | |
 * | |\ \| | (_| | | | | |_\  /\  / (_| | | |
 * \_| \_|_|\__, |_| |_|\__|\/  \/ \__,_|_|_|
 *           __/ |
 *          |___/
 */
class RightWall : public Strategy {
public:

    virtual Strategy *init(Strategy *callback, unsigned int minMs) final override {
        Strategy::init(callback, minMs);
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final override {
        if (minTimeout->isReady() || sensors->minForwardDistance > 100) {
            return callback->init(this, 0);
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) {
        int rightDistToWall = 50;
        int forwardDistToWall = 90;
        int borderCenter = 5;
        int borderFullTurn = 30;
        if (sensors->rightDistance > rightDistToWall + borderCenter) {
            angle = mapConstrain(sensors->rightDistance,
                                 rightDistToWall + borderCenter, rightDistToWall + borderFullTurn,
                                 0, Mechanics::FULL_RIGHT);
        } else if (sensors->forwardRightDistance < forwardDistToWall) {
            angle = mapConstrain(sensors->forwardRightDistance,
                                 50, forwardDistToWall,
                                 Mechanics::FULL_LEFT, 0);
        } else if (sensors->rightDistance < rightDistToWall - borderCenter) {
            angle = mapConstrain(sensors->rightDistance,
                                 rightDistToWall - borderCenter, rightDistToWall - borderFullTurn,
                                 0, Mechanics::FULL_LEFT);
        } else {
            angle = 0;
        }


//        angle = Mechanics::FULL_LEFT;
        power = 80;

        if (sensors->maxForwardDistance < 30 || sensors->minForwardDistance < 10) {
            angle *= -1;
            power *= -1;
        }
    }

    Strategy *forward;

};

#endif
