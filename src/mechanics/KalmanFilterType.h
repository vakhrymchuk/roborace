#pragma once

template<typename T>
class KalmanFilterType {
public:

    static constexpr float KALMAN_FILTER_TYPE_FACTOR = 0.05;

    explicit KalmanFilterType(T prevValue = 0) : prevValue(prevValue) {
    }

    T update(const T value) {
        return prevValue = (value * KALMAN_FILTER_TYPE_FACTOR + (1 - KALMAN_FILTER_TYPE_FACTOR) * prevValue);
    }

    T get() {
        return prevValue;
    }

    T set(const T value) {
        return prevValue = value;
    }

private:
    T prevValue = 0;
};
