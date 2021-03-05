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


byte checkWire(TwoWire &wire) {
    byte working = 0;
    byte count = 0;
//    wire.begin();
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


class SensorsHolder1 {

public:

    static const bool USE_MEDIAN_FILTER = false;
    static const bool USE_KALMAN_FILTER = false;

    int forwardLeftDistance = 0, forwardRightDistance = 0, forwardCenterDistance = 0;
    int leftDistance = 0, rightDistance = 0;
    int left45Distance = 0, right45Distance = 0;

    int minForwardDistance, maxForwardDistance;

    int minSideDistance, maxSideDistance;

    int maxDistance, minDistance;

    unsigned long read0Time, read1Time;

#if defined VL53
    TCA9548A *i2cMux;
#endif
    TimingFilter *forwardRightSensor;
    TimingFilter *forwardLeftSensor;
//    TimingFilter *forwardCenterSensor;
    TimingFilter *rightSensor;
    TimingFilter *leftSensor;
    TimingFilter *right45Sensor;
    TimingFilter *left45Sensor;

public:

    SensorsHolder1() {
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

    unsigned long start = millis();

    forwardRightDistance = forwardRightSensor->getDistance();
    forwardLeftDistance = forwardLeftSensor->getDistance();
    rightDistance = rightSensor->getDistance();
    leftDistance = leftSensor->getDistance();
    right45Distance = right45Sensor->getDistance();
    left45Distance = left45Sensor->getDistance();

    unsigned long read0Sensors = millis();

//    forwardCenterDistance = forwardCenterSensor->getDistance();


    calcMaxDistance();
    calcMinDistance();

    unsigned long finish = millis();

    read0Time = read0Sensors - start;
    read1Time = finish - read0Sensors;
}


void SensorsHolder::calcMaxDistance() {
    maxForwardDistance = max(forwardLeftDistance, forwardRightDistance);
//    maxForwardDistance = max(maxForwardDistance, forwardCenterDistance);
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
//    forwardCenterSensor = createSensor(createSensor1(3));
    rightSensor = createSensor(createSensor0(6));
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
