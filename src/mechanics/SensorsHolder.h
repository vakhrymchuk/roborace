#ifndef ROBORACE_SENSORS_HOLDER_H
#define ROBORACE_SENSORS_HOLDER_H

#include "KalmanFilter.h"
#include "TimingFilter.h"
#include "MedianFilter.h"
#include "Sharp.h"
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

    unsigned int forwardLeftDistance, forwardRightDistance;

    unsigned int leftDistance, rightDistance;

    unsigned int minForwardDistance, maxForwardDistance;

    unsigned int maxDistance, minDistance;

    unsigned int minFactor;


    TimingFilter *forwardLeftSensor = createSensor(A3);
    TimingFilter *rightSensor = createSensor(A2);
    TimingFilter *leftSensor = createSensor(A1);
    TimingFilter *forwardRightSensor = createSensor(A0);


    void readDistances();

    void setDistances(Message &message);

    bool isSamePlace(unsigned long ms) const;


private:

    void calcMaxDistance();

    void calcMinDistance();

    static TimingFilter *createSensor(const byte pin) {
        DistanceSensor *distanceSensor = new Sharp10_150(pin);
//        distanceSensor = new MedianFilter(distanceSensor, MedianFilter::ARR_SIZE);
//        distanceSensor = new KalmanFilter(distanceSensor);
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

#endif
