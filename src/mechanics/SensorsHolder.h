#pragma once

#include "KalmanFilter.h"
#include "TimingFilter.h"
#include "MedianFilter.h"
#include "MedianFilterWindow.h"

#if defined VL53

#include <TCA9548A.h>
#include <Vl53l0xSensorI2cMux.h>
#include <Vl53l1xSensorI2cMux.h>

#elif defined SHARP_SENSORS

#include <ADC.h>
#include <Sharp.h>

#endif

/**
 *  _____                                _   _       _     _
 * /  ___|                              | | | |     | |   | |
 * \ `--.  ___ _ __  ___  ___  _ __ ___ | |_| | ___ | | __| | ___ _ __
 *  `--. \/ _ \ '_ \/ __|/ _ \| '__/ __||  _  |/ _ \| |/ _` |/ _ \ '__|
 * /\__/ /  __/ | | \__ \ (_) | |  \__ \| | | | (_) | | (_| |  __/ |
 * \____/ \___|_| |_|___/\___/|_|  |___/\_| |_/\___/|_|\__,_|\___|_|
 *
 */
class SensorsHolder {

public:

    static const bool USE_MEDIAN_FILTER = false;
    static const bool USE_KALMAN_FILTER = false;

    int forwardLeftDistance, forwardRightDistance;
    int leftDistance, rightDistance;
    int left45Distance, right45Distance;

    int minForwardDistance, maxForwardDistance;

    int minSideDistance, maxSideDistance;

    int maxDistance, minDistance;

#if defined VL53
    TCA9548A i2cMux;
#endif
    TimingFilter *forwardRightSensor;
    TimingFilter *forwardLeftSensor;
    TimingFilter *rightSensor;
    TimingFilter *leftSensor;
    TimingFilter *right45Sensor;
    TimingFilter *left45Sensor;

    SensorsHolder() {
        initSensors();
    }

    void readDistances();

    bool isSamePlace(unsigned long ms) const;

private:

    void calcMaxDistance();

    void calcMinDistance();

    static TimingFilter *createSensor(DistanceSensor *sensor) {
        DistanceSensor *distanceSensor = sensor;
        if (USE_MEDIAN_FILTER) {
            distanceSensor = new MedianFilterWindow(distanceSensor, MedianFilter::ARR_SIZE);
        }
        if (USE_KALMAN_FILTER) {
            distanceSensor = new KalmanFilter(distanceSensor);
        }
        return new TimingFilter(distanceSensor);
    }

    void initSensors();

};

void SensorsHolder::readDistances() {

    forwardRightDistance = forwardRightSensor->getDistance();
    forwardLeftDistance = forwardLeftSensor->getDistance();
    rightDistance = rightSensor->getDistance();
    leftDistance = leftSensor->getDistance();
    right45Distance = right45Sensor->getDistance();
    left45Distance = left45Sensor->getDistance();

    calcMaxDistance();
    calcMinDistance();
}


void SensorsHolder::calcMaxDistance() {
    maxForwardDistance = max(forwardLeftDistance, forwardRightDistance);
    maxSideDistance = max(left45Distance, right45Distance);
    maxDistance = max(maxForwardDistance, maxSideDistance);
}

void SensorsHolder::calcMinDistance() {
    minForwardDistance = min(forwardLeftDistance, forwardRightDistance);
    minSideDistance = min(left45Distance, right45Distance);
    minDistance = min(minForwardDistance, minSideDistance);
}


bool SensorsHolder::isSamePlace(unsigned long ms) const {
    return forwardLeftSensor->isLongerThan(ms)
           || forwardRightSensor->isLongerThan(ms)
           || leftSensor->isLongerThan(ms)
           || rightSensor->isLongerThan(ms);
}


void SensorsHolder::initSensors() {
#if defined FREE_RUN_MODE
    ADC_setup();
#endif
#if defined SHARP_SENSORS
    forwardLeftSensor = createSensor(new Sharp10_150(A3));
    rightSensor = createSensor(new Sharp10_150(A2));
    leftSensor = createSensor(new Sharp10_150(A1));
    forwardRightSensor = createSensor(new Sharp10_150(A0));
#elif defined VL53
    i2cMux.begin(Wire);
    i2cMux.closeAll();
    forwardRightSensor = createSensor(new Vl53l1xSensorI2cMux(0, &i2cMux));
    forwardLeftSensor = createSensor(new Vl53l1xSensorI2cMux(1, &i2cMux));
    rightSensor = createSensor(new Vl53l0xSensorI2cMux(2, &i2cMux));
    leftSensor = createSensor(new Vl53l0xSensorI2cMux(3, &i2cMux));
    right45Sensor = createSensor(new Vl53l0xSensorI2cMux(4, &i2cMux));
    left45Sensor = createSensor(new Vl53l0xSensorI2cMux(5, &i2cMux));
#endif
}
