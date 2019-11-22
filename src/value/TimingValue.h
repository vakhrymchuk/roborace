#ifndef INTERVAL_VALUE_H
#define INTERVAL_VALUE_H

#include "ValueBase.h"

class IntervalValue {
public:

    explicit IntervalValue(ValueInt *value) : value(value) { }

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

    ValueInt *getValueInt() {
        return (ValueInt *) value;
    }

private:
    const ValueInt *value;
    unsigned long msReady = 0;
};


class TimeoutValue {
public:

    TimeoutValue(ValueInt *value) : value(value) { }

    bool isReady() const {
        return millis() >= msReady;
    }

    void start(unsigned short timeout) {
        msReady = millis() + timeout;
    }

    void start() {
        msReady = millis() + value->value;
    }

    ValueInt *getValueInt() {
        return (ValueInt *) value;
    }

private:
    const ValueInt *value;

    unsigned long msReady = 0;
};

#endif
