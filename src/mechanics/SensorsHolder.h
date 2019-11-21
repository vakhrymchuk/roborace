#ifndef ROBORACE_SENSORS_HOLDER_H
#define ROBORACE_SENSORS_HOLDER_H

#include <Vl53l0xSensor.h>
#include <Vl53l1xSensor.h>
#include "KalmanFilter.h"
#include "TimingFilter.h"
#include "MedianFilter.h"
#include "MedianFilterWindow.h"
#include "../remote/Message.h"

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

    unsigned int forwardLeftDistance, forwardRightDistance;

    unsigned int leftDistance, rightDistance;

    unsigned int minForwardDistance, maxForwardDistance;

    unsigned int maxDistance, minDistance;

    unsigned int minFactor;


    TimingFilter *forwardLeftSensor;
    TimingFilter *rightSensor;
    TimingFilter *leftSensor;
    TimingFilter *forwardRightSensor;

    SensorsHolder() {
        initSensors();
    }

    void readDistances();

    void setDistances(Message &message);

    bool isSamePlace(unsigned long ms) const;

    void initSensors();


private:

    void calcMaxDistance();

    void calcMinDistance();

    static TimingFilter *createSensor(DistanceSensor *sensor) {
        DistanceSensor *distanceSensor = sensor;
        if (USE_MEDIAN_FILTER) {
            distanceSensor = new MedianFilter(distanceSensor, MedianFilter::ARR_SIZE);
        }
        if (USE_KALMAN_FILTER) {
            distanceSensor = new KalmanFilter(distanceSensor);
        }
        return new TimingFilter(distanceSensor);
    }

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

    maxDistance = max(maxForwardDistance, leftDistance);
    maxDistance = max(maxDistance, rightDistance);
}

void SensorsHolder::calcMinDistance() {
    minForwardDistance = min(forwardLeftDistance, forwardRightDistance);

    minDistance = min(minForwardDistance, leftDistance);
    minDistance = min(minDistance, rightDistance);

    minFactor = min(minForwardDistance, 2 * leftDistance);
    minFactor = min(minFactor, 2 * rightDistance);
}


bool SensorsHolder::isSamePlace(unsigned long ms) const {
    return forwardLeftSensor->isLongerThan(ms)
           || forwardRightSensor->isLongerThan(ms)
           || leftSensor->isLongerThan(ms)
           || rightSensor->isLongerThan(ms);
}

void SensorsHolder::initSensors() {
    Vl53l0xSensor::nextAddress = 0x30;
    Vl53l1xSensor::nextAddress = 0x40;

    Vl53l1xSensor::lowPin(A0);
    Vl53l0xSensor::lowPin(A1);
    Vl53l0xSensor::lowPin(A2);
    Vl53l1xSensor::lowPin(A3);

    forwardLeftSensor = createSensor(new Vl53l1xSensor(A3));
    rightSensor = createSensor(new Vl53l0xSensor(A2));
    leftSensor = createSensor(new Vl53l0xSensor(A1));
    forwardRightSensor = createSensor(new Vl53l1xSensor(A0));

}

#endif
