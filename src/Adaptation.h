#pragma once

class Adaptation {
public:

    explicit Adaptation(Param *param,
                        const int min, const int max,
                        const int educationTime = 20, const int step = 1,
                        const int errorsThresholdInc = 1, const int errorsThresholdDec = 3) :
            param(param),
            min(min),
            max(max),
            educationTime(educationTime),
            step(step),
            errorsThresholdInc(errorsThresholdInc),
            errorsThresholdDec(errorsThresholdDec) {
        init();
    }

    void init() {
        stopwatch->start();
        errorsCount++;
        process();
    }

    int adaptedValue() {
        process();
        return param->value;
    }

private:
    Param *param;
    const int min, max;
    const int educationTime;
    const int step;
    const int errorsThresholdInc, errorsThresholdDec;

    int errorsCount = 0;

    Stopwatch *stopwatch = new Stopwatch();

    void process() {
        if (stopwatch->isMoreThan(educationTime, SECOND) || errorsCount >= errorsThresholdDec) {
            if (errorsCount >= errorsThresholdDec && param->value > min) {
                param->value -= step;
            } else if (errorsCount <= errorsThresholdInc && param->value < max) {
                param->value += step;
            }
            errorsCount = 0;
            param->value = constrain(param->value, min, max);
            stopwatch->start();
        }
    }

};
