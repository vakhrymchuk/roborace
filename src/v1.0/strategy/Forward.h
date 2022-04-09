#pragma once

#include "v1.0/value/Param.h"
#include "Strategy.h"
#include "v1.0/Adaptation.h"
#include "RotationHelper.h"


class Forward : public Strategy {
private:

    int rotation = 0;

public:

    RotationHelper *rotationHelper = new RotationHelper();


    Param *stuckCheckEnabled = new Param(0, "stuck-check", "main");
    Param *rotationCheckEnabled = new Param(0, "rotation-check", "main");
    Param *runCorrectionSide = new Param(0, "run-correction-side", "forward");

    Param *speed = new Param(54, "forward-speed", "forward");
    Param *minSpeed = new Param(40, "min-speed", "forward");
    Param *maxSpeed = new Param(100, "max-speed", "forward");
    Adaptation *forwardSpeed = new Adaptation(speed, minSpeed, maxSpeed, 10, 2, 1, 4);

    Param *distWall = new Param(20, "wall-dist", "forward");

    Param *distPersecution = new Param(50, "persecution-dist", "forward");

    Param *turboModeDist = new Param(120, "turbo-mode-dist", "turbo");
    Param *turboSpeed = new Param(8, "turbo-speed", "turbo");
    Param *turboMaxTurn = new Param(10, "turbo-angle-max-turn", "turbo");


    Stopwatch backStopwatch = Stopwatch();
    boolean backStarted = false;

    Strategy *init(Strategy *callback, unsigned int minMs, int param) final {
        Strategy::init(callback, minMs);
        forwardSpeed->init();
        persecution = false;
        rotation = 0;
        return this;
    }

    Strategy *check(SensorsHolder *sensors) final {
        if (minTimeout->isReady()) {
            if (isWallNear(sensors)) {
                return backward->init(this, 500, rotation);
            }
            if (stuckCheckEnabled->value && sensors->isSamePlace(4000)) {
                return backward->init(this, 600);
            }
//            if (persecutionStopwatch->isMoreThan(3000)) {
//                return leftWall->init(this, 5000);
//            }
            if (rotationCheckEnabled->value && rotationHelper->isCounterClockWise()
                && sensors->left45Distance >= 30) {
                rotationHelper->reset();
                return rotate->init(this);
            }
//            if (sensors->maxForwardDistance >= turboModeDist->value) {
//                return turbo->init(this);
//            }
        }
        return this;
    }

    void calc(SensorsHolder *sensors) final {
        power = forwardSpeed->adaptedValue();

        if (sensors->maxForwardDistance >= turboModeDist->value) {
//            power += map(sensors->f00d, turboModeDist->value, 200, 0, turboSpeed->value);
            power += turboSpeed->value;
        }
//        checkPersecution(sensors);

        rotationHelper->placeVector(angle, power);

//        Serial.printf("forward power=%d\n", turboSpeed->value);
    }


    void checkPersecution(const SensorsHolder *sensors) {
        if (sensors->minForwardDistance < distPersecution->value) {
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

//    Strategy *turbo;
    Strategy *backward;
    Strategy *rotate;
    Strategy *rightWall;
    Strategy *leftWall;

private:

    bool persecution = false;
    Stopwatch *persecutionStopwatch = new Stopwatch();


    bool isWallNear(SensorsHolder *sensors) {

        bool isBackNeed = sensors->minForwardDistance < distWall->value
                          || sensors->maxForwardDistance < 40;

        if (isBackNeed) {
            if (!backStarted) {
                backStarted = true;
                backStopwatch.start();
            }
            return backStopwatch.isMoreThan(100);
        }
        backStarted = false;
        return false;
    }
};
