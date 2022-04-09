#pragma once

#include <TimingSensor.h>
#include <MedianFilterWindow.h>

#ifdef SHARP_SENSORS

#include <ADC.h>
#include <Sharp.h>

#endif

class Sensors {
public:

    Sensors() {
#ifdef FREE_RUN_MODE
        ADC_setup();
#endif
    }


    TimingSensor *right45 = createSensor(
            new Sharp10_150Table(A0, new uint16_t[READING_COUNT]{905, 680, 473, 316, 247, 196, 163, 147, 122}));
    TimingSensor *left45 = createSensor(
            new Sharp10_150Table(A1, new uint16_t[READING_COUNT]{834, 603, 400, 320, 235, 192, 144, 130, 118}));
    TimingSensor *forward = createSensor(
            new Sharp10_150Table(A2, new uint16_t[READING_COUNT]{836, 593, 416, 302, 243, 200, 172, 158, 128}));

    int forwardD = 0;
    int l45d = 0;
    int r45d = 0;

    void read() {
        forwardD = (int) forward->getDistance();
        l45d = (int) left45->getDistance();
        r45d = (int) right45->getDistance();

#ifdef DEBUG
        if (debug.isReady()) {
            Serial.print(millis());
            Serial.print(" left45 = ");
            Serial.print(l45d);
            Serial.print(" forward = ");
            Serial.print(forwardD);
            Serial.print(" right45 = ");
            Serial.print(r45d);
            Serial.println();
        }
#endif
    }


    bool isSamePlace(unsigned long ms) const {
        return forward->isLongerThan(ms)
               || left45->isLongerThan(ms)
               || right45->isLongerThan(ms);
    }

    bool isForwardLongerThan(int ms) const {
        return forward->isLongerThan(ms);
    }
    bool isLeftLongerThan(int ms) const {
        return left45->isLongerThan(ms);
    }
    bool isRightLongerThan(int ms) const {
        return right45->isLongerThan(ms);
    }

private:

    Interval debug = Interval(200);

    static TimingSensor *createSensor(DistanceSensor *sensor) {
        return new TimingSensor((sensor));
//        return new TimingSensor(new MedianFilterWindow(sensor));
    }

};
