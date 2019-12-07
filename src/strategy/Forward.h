#ifndef ROBORACE_FORWARD_H
#define ROBORACE_FORWARD_H

#include "Strategy.h"
#include "Adaptation.h"
#include "RotationHelper.h"

/**
 *  ______ ___________ _    _  ___  ____________
 *  |  ___|  _  | ___ \ |  | |/ _ \ | ___ \  _  \
 *  | |_  | | | | |_/ / |  | / /_\ \| |_/ / | | |
 *  |  _| | | | |    /| |/\| |  _  ||    /| | | |
 *  | |   \ \_/ / |\ \\  /\  / | | || |\ \| |/ /
 *  \_|    \___/\_| \_|\/  \/\_| |_/\_| \_|___/
 */
class Forward : public Strategy {
private:

    int rotation = 0;

public:

    ValueInt *distStartTurn = new ValueInt(130);
    ValueInt *distFullTurn = new ValueInt(110);

    ValueInt *turboModeDist = new ValueInt(130);

    Adaptation *forwardSpeed = new Adaptation(56, 25, 0);

    ValueInt *distWall = new ValueInt(10);

    ValueInt *distPersecution = new ValueInt(50);


    virtual Strategy *init(Strategy *callback, unsigned int minMs, int param = 0) final {
        Strategy::init(callback, minMs);
        forwardSpeed->init();
        persecution = false;
        rotation = 0;
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final {
        if (minTimeout->isReady()) {
            if (isWallNear(sensors)) {
                return backward->init(this, 500, rotation);
            }
            if (sensors->isSamePlace(4000)) {
                return backward->init(this, 600);
            }
//            if (persecutionStopwatch->isMoreThan(3000)) {
//                return leftWall->init(this, 5000);
//            }
            if (rotationHelper->isCounterClockWise()) {
                rotationHelper->reset();
                return rotate->init(this);
            }
            if (sensors->minForwardDistance >= turboModeDist->value) {
                return turbo->init(this);
            }
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final {

        if (sensors->minForwardDistance < distStartTurn->value) {

            angle = getAngleSign(sensors->rightDistance, sensors->leftDistance);

            int minAngle = map(sensors->minForwardDistance,
                                            distStartTurn->value, distFullTurn->value,
                                            0, Mechanics::TURN_MAX_ANGLE);
            angle = angle * minAngle;
        }
//            if (speed > power) {
//                angle = min(angle, 20);
//            }

//        if (stopwatch->isLessThan(300, MS)) {
//            angle = min(angle, 15);
//        }

        if (sensors->leftDistance > 80) {
            rotation = Mechanics::FULL_LEFT;
        } else if (sensors->rightDistance > 80) {
            rotation = Mechanics::FULL_RIGHT;
        } else {
            rotation = 0;
        }

        power = forwardSpeed->adaptedValue();
//        checkPersecution(sensors);

        rotationHelper->placeVector(angle, power);
    }


    void checkPersecution(const SensorsHolder *sensors) {
        if (sensors->minDistance < distPersecution->value) {
//                angle = limitMaxAngle(angle, (int) map(sensors->minDistance, 0, 50, 15, 30));
            power = (int) map(sensors->minForwardDistance,
                              8, distPersecution->value,
                              80, power);
            if (!persecution) {
                persecution = true;
                persecutionStopwatch->start();
            }
        } else {
            persecution = false;
        }
    }

    Strategy *turbo;
    Strategy *backward;
    Strategy *rotate;
    Strategy *rightWall;
    Strategy *leftWall;

private:

    bool persecution = false;
    Stopwatch *persecutionStopwatch = new Stopwatch();

    RotationHelper *rotationHelper = new RotationHelper();

    bool isWallNear(SensorsHolder *sensors) const {
        return sensors->minForwardDistance < distWall->value
//               || sensors->maxForwardDistance < 10
//               || sensors->minDistance < 20
               ;
    }
};

#endif
