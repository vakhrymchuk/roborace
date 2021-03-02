#pragma once

class Adaptation {
public:

    Param *param;

    explicit Adaptation(Param *param, const int educationTime = 20, const int step = 1) :
            param(param),
            educationTime(educationTime),
            step(step) {
        init();
    }

    void init() {
        stopwatch->start();
        if (testingNewValue) {
            param->value -= step;
        }
        testingNewValue = false;
    }

    int adaptedValue() {
        process();
        return param->value;
    }

private:
    bool testingNewValue = false;
    const int educationTime;
    const int step;

    Stopwatch *stopwatch = new Stopwatch();

    void process() {
        if (stopwatch->isMoreThan(educationTime, SECOND)) {
            testingNewValue = true;
            stopwatch->start();
            param->value += step;
        }
    }

};
