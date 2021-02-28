#ifndef VOLTAGE_DIVIDER_H
#define VOLTAGE_DIVIDER_H

#include <KalmanFilterType.h>

class VoltageDivider {
public:
    static constexpr double ADC_VOLTAGE = 3.3;

    VoltageDivider(const byte pin, const byte degree) : pin(pin), degree(degree) {
        value.set(readFloat());
    }

    int readSource() const {
        return analogRead(pin);
    }

    float readFloat() const {
        return degree * readSource() * ADC_VOLTAGE / 1024;
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
