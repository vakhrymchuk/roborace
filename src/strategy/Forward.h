#ifndef ROBORACE_FORWARD_H
#define ROBORACE_FORWARD_H

#include <value/Param.h>
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

    Param *distStartTurn = new Param(110);
    Param *distFullTurn = new Param(80);

    Param *turboModeDist = new Param(110);

    Adaptation *forwardSpeed = new Adaptation(56, 20, 4);

    Param *distWall = new Param(15);

    Param *distPersecution = new Param(50);


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

        int sum = -sensors->right45Distance - sensors->rightDistance / 2
                  + sensors->left45Distance + sensors->leftDistance / 2;

        sum = constrain(sum, -100, 100);

        angle = (int) map(sum, -100, 100, Mechanics::FULL_RIGHT, Mechanics::FULL_LEFT);


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

#endif
