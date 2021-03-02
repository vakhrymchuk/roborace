#pragma once

#include "Param.h"

class IntervalValue {
public:

    explicit IntervalValue(Param *value) : value(value) {}

    bool isReady() {
        bool ready = millis() >= msReady;
        if (ready) {
            recalculate();
        }
        return ready;
    }

    void recalculate() {
        msReady += value->value;
    }

    Param *getParam() {
        return (Param *) value;
    }

private:
    const Param *value;
    unsigned long msReady = 0;
};
