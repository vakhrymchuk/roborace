#pragma once

#include <value/Param.h>
#include "Strategy.h"
#include "Adaptation.h"
#include "RotationHelper.h"


class Forward : public Strategy {
private:

    int rotation = 0;

public:

    RotationHelper *rotationHelper = new RotationHelper();

//    Param *distFullTurn = new Param(80);
    Param *f30k = new Param(80, "forward-30-koef", "forward");
    Param *f60k = new Param(100, "forward-60-koef", "forward");
    Param *f90k = new Param(50, "forward-90-koef", "forward");
    Param *t30k = new Param(80, "turbo-30-koef", "turbo");
    Param *t60k = new Param(30, "turbo-60-koef", "turbo");
    Param *t90k = new Param(10, "turbo-90-koef", "turbo");
    Param *maxSum = new Param(100, "max-sum", "forward");

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
                && sensors->l30d >= 30 && sensors->l60d >= 30) {
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

        if (sensors->f00d >= turboModeDist->value) {
            power += map(sensors->f00d, turboModeDist->value, 200, 0, turboSpeed->value);
            angle = calcAngleBySensors(sensors, t30k->value, t60k->value, t90k->value, turboMaxTurn->value);
        } else {
            angle = calcAngleBySensors(sensors, f30k->value, f60k->value, f90k->value, Mechanics::TURN_MAX_ANGLE);
        }

//        Serial.printf("sum = %d  angle = %d  power = %d \n", sum, angle, power);

//        checkPersecution(sensors);

        rotationHelper->placeVector(angle, power);

//        Serial.printf("forward power=%d\n", turboSpeed->value);
    }

    int
    calcAngleBySensors(const SensorsHolder *sensors, int s30k, int s60k, int s90k, int maxTurn) const {
        int sum = (sensors->l30d - sensors->r30d) * s30k / 100
                  + (sensors->l60d - sensors->r60d) * s60k / 100
                  + (sensors->l90d - sensors->r90d) * s90k / 100;

        sum += runCorrectionSide->value;

        int maxSumValue = maxSum->value;
        sum = constrain(sum, -maxSumValue, maxSumValue);
        return (int) map(sum, -maxSumValue, maxSumValue, -maxTurn, maxTurn);
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
