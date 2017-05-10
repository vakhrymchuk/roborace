#ifndef VOLTAGE_DIVIDER_H
#define VOLTAGE_DIVIDER_H

#include <KalmanFilterType.h>

class VoltageDivider {
public:
    VoltageDivider(const byte pin, const byte degree) : pin(pin), degree(degree) {
        value.set(readFloat());
    }

    int readSource() const {
        return aRead(pin);
    }

    float readFloat() const {
        return degree * readSource() * 5.0 / 1024;
    }

    float readFloatKalman() {
        return value.update(readFloat());
    }

private:
    const byte pin;
    const byte degree;
    KalmanFilterType<float> value;

};

#endif
