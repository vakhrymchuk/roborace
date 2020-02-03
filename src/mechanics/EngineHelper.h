#ifndef ROBORACE_ENGINE_HELPER_H
#define ROBORACE_ENGINE_HELPER_H

/**
 *  _____            _            _   _      _
 * |  ___|          (_)          | | | |    | |
 * | |__ _ __   __ _ _ _ __   ___| |_| | ___| |_ __   ___ _ __
 * |  __| '_ \ / _` | | '_ \ / _ \  _  |/ _ \ | '_ \ / _ \ '__|
 * | |__| | | | (_| | | | | |  __/ | | |  __/ | |_) |  __/ |
 * \____/_| |_|\__, |_|_| |_|\___\_| |_/\___|_| .__/ \___|_|
 *              __/ |                         | |
 *             |___/                          |_|
 */
class EngineHelper {
public:

    /**
     * 16*20ms = 320 ms
     */
    static const int SIZE = 20;

    ValueInt *maxCorrectionRun = new ValueInt(12); // 50
    ValueInt *maxCorrectionBrake = new ValueInt(25); // 50
    ValueInt *correctionFactor = new ValueInt(5); // 10

    EngineHelper() {
        for (long & i : arr) {
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

    long arr[SIZE];

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
        int correction = correctionFactor->value * (power - getSpeed()) ;
        if (power > 0) {
            return constrain(correction, -maxCorrectionBrake->value, maxCorrectionRun->value);
        }
        return constrain(correction, -maxCorrectionRun->value, maxCorrectionBrake->value);
    }
};

#endif
