#pragma once

#include <TimingSensor.h>
#include <MedianFilterWindow.h>

#ifdef SHARP_SENSORS


#include <Sharp.h>

#endif

class Sensors {
public:
    TimingSensor *left45 = createSensor(
            new Sharp10_150Table(A0, new uint16_t[READING_COUNT]{625, 387, 295, 214, 167, 145, 132, 115, 114}));
    TimingSensor *right45 = createSensor(
            new Sharp10_150Table(A1, new uint16_t[READING_COUNT]{612, 359, 276, 194, 145, 121, 97, 80, 75}));
    TimingSensor *left0 = createSensor(
            new Sharp10_150Table(A6, new uint16_t[READING_COUNT]{611, 350, 255, 177, 130, 103, 78, 60, 49}));
    TimingSensor *right0 = createSensor(
            new Sharp10_150Table(A7, new uint16_t[READING_COUNT]{620, 360, 268, 183, 133, 108, 81, 62, 51}));
    TimingSensor *forward = createSensor(
            new Sharp100_500Table(A2, new uint16_t[READING_COUNT]{627, 556, 507, 379, 327, 302, 287, 277, 270}));

    int forwardD = 0;
    int r0d = 0;
    int l0d = 0;
    int l45d = 0;
    int r45d = 0;

    void read() {
        forwardD = (int) forward->getDistance();
        r0d = (int) right0->getDistance();
        l0d = (int) left0->getDistance();
        l45d = (int) left45->getDistance();
        r45d = (int) right45->getDistance();

#ifdef DEBUG
        if (debug.isReady()) {
            Serial.print(millis());
            Serial.print(" left45 = ");
            Serial.print(l45d);
            Serial.print(" left0 = ");
            Serial.print(l0d);
            Serial.print(" forward = ");
            Serial.print(forwardD);
            Serial.print(" right0 = ");
            Serial.print(r0d);
            Serial.print(" right45 = ");
            Serial.print(r45d);
            Serial.println();
        }
#endif
    }


    bool isSamePlace(unsigned long ms) const {
        return right0->isLongerThan(ms) || left0->isLongerThan(ms);
    }

    bool isForwardLongerThan(int ms) const {
        return forward->isLongerThan(ms);
    }

    bool isLeftLongerThan(int ms) const {
        return left0->isLongerThan(ms);
    }

    bool isRightLongerThan(int ms) const {
        return right0->isLongerThan(ms);
    }

private:

    Interval debug = Interval(200);

    static TimingSensor *createSensor(DistanceSensor *sensor) {
//        return new TimingSensor((sensor));
        return new TimingSensor(new MedianFilterWindow(sensor));
    }

};
