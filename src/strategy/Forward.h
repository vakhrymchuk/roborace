#ifndef ROBORACE_FORWARD_H
#define ROBORACE_FORWARD_H

#include "Strategy.h"

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

    ValueInt *distStartTurn = new ValueInt(120);
    ValueInt *distFullTurn = new ValueInt(105);

    ValueInt *turboModeDist = new ValueInt(120);
//    ValueInt *turboTurn = new ValueInt(5);
    ValueInt *turboMaxTurn = new ValueInt(6);

    Adaptation *forwardSpeed = new Adaptation(90, 15, 2);
    Adaptation *forwardAcceleration = new Adaptation(4, 15, 0);

    ValueInt *distWall = new ValueInt(8);

    ValueInt *distPersecution = new ValueInt(40);


    virtual Strategy *init(Strategy *callback, unsigned int minMs) final override {
        Strategy::init(callback, minMs);
        forwardSpeed->init();
        forwardAcceleration->init();
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
                return backward->init(this, 800);
            }
//            if (persecutionStopwatch->isMoreThan(3000)) {
//                return leftWall->init(this, 5000);
//            }
            if (rotationHelper->isCounterClockWise()) {
                rotationHelper->reset();
                return rotate->init(this);
            }
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final override {
        angle = getAngleSign(sensors->rightDistance, sensors->leftDistance);

        power = forwardSpeed->adaptedValue();

        if (sensors->maxForwardDistance >= turboModeDist->value) {
//            angle = (int) (angle * 30.0 / sensors->maxForwardDistance);
//            angle = angle * (int) map(sensors->minForwardDistance,
//                                            turboModeDist->value, 120,
//                                            turboMaxTurn->value, 0);
            angle = angle * turboMaxTurn->value;
//            angle = 0;
//            angle = maxAngle(angle, turboMaxTurn->value);
//            if (turboStopwatch.isLessThan(1700)) {
//                power += (int) map(sensors->minForwardDistance,
//                                   turboModeDist->value, 150,
//                                   0, forwardAcceleration->adaptedValue());
                    power += forwardAcceleration->adaptedValue();
//            }
            if (!turbo) {
                turbo = true;
                turboStopwatch.start();
            }
        } else if (sensors->rightDistance > 90 && smooth(sensors->rightDistance) > smooth(sensors->leftDistance)) {
            power -= 10;
            turbo = false;
            angle = Mechanics::FULL_RIGHT;
        } else if (sensors->leftDistance > 90 && smooth(sensors->leftDistance) > smooth(sensors->rightDistance)) {
            power -= 10;
            turbo = false;
            angle = Mechanics::FULL_LEFT;
        } else {
            turbo = false;
            angle = minAngle(angle, (int) map(sensors->minForwardDistance,
                                              distFullTurn->value, distStartTurn->value,
                                              Mechanics::TURN_MAX_ANGLE, 0));
//            if (abs(angle) > 20) {power +=10;}
//            angle = maxAngle(angle, 30);
            checkPersecution(sensors);
        }

        rotationHelper->placeVector(angle, power);
    }

    void checkPersecution(const SensorsHolder *sensors) {
        if (sensors->minDistance < distPersecution->value) {
//                angle = maxAngle(angle, (int) map(sensors->minDistance, 0, 50, 15, 30));
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

    Strategy *backward;
    Strategy *rotate;
    Strategy *rightWall;
    Strategy *leftWall;

private:

    bool turbo = false;
    Stopwatch turboStopwatch;

    bool persecution = false;
    Stopwatch *persecutionStopwatch = new Stopwatch();

    RotationHelper *rotationHelper = new RotationHelper();

    bool isWallNear(SensorsHolder *sensors) const {
        return sensors->minForwardDistance < distWall->value || sensors->maxForwardDistance < 20;
    }
};

#endif
