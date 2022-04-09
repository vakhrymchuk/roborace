#pragma once

#include <value/Param.h>
#include "Strategy.h"
#include "RotationHelper.h"


class Drag : public Strategy {
public:

    RotationHelper *rotationHelper = new RotationHelper();


    Param *f30k = new Param(0, "forward-30-koef", "forward");
    Param *f60k = new Param(80, "forward-60-koef", "forward");
    Param *f90k = new Param(20, "forward-90-koef", "forward");
    Param *t30k = new Param(80, "turbo-30-koef", "turbo");
    Param *t60k = new Param(30, "turbo-60-koef", "turbo");
    Param *t90k = new Param(10, "turbo-90-koef", "turbo");
    Param *maxSum = new Param(100, "max-sum", "forward");

    Param *stuckCheckEnabled = new Param(0, "stuck-check", "main");
    Param *rotationCheckEnabled = new Param(0, "rotation-check", "main");
    Param *runCorrectionSide = new Param(0, "run-correction-side", "forward");

    Param *speed = new Param(70, "forward-speed", "forward");
    Param *minSpeed = new Param(40, "min-speed", "forward");
    Param *maxSpeed = new Param(100, "max-speed", "forward");

    Param *distWall = new Param(110, "wall-dist", "forward");

    Param *distPersecution = new Param(50, "persecution-dist", "forward");

    Param *turboModeDist = new Param(120, "turbo-mode-dist", "turbo");
    Param *turboSpeed = new Param(8, "turbo-speed", "turbo");
    Param *turboMaxTurn = new Param(10, "turbo-angle-max-turn", "turbo");

    Strategy *backward;
    Strategy *rotate;

    Stopwatch start;


public:

    Stopwatch backStopwatch = Stopwatch();
    boolean backStarted = false;

    Strategy *init(Strategy *callback, unsigned int minMs, int param) final {
        Strategy::init(callback, minMs);
        start.start();
        return this;
    }

    Strategy *check(SensorsHolder *sensors) final {
        return this;
    }

    void calc(SensorsHolder *sensors) final {
        power = speed->value;
        if (start.isMoreThan(6000)) {
            power = 60;
        }

        angle = calcAngleBySensors(sensors, f30k->value, f60k->value, f90k->value, Mechanics::TURN_MAX_ANGLE);

        if (isWallNear(sensors)) {
            power = 0;
        }

    }

    int calcAngleBySensors(const SensorsHolder *sensors, int s30k, int s60k, int s90k, int maxTurn) const {
        int sum = (sensors->l30d - sensors->r30d) * s30k / 100
                  + (sensors->l60d - sensors->r60d) * s60k / 100
                  + (sensors->l90d - sensors->r90d) * s90k / 100;

        int maxSumValue = maxSum->value;
        sum = constrain(sum, -maxSumValue, maxSumValue);
        return (int) map(sum, -maxSumValue, maxSumValue, -maxTurn, maxTurn);
    }


private:

    bool isWallNear(SensorsHolder *sensors) {
        bool isBackNeed = sensors->f00d <= 110;

        if (isBackNeed) {
            if (!backStarted) {
                backStarted = true;
                backStopwatch.start();
            }
            return backStopwatch.isMoreThan(40);
        }
        backStarted = false;

        return isBackNeed;
    }
};
