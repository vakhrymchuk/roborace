#pragma once

#include <value/Param.h>
#include "Strategy.h"
#include "Adaptation.h"
#include "RotationHelper.h"


class Forward : public Strategy {
private:

    int rotation = 0;

public:

//    Param *distFullTurn = new Param(80);
    Param *side45SensorsKoef = new Param(100, "side-45-sensors-koef", "forward");
    Param *sideSensorsKoef = new Param(10, "side-sensors-koef", "forward");
    Param *maxSum = new Param(100, "max-sum", "forward");

    Param *turboModeDist = new Param(110, "turbo-mode-dist", "forward");

    Param *speed = new Param(56, "forward-speed", "forward");
    Adaptation *forwardSpeed = new Adaptation(speed, 20, 4);

    Param *distWall = new Param(15, "wall-dist", "forward");

    Param *distPersecution = new Param(50, "persecution-dist", "forward");


    virtual Strategy *init(Strategy *callback, unsigned int minMs, int param = 0) final {
        Strategy::init(callback, minMs);
        forwardSpeed->init();
        persecution = false;
        rotation = 0;
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final {
        if (minTimeout->isReady()) {
//            if (isWallNear(sensors)) {
//                return backward->init(this, 500, rotation);
//            }
//            if (sensors->isSamePlace(4000)) {
//                return backward->init(this, 600);
//            }
//            if (persecutionStopwatch->isMoreThan(3000)) {
//                return leftWall->init(this, 5000);
//            }
//            if (rotationHelper->isCounterClockWise()) {
//                rotationHelper->reset();
//                return rotate->init(this);
//            }
//            if (sensors->maxForwardDistance >= turboModeDist->value) {
//                return turbo->init(this);
//            }
        }
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final {

        power = forwardSpeed->adaptedValue();

        int sum =
                (sensors->left45Distance - sensors->right45Distance) * side45SensorsKoef->value / 100
                + (sensors->leftDistance - sensors->rightDistance) * sideSensorsKoef->value / 100;

        sum = constrain(sum, -maxSum->value, maxSum->value);

        angle = (int) map(sum, -maxSum->value, maxSum->value, Mechanics::FULL_RIGHT, Mechanics::FULL_LEFT);


        Serial.printf("sum = %d  angle = %d  power = %d \n", sum, angle, power);

//        checkPersecution(sensors);

//        rotationHelper->placeVector(angle, power);
    }


    void checkPersecution(const SensorsHolder *sensors) {
        if (sensors->minDistance < distPersecution->value) {
//                angle = limitMaxAngle(angle, (int) map(sensors->minDistance, 0, 50, 15, 30));
            power = (int) map(sensors->minForwardDistance,
                              10, distPersecution->value,
                              50, power);
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
        return sensors->minForwardDistance < distWall->value && sensors->forwardLeftSensor->isLongerThan(100)
               && sensors->forwardRightSensor->isLongerThan(100)
//               || sensors->maxForwardDistance < 10
//               || sensors->minDistance < 20
                ;
    }
};
