#ifndef ROBORACE_STRATEGY_H
#define ROBORACE_STRATEGY_H

#include <Arduino.h>
#include "Led.h"
#include "Timing.h"
#include "mechanics/Mechanics.h"
#include "mechanics/SensorsHolder.h"
#include "Adaptation.h"
#include "RotationHelper.h"


template<typename T>
int sign(T val) {
    return (T(0) < val) - (val < T(0));
}


/**
 *  _____ _             _
 * /  ___| |           | |
 * \ `--.| |_ _ __ __ _| |_ ___  __ _ _   _
 *  `--. \ __| '__/ _` | __/ _ \/ _` | | | |
 * /\__/ / |_| | | (_| | ||  __/ (_| | |_| |
 * \____/ \__|_|  \__,_|\__\___|\__, |\__, |
 *                               __/ | __/ |
 *                              |___/ |___/
 */
class Strategy {
public:

    virtual Strategy *init(unsigned int minMs = 0) {
        stopwatch->start();
        minTimeout->start(minMs);
        return this;
    };

    virtual Strategy *check(SensorsHolder *sensors) {
        return this;
    }

    virtual void calc(SensorsHolder *sensors) {};

    void run(Mechanics *mechanics) {
        mechanics->run(angle, power);
    }

    int angle;
    int power;

protected:

    Stopwatch *stopwatch = new Stopwatch;
    Timeout *minTimeout = new Timeout();

    int getAngle(int right, int left) const {
        return sign(45 - (int) degrees(atan2(right, left)));
    }

    int minAngle(int angle, int minValue) const {
        if (angle == 0) return minValue;

        if (abs(angle) < minValue) {
            return (sign<int>(angle) * minValue);
        }

        return angle;
    }

    int maxAngle(int angle, int maxValue) const {
        if (abs(angle) > maxValue) {
            return (sign<int>(angle) * maxValue);
        }

        return angle;
    }

};

#include "Forward.h"
#include "Backward.h"
#include "Rotate.h"
//#include "RightWall.h"

#endif
