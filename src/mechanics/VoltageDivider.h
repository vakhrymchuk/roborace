#pragma once

#include <mechanics/KalmanFilterType.h>

class VoltageDivider {
public:
    static constexpr float ADC_VOLTAGE = 3.3;
    static const int ADC_BITS = 4096;
    static constexpr float ADC_KOEF = ADC_VOLTAGE / ADC_BITS;

    VoltageDivider(const byte pin, const byte degree) : pin(pin), degree(degree) {
        value.set(readFloat());
        pinMode(pin, INPUT);
    }

    int readSource() const {
        return analogRead(pin);
    }

    float readFloat() const {
        return ADC_KOEF * (float) (degree * readSource());
    }

    float readFloatKalman() {
        float newValue = readFloat();
        if (abs(newValue - value.get()) > 1.0)
            value.set(newValue);
        return value.update(newValue);
    }

private:
    const byte pin;
    const byte degree;
    KalmanFilterType<float> value;

};
