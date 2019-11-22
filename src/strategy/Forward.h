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

    ValueInt *distStartTurn = new ValueInt(80);
    ValueInt *distFullTurn = new ValueInt(50);

    ValueInt *turboModeDist = new ValueInt(150);

    Adaptation *forwardSpeed = new Adaptation(60, 15, 0);

    ValueInt *distWall = new ValueInt(8);

    ValueInt *distPersecution = new ValueInt(50);

    Stopwatch turnStopwatch;
    int turnDirection = 0;
    boolean turn = false;


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
                return backward->init(this, 800);
            }
//            if (persecutionStopwatch->isMoreThan(3000)) {
//                return leftWall->init(this, 5000);
//            }
            if (rotationHelper->isCounterClockWise()) {
                rotationHelper->reset();
                return rotate->init(this);
            }
            if (sensors->minForwardDistance >= turboModeDist->value) {
                return turbo;
            }
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final override {
        angle = getAngleSign(sensors->rightDistance, sensors->leftDistance);

        power = forwardSpeed->adaptedValue();

        if (sensors->rightDistance > 90 && smooth(sensors->rightDistance) > smooth(sensors->leftDistance)) {
            if (!turn) {
                turn = true;
                turnStopwatch.start();
            }
            angle = Mechanics::FULL_RIGHT;
        } else if (sensors->leftDistance > 90 && smooth(sensors->leftDistance) > smooth(sensors->rightDistance)) {
            if (!turn) {
                turn = true;
                turnStopwatch.start();
            }
            angle = Mechanics::FULL_LEFT;
        } else {
            normalMode(sensors);
        }
        checkPersecution(sensors);

        rotationHelper->placeVector(angle, power);
    }


    void normalMode(const SensorsHolder *sensors) {
        turn = false;
        int minAngle = (int) map(sensors->minForwardDistance,
                                 distFullTurn->value, distStartTurn->value,
                                 Mechanics::TURN_MAX_ANGLE, 0);
        angle = limitMinAngle(angle, minAngle);

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
        return sensors->minForwardDistance < distWall->value || sensors->maxForwardDistance < 20;
    }
};

#endif
