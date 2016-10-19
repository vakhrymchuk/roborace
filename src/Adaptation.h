#ifndef ROBORACE_ADAPTATION_H
#define ROBORACE_ADAPTATION_H

class Adaptation {
public:

    ValueInt *valueInt;

    Adaptation(int value = 0, int educationTime = 10, int step = 1) : educationTime(educationTime), step(step) {
        valueInt = new ValueInt(value);
    }

    void init() {
        stopwatch->start();
        testValue = valueInt->value;
    }

    int adaptedValue() {
        process();
        return testValue;
    }

    bool adapted = false;

private:
    int testValue;
    int educationTime;
    int step;

    Stopwatch *stopwatch = new Stopwatch();

    void process() {
        if (stopwatch->isMoreThanSec((const unsigned long) educationTime)) {
            if (testValue > valueInt->value) {
                valueInt->value = testValue;
                adapted = true;
            }
            stopwatch->start();
            testValue += step;
        }
    }

};

#endif