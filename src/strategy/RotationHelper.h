#ifndef ROBORACE_ROTATION_HELPER_H
#define ROBORACE_ROTATION_HELPER_H

#include <Timing.h>

/**
 * ______      _        _   _             _   _      _
 * | ___ \    | |      | | (_)           | | | |    | |
 * | |_/ /___ | |_ __ _| |_ _  ___  _ __ | |_| | ___| |_ __   ___ _ __
 * |    // _ \| __/ _` | __| |/ _ \| '_ \|  _  |/ _ \ | '_ \ / _ \ '__|
 * | |\ \ (_) | || (_| | |_| | (_) | | | | | | |  __/ | |_) |  __/ |
 * \_| \_\___/ \__\__,_|\__|_|\___/|_| |_\_| |_/\___|_| .__/ \___|_|
 *                                                    | |
 *                                                    |_|
 */
class RotationHelper {
public:

    /** Время круга */
    static const int ARR_SIZE = 10;
    static const int TURN_ON_SECOND = 600;
    static const int ROTATE_THRESHOLD_VALUE = ARR_SIZE * TURN_ON_SECOND;

    RotationHelper() {
        arr = new int[ARR_SIZE];
        reset();
        stopwatch->start();
    }

    void reset() const {
        for (int i = 0; i < ARR_SIZE; ++i) {
            arr[i] = 0;
        }
    }

    void placeVector(int angle, int power) {
        if (stopwatch->isMoreThanSec(1)) {
            index = (index + 1) % ARR_SIZE;
            arr[index] = 0;
            stopwatch->start();
        }

        arr[index] += constrain(angle, -Mechanics::TURN_MAX_ANGLE, Mechanics::TURN_MAX_ANGLE) /* * (power - 50)*/;
    }

    bool isCounterClockWise() const {
        return sum() > ROTATE_THRESHOLD_VALUE;
    }

    bool isClockWise() const {
        return sum() < -ROTATE_THRESHOLD_VALUE;
    }

private:
    int index = 0;
    int *arr;

    Stopwatch *stopwatch = new Stopwatch();

    int sum() const {
        int sum = 0;
        for (int i = 0; i < ARR_SIZE; ++i) {
            sum += arr[i];
        }
        return sum;
    }
};

#endif
