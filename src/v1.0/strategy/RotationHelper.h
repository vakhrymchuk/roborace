#pragma once

#include "Interval.h"


class RotationHelper {
public:

    /** Время круга */
    static const int ARR_SIZE = 20;
    static const int TURN_ON_SECOND = 200;
    static const int ROTATE_THRESHOLD_VALUE = ARR_SIZE * TURN_ON_SECOND;

    Param *rotationThreshold = new Param(ROTATE_THRESHOLD_VALUE, "rotation-threshold", "main");

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
        if (stopwatch->isMoreThan(1, SECOND)) {
            index = (index + 1) % ARR_SIZE;
            arr[index] = 0;
            stopwatch->start();
        }

        arr[index] += constrain(angle, -Mechanics::TURN_MAX_ANGLE, Mechanics::TURN_MAX_ANGLE) /* * (power - 50)*/;
    }

    bool isCounterClockWise() const {
        return sum() > rotationThreshold->value;
    }

    bool isClockWise() const {
        return sum() < -rotationThreshold->value;
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
