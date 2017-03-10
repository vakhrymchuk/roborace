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

    ValueInt *distStartTurn = new ValueInt(100); // 90
    ValueInt *distFullTurn = new ValueInt(80); // 85
    ValueInt *mediumModeMaxTurn = new ValueInt(31); // 30

    ValueInt *turboModeDist = new ValueInt(170); // 70
    ValueInt *turboTurn = new ValueInt(45); // 20
    ValueInt *turboMaxTurn = new ValueInt(20); // 15

    Adaptation *forwardSpeed = new Adaptation(77, 10, 1); // 80
    Adaptation *forwardAcceleration = new Adaptation(5);

    ValueInt *distWall = new ValueInt(10); // 8


    virtual Strategy *init(unsigned int minMs = 500) final override {
        Strategy::init(minMs);
        forwardSpeed->init();
        forwardAcceleration->init();
        persecution = false;
//        rotationHelper->reset();
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final override {
        if (minTimeout->isReady()) {
            if (isWallNear(sensors)) {
                return backward->init(0);
            }
            if (sensors->isSamePlace(3500)) {
                return backward->init(800);
            }
            if (persecutionStopwatch->isMoreThan(2000)) {
                return backward->init(1000);
//                return rightWall->init(5000);
            }
            if (rotationHelper->isCounterClockWise()) {
                rotationHelper->reset();
                return rotate->init();
            }
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final override {
        angle = getAngle(sensors->rightDistance, sensors->leftDistance);

//        power += map(abs(angle), Mechanics::TURN_MAX_ANGLE, 0, 0, 8);
        power = forwardSpeed->adaptedValue();
        power += (int) map(sensors->maxForwardDistance, 0, 180, 0, forwardAcceleration->adaptedValue());

        if (sensors->minForwardDistance > turboModeDist->value) {
            angle = (int) (angle * 30.0 * turboTurn->value / sensors->maxForwardDistance);
            angle = maxAngle(angle, turboMaxTurn->value);
//            if (sensors->minForwardDistance > 130) {
//                power += 5;
//            }
        } else {
            angle = minAngle(angle, (int) map(sensors->minFactor,
                                              distFullTurn->value, distStartTurn->value,
                                              30, 0));
            angle = maxAngle(angle, mediumModeMaxTurn->value);

            // уменьшаем скорость вплоть до остановки если везде препятствия
            if (sensors->minDistance < 50) {
//                angle = maxAngle(angle, (int) map(sensors->minDistance, 0, 50, 15, 30));
                power = (int) map(sensors->minDistance, 8, 50, 50, power);
                if (!persecution && power < 55) {
                    persecution = true;
                    persecutionStopwatch->start();
                } else {
                    persecution = false;
                }
            } else {
                persecution = false;
            }

        }

        rotationHelper->placeVector(angle, power);
    }

    Strategy *backward;
    Strategy *rotate;

private:

    bool persecution = false;
    Stopwatch *persecutionStopwatch = new Stopwatch();

    RotationHelper *rotationHelper = new RotationHelper();

    bool isWallNear(SensorsHolder *sensors) const {
        return sensors->forwardLeftDistance < distWall->value || sensors->forwardRightDistance < distWall->value;
    }
};

#endif
