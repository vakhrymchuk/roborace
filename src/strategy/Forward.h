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
public:

    ValueInt *distStartTurn = new ValueInt(100);
    ValueInt *distFullTurn = new ValueInt(60);

    ValueInt *turboModeDist = new ValueInt(120);

    Adaptation *forwardSpeed = new Adaptation(72, 25, 0);

    ValueInt *distWall = new ValueInt(10);

    ValueInt *distPersecution = new ValueInt(50);


    virtual Strategy *init(Strategy *callback, unsigned int minMs) final override {
        Strategy::init(callback, minMs);
        forwardSpeed->init();
        persecution = false;
//        rotationHelper->reset();
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final override {
        if (minTimeout->isReady()) {
            if (isWallNear(sensors)) {
                return backward->init(this, 0);
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

    virtual void calc(SensorsHolder *sensors) final override {
        angle = getAngleSign(sensors->rightDistance, sensors->leftDistance);

        power = forwardSpeed->adaptedValue();

        normalMode(sensors);
//        checkPersecution(sensors);

        rotationHelper->placeVector(angle, power);
    }


    void normalMode(const SensorsHolder *sensors) {
        int minAngle = (int) map(sensors->minForwardDistance,
                                 distFullTurn->value, distStartTurn->value,
                                 Mechanics::TURN_MAX_ANGLE, 0);
        angle = limitMinAngle(angle, minAngle);
//        angle = angle * minAngle;

//            if (speed > power) {
//                angle = min(angle, 20);
//            }

//            if (abs(angle) > 20) {power +=10;}
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
        return sensors->minForwardDistance < distWall->value || sensors->maxForwardDistance < 30;
    }
};

#endif
