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

    ValueInt *distStartTurn = new ValueInt(85); // 90
    ValueInt *distFullTurn = new ValueInt(75); // 85

    ValueInt *turboModeDist = new ValueInt(85); // 70
    ValueInt *turboTurn = new ValueInt(45); // 20
    ValueInt *turboMaxTurn = new ValueInt(5); // 15

    Adaptation *forwardSpeed = new Adaptation(74, 15, 2); // 80
    Adaptation *forwardAcceleration = new Adaptation(10, 15, 2);

    ValueInt *distWall = new ValueInt(12); // 8

    ValueInt *distPersecution = new ValueInt(40);


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
            if (sensors->isSamePlace(1500)) {
                return backward->init(800);
            }
//            if (persecutionStopwatch->isMoreThan(3000)) {
//                return rightWall->init(5000);
//            }
//            if (rotationHelper->isCounterClockWise()) {
//                rotationHelper->reset();
//                return rotate->init();
//            }
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final override {
        angle = getAngle(sensors->rightDistance, sensors->leftDistance);

//        power += map(abs(angle), Mechanics::TURN_MAX_ANGLE, 0, 0, 8);
        power = forwardSpeed->adaptedValue();

        if (sensors->minForwardDistance > turboModeDist->value) {
//            angle = (int) (angle * 30.0 * turboTurn->value / sensors->maxForwardDistance);
            angle = sign(angle) * (int) map(sensors->minForwardDistance,
                                      turboModeDist->value, 150,
                                      turboMaxTurn->value, 0);
//            angle = maxAngle(angle, turboMaxTurn->value);
            power += (int) map(sensors->minForwardDistance,
                               turboModeDist->value, 150,
                               0, forwardAcceleration->adaptedValue());
        } else {
            angle = minAngle(angle, (int) map(sensors->minForwardDistance,
                                              distFullTurn->value, distStartTurn->value,
                                              Mechanics::TURN_MAX_ANGLE, 0));
            checkPersecution(sensors);
        }

        rotationHelper->placeVector(angle, power);
    }

    void checkPersecution(const SensorsHolder *sensors) {
        // уменьшаем скорость вплоть до остановки если везде препятствия
        if (sensors->minDistance < distPersecution->value) {
//                angle = maxAngle(angle, (int) map(sensors->minDistance, 0, 50, 15, 30));
            power = (int) map(sensors->minForwardDistance,
                              8, distPersecution->value,
                              70, power);
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

private:

    bool persecution = false;
    Stopwatch *persecutionStopwatch = new Stopwatch();

    RotationHelper *rotationHelper = new RotationHelper();

    bool isWallNear(SensorsHolder *sensors) const {
        return sensors->minForwardDistance < distWall->value;
    }
};

#endif
