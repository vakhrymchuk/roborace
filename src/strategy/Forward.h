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

    ValueInt *distStartTurn = new ValueInt(90); // 90
    ValueInt *distFullTurn = new ValueInt(75); // 85
    ValueInt *mediumModeMaxTurn = new ValueInt(30); // 30

    ValueInt *turboModeDist = new ValueInt(100); // 70
    ValueInt *turboTurn = new ValueInt(40); // 20
    ValueInt *turboMaxTurn = new ValueInt(20); // 15

    Adaptation *forwardSpeed = new Adaptation(65); // 80
    Adaptation *forwardAcceleration = new Adaptation(0);

    ValueInt *distWall = new ValueInt(10); // 8


    virtual Strategy *init(unsigned int minMs = 500) final override {
        Strategy::init(minMs);
        forwardSpeed->init();
        forwardAcceleration->init();
//        rotationHelper->reset();
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final override {
        if (minTimeout->isReady()) {
            if (isWallNear(sensors)) {
                return backward->init(0);
            }
            if (sensors->isSamePlace(4000)) {
                return backward->init(800);
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
//        power += (int) map(sensors->maxForwardDistance, 0, 180, 0, forwardAcceleration->adaptedValue());

        if (sensors->minForwardDistance < 45) {
            angle = minAngle(angle, 35);
        } else if (sensors->forwardLeftDistance > turboModeDist->value &&
            sensors->forwardRightDistance > turboModeDist->value) {
            angle = (int) (angle * 30.0 * turboTurn->value / sensors->maxForwardDistance);
            angle = maxAngle(angle, turboMaxTurn->value);
        } else {
            angle = minAngle(angle, (int) map(sensors->minFactor,
                                              distFullTurn->value, distStartTurn->value,
                                              30, 0));
            angle = maxAngle(angle, mediumModeMaxTurn->value);
        }

        rotationHelper->placeVector(angle, power);
    }

    Strategy *backward;
    Strategy *rotate;

private:

    RotationHelper *rotationHelper = new RotationHelper();

    bool isWallNear(SensorsHolder *sensors) const {
        return sensors->forwardLeftDistance < distWall->value || sensors->forwardRightDistance < distWall->value;
    }
};

#endif
