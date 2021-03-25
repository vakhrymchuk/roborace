#pragma once

template<typename T>
class KalmanFilterType {
public:

    static constexpr float DEFAULT_FACTOR = 0.1;

    explicit KalmanFilterType(T prevValue = 0, const float factor = DEFAULT_FACTOR) :
            prevValue(prevValue), factor(factor) {
    }

    T update(const T value) {
        return prevValue = (value * factor + (1 - factor) * prevValue);
    }

    T get() {
        return prevValue;
    }

    T set(const T value) {
        return prevValue = value;
    }

private:
    T prevValue = 0;
    const float factor;
};
