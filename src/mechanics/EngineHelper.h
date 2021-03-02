#pragma once


class EngineHelper {
public:

    /** 20*20ms = 400 ms */
    static const int SIZE = 20;

    Param *maxCorrectionRun = new Param(5, "correction-run", "mechanics");
    Param *maxCorrectionBrake = new Param(5, "correction-brake", "mechanics");
    Param *correctionFactor = new Param(5, "correction-factor", "mechanics");

    EngineHelper() {
        for (int &i : arr) {
            i = 0;
        }
    }

    int get(int power) {

        int result = power + getCorrection(power);

        updateSum(power);

        replaceElement(power);

        return result;
    }

    int getSpeed() const {
        return sum / SIZE;
    }

private:

    int arr[SIZE]{};

    int sum = 0;

    int currentIndex = 0;

    void updateSum(int power) {
        sum += power - arr[currentIndex];
    }

    void replaceElement(int power) {
        arr[currentIndex] = power;
        currentIndex = (currentIndex + 1) % SIZE;
    }

    int getCorrection(int power) const {
        int correction = correctionFactor->value * (power - getSpeed());
        if (power > 0) {
            return constrain(correction, -maxCorrectionBrake->value, maxCorrectionRun->value);
        }
        return constrain(correction, -maxCorrectionRun->value, maxCorrectionBrake->value);
    }
};
