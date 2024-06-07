#pragma once

#include "Param.h"

class IntervalValue {
public:

    explicit IntervalValue(Param *param) : param(param) {}

    bool isReady() {
        bool ready = millis() >= msReady;
        if (ready) {
            recalculate();
        }
        return ready;
    }

    void recalculate() {
        while (msReady <= millis())
            msReady += param->value;
    }

    Param *getParam() {
        return (Param *) param;
    }

private:
    const Param *param;
    unsigned long msReady = 0;
};
