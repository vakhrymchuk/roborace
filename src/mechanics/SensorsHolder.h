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


byte checkWire(TwoWire wire) {
    byte working = 0;
    byte count = 0;
    wire.begin();
//    for (byte i = 8; i < 120; i++) {
    for (byte i = 0x28; i < 0x71; i++) {
        wire.beginTransmission(i);
        if (wire.endTransmission() == 0) {
//            Serial.print("Found address: ");
            Serial.print(i, DEC);
            Serial.print(" (0x");
            Serial.print(i, HEX);
            Serial.print(") ");
            count++;
            delay(10);
            working = i;
        }
    }
//    Serial.println("Done.");
    Serial.print("Found ");
    Serial.print(count, DEC);
    Serial.println(" device(s).");
//    wire.end();
    return working;
}

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

    int forwardLeftDistance, forwardRightDistance, forwardCenterDistance;
    int leftDistance, rightDistance;
    int left45Distance, right45Distance;

    int minForwardDistance, maxForwardDistance;

    int minSideDistance, maxSideDistance;

    int maxDistance, minDistance;

#if defined VL53
    TCA9548A *i2cMux;
#endif
    TimingFilter *forwardRightSensor;
    TimingFilter *forwardLeftSensor;
    TimingFilter *forwardCenterSensor;
    TimingFilter *rightSensor;
    TimingFilter *leftSensor;
    TimingFilter *right45Sensor;
    TimingFilter *left45Sensor;

public:

    SensorsHolder() {
        createSensors();
    }

    void createSensors();

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

    Vl53l1xSensorI2cMux *createSensor1(byte channel) const;

    Vl53l0xSensorI2cMux *createSensor0(byte channel) const;
};

void SensorsHolder::readDistances() {

//    checkWire(Wire);

    forwardRightDistance = forwardRightSensor->getDistance();
    forwardLeftDistance = forwardLeftSensor->getDistance();
    forwardCenterDistance = forwardCenterSensor->getDistance();
    rightDistance = rightSensor->getDistance();
    leftDistance = leftSensor->getDistance();
    right45Distance = right45Sensor->getDistance();
    left45Distance = left45Sensor->getDistance();

    calcMaxDistance();
    calcMinDistance();
}


void SensorsHolder::calcMaxDistance() {
    maxForwardDistance = max(forwardLeftDistance, forwardRightDistance);
    maxForwardDistance = max(maxForwardDistance, forwardCenterDistance);
    maxSideDistance = max(left45Distance, right45Distance);
    maxDistance = max(maxForwardDistance, maxSideDistance);
}

void SensorsHolder::calcMinDistance() {
    minForwardDistance = min(forwardLeftDistance, forwardRightDistance);
//    minForwardDistance = min(minForwardDistance, forwardCenterDistance);
    minSideDistance = min(left45Distance, right45Distance);
    minDistance = min(minForwardDistance, minSideDistance);
}


bool SensorsHolder::isSamePlace(unsigned long ms) const {
    return forwardLeftSensor->isLongerThan(ms)
           || forwardRightSensor->isLongerThan(ms)
           || leftSensor->isLongerThan(ms)
           || rightSensor->isLongerThan(ms);
}


void SensorsHolder::createSensors() {
#if defined FREE_RUN_MODE
    ADC_setup();
#endif
#if defined SHARP_SENSORS
    forwardLeftSensor = createSensor(new Sharp10_150(A3));
    rightSensor = createSensor(new Sharp10_150(A2));
    leftSensor = createSensor(new Sharp10_150(A1));
    forwardRightSensor = createSensor(new Sharp10_150(A0));
#elif defined VL53
//    checkWire(Wire);

    i2cMux = new TCA9548A();
    i2cMux->begin(Wire);
    i2cMux->closeAll();
    forwardRightSensor = createSensor(createSensor0(5));
    forwardLeftSensor = createSensor(createSensor0(1));
    forwardCenterSensor = createSensor(createSensor1(3));
    rightSensor = createSensor(createSensor0(7));
    right45Sensor = createSensor(createSensor0(2));
    left45Sensor = createSensor(createSensor0(4));
    leftSensor = createSensor(createSensor0(0));
#endif
#ifdef DEBUG
    Serial.println(F("Sensors inited"));
#endif
}

Vl53l0xSensorI2cMux *SensorsHolder::createSensor0(const byte channel) const {
    auto *sensor = new Vl53l0xSensorI2cMux(channel, i2cMux);
    sensor->initSensor();
    return sensor;
}

Vl53l1xSensorI2cMux *SensorsHolder::createSensor1(const byte channel) const {
    auto *sensor = new Vl53l1xSensorI2cMux(channel, i2cMux);
    sensor->initSensor();
    return sensor;
}
