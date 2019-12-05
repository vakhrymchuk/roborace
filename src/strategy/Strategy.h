#ifndef ROBORACE_STRATEGY_H
#define ROBORACE_STRATEGY_H

#include "Timeout.h"
#include "../mechanics/Mechanics.h"
#include "../mechanics/SensorsHolder.h"


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
protected:

    int angle = 0;
    int power = 0;
    int speed = 0;


    Stopwatch *stopwatch = new Stopwatch;
    Timeout *minTimeout = new Timeout();
    Strategy *callback;

public:

    virtual Strategy *init(Strategy *callback, unsigned int minMs = 0) {
        stopwatch->start();
        minTimeout->start(minMs);
        Strategy::callback = callback;
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) {
        return this;
    }

    virtual void calc(SensorsHolder *sensors) = 0;

    void run(Mechanics *mechanics) {
        mechanics->run(angle, power);
        speed = mechanics->engine->getSpeed();
    }

    static int getAngle(int right, int left) {
        return 45 - degrees(atan2(right, left));
    }

    static int getAngleSign(int right, int left) {
        if (smoothEquals(right, left)) return 0;
        return right > left ? -1 : 1;
    }

    static int limitMinAngle(int angle, int minValue) {
        if (angle == 0) return minValue;

        if (abs(angle) < minValue) {
            return (sign<int>(angle) * minValue);
        }

        return angle;
    }

    static int limitMaxAngle(int angle, int maxValue) {
        if (abs(angle) > maxValue) {
            return (sign<int>(angle) * maxValue);
        }

        return angle;
    }

    static int mapConstrain(int x, int in_min, int in_max, int out_min, int out_max) {
        int y = map(x, in_min, in_max, out_min, out_max);
        return constrain(y, out_min, out_max);
    }

    static int mapConstrain(long x, long in_min, long in_max, int out_min, int out_max) {
        long y = map(x, in_min, in_max, out_min, out_max);
        return (int) constrain(y, out_min, out_max);
    }


    static bool smoothEquals(int right, int left) {
        return fabs(sqrt(right) - sqrt(left)) <= 1.0;
    }

    static unsigned short smooth(unsigned short num) {
//        return num >> 3;
        return (unsigned short) sqrt(num);
    }
};

#endif
