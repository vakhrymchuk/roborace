#ifndef ROBORACE_SENSORS_HOLDER_H
#define ROBORACE_SENSORS_HOLDER_H

//#include <Vl53l0xSensor.h>
//#include <Vl53l1xSensor.h>
#include "KalmanFilter.h"
#include <Sharp.h>
#include "TimingFilter.h"
#include "MedianFilter.h"
#include "MedianFilterWindow.h"
#include "../remote/Message.h"
//#include "TimingFilter.h"

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

    int minForwardDistance, maxForwardDistance;

    int minSideDistance, maxSideDistance;

    int maxDistance, minDistance;



    TimingFilter *forwardLeftSensor;
    TimingFilter *rightSensor;
    TimingFilter *leftSensor;
    TimingFilter *forwardRightSensor;

    SensorsHolder() {
//        initVl53Sensors();
        initSharpSensors();
    }

    void readDistances();

    void setDistances(Message &message);

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

    void initVl53Sensors();

    void initSharpSensors();
};

void SensorsHolder::readDistances() {

    forwardLeftDistance = forwardLeftSensor->getDistance();
    rightDistance = rightSensor->getDistance();
    leftDistance = leftSensor->getDistance();
    forwardRightDistance = forwardRightSensor->getDistance();

    calcMaxDistance();
    calcMinDistance();
}

void SensorsHolder::setDistances(Message &message) {

    forwardLeftDistance = forwardLeftSensor->updateValue(message.forwardLeftDistance);
    rightDistance = rightSensor->updateValue(message.rightDistance);
    leftDistance = leftSensor->updateValue(message.leftDistance);
    forwardRightDistance = forwardRightSensor->updateValue(message.forwardRightDistance);

    calcMaxDistance();
    calcMinDistance();
}


void SensorsHolder::calcMaxDistance() {
    maxForwardDistance = max(forwardLeftDistance, forwardRightDistance);
    maxSideDistance = max(leftDistance, rightDistance);
    maxDistance = max(maxForwardDistance, maxSideDistance);
}

void SensorsHolder::calcMinDistance() {
    minForwardDistance = min(forwardLeftDistance, forwardRightDistance);
    minSideDistance = min(leftDistance, rightDistance);
    minDistance = min(minForwardDistance, minSideDistance);
}


bool SensorsHolder::isSamePlace(unsigned long ms) const {
    return forwardLeftSensor->isLongerThan(ms)
           || forwardRightSensor->isLongerThan(ms)
           || leftSensor->isLongerThan(ms)
           || rightSensor->isLongerThan(ms);
}

void SensorsHolder::initSharpSensors() {
    forwardLeftSensor = createSensor(new Sharp10_150(A3));
    rightSensor = createSensor(new Sharp10_150(A2));
    leftSensor = createSensor(new Sharp10_150(A1));
    forwardRightSensor = createSensor(new Sharp10_150(A0));
}

void SensorsHolder::initVl53Sensors() {
//    Vl53l0xSensor::nextAddress = 0x30;
//    Vl53l1xSensor::nextAddress = 0x40;
//
//    Vl53l1xSensor::lowPin(A0);
//    Vl53l0xSensor::lowPin(A1);
//    Vl53l0xSensor::lowPin(A2);
//    Vl53l1xSensor::lowPin(A3);
//
//    forwardLeftSensor = createSensor(new Vl53l1xSensor(A3));
//    rightSensor = createSensor(new Vl53l0xSensor(A2));
//    leftSensor = createSensor(new Vl53l0xSensor(A1));
//    forwardRightSensor = createSensor(new Vl53l1xSensor(A0));
}

#endif
