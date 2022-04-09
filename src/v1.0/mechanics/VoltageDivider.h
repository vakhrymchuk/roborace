#pragma once

#include "KalmanFilterType.h"
#include "Interval.h"

class VoltageDivider {
public:
    static constexpr float ADC_VOLTAGE = 3.3;
    static const int ADC_BITS = 4096;
    static constexpr float ADC_KOEF = ADC_VOLTAGE / ADC_BITS;

private:
    const byte pin;
    const byte degree;
    int source = 0;
    KalmanFilterType<float>* value = new KalmanFilterType<float>(0.0, 0.05);
    Interval *readInterval = new Interval(100);

public:
    VoltageDivider(const byte pin, const byte degree) : pin(pin), degree(degree) {
        value->set(readFloat());
        pinMode(pin, INPUT);
    }

    int readSource() {
        if (readInterval->isReady()) {
            source = analogRead(pin);
        }
        return source;
    }

    float readFloat() {
        return ADC_KOEF * (float) (degree * readSource());
    }

    float readFloatKalman() {
        float newValue = readFloat();
        if (abs(newValue - value->get()) > 1.0)
            value->set(newValue);
        return value->update(newValue);
    }

};
