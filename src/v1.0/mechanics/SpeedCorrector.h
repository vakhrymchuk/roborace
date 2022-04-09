#pragma once


class SpeedCorrector {
public:

    /** 20*20ms = 400 ms */
    static const int SIZE = 20;

    Param *maxCorrectionRun = new Param(10, "correction-run", "mechanics");
    Param *maxCorrectionBrake = new Param(10, "correction-brake", "mechanics");
    Param *correctionFactor = new Param(1, "correction-factor", "mechanics");

    SpeedCorrector() {
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
        return constrain(correction, -maxCorrectionBrake->value, maxCorrectionRun->value);
    }
};
