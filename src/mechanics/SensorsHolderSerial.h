#pragma once

#include <Timeout.h>
#include "TimingSerialSensor.h"


class SensorsHolder {

public:

    const int max30 = 200;
    const int max60 = 200;
    const int max90 = 200;

    int f00d = 0;
    int l30d = 0, r30d = 0;
    int l60d = 0, r60d = 0;
    int l90d = 0, r90d = 0;

    int minForwardDistance = 0, maxForwardDistance = 0;
    int minSideDistance = 0, maxSideDistance = 0;
    int maxDistance = 0, minDistance = 0;

    TimingSerialSensor *l90Sensor = new TimingSerialSensor();
    TimingSerialSensor *l60Sensor = new TimingSerialSensor();
    TimingSerialSensor *l30Sensor = new TimingSerialSensor();
    TimingSerialSensor *f00Sensor = new TimingSerialSensor();
    TimingSerialSensor *r30Sensor = new TimingSerialSensor();
    TimingSerialSensor *r60Sensor = new TimingSerialSensor();
    TimingSerialSensor *r90Sensor = new TimingSerialSensor();

    DynamicJsonDocument doc = DynamicJsonDocument(128);

public:

    SensorsHolder() {
        Serial2.begin(115200, SERIAL_8N1, GPIO_NUM_22, GPIO_NUM_21);
    }


    void readDistances();

    bool isSamePlace(unsigned long ms) const;

private:

    void calcMaxDistance();

    void calcMinDistance();

    int minV(int a, int b) {
        return a < b ? a : b;
    }

};

void SensorsHolder::readDistances() {
    Timeout readTimeout(20);
    while (!Serial2.available() && !readTimeout.isReady()) {}

    while (Serial2.available()) {
        const String &data = Serial2.readStringUntil('\n');
//        Serial.printf("sensors data=%s\n", data.c_str());
        deserializeJson(doc, data);
//        serializeJson(doc, Serial);

        l90Sensor->setDistance(minV(doc[0], max90));
        l60Sensor->setDistance(minV(doc[1], max60));
        l30Sensor->setDistance(minV(doc[2], max30));
        f00Sensor->setDistance(minV(doc[3], 200));
        r30Sensor->setDistance(minV(doc[4], max30));
        r60Sensor->setDistance(minV(doc[5], max60));
        r90Sensor->setDistance(minV(doc[6], max90));

        l90d = l90Sensor->getDistance();
        l60d = l60Sensor->getDistance();
        l30d = l30Sensor->getDistance();
        f00d = f00Sensor->getDistance();
        r30d = r30Sensor->getDistance();
        r60d = r60Sensor->getDistance();
        r90d = r90Sensor->getDistance();
//        int fps = doc[7];

        calcMaxDistance();
        calcMinDistance();
    }

}


void SensorsHolder::calcMaxDistance() {
    maxForwardDistance = max(l30d, r30d);
    maxForwardDistance = max(maxForwardDistance, f00d);
    maxSideDistance = max(l60d, r60d);
    maxSideDistance = max(maxSideDistance, l90d);
    maxSideDistance = max(maxSideDistance, r90d);
    maxDistance = max(maxForwardDistance, maxSideDistance);
}

void SensorsHolder::calcMinDistance() {
    minForwardDistance = min(l30d, r30d);
    minForwardDistance = min(minForwardDistance, f00d);
    minSideDistance = min(l60d, r60d);
    minSideDistance = min(minSideDistance, l90d);
    minSideDistance = min(minSideDistance, r90d);
    minDistance = min(minForwardDistance, minSideDistance);
}


bool SensorsHolder::isSamePlace(unsigned long ms) const {
    return f00Sensor->isLongerThan(ms)
           + l90Sensor->isLongerThan(ms)
           + l60Sensor->isLongerThan(ms)
           + l30Sensor->isLongerThan(ms)
           + r30Sensor->isLongerThan(ms)
           + r60Sensor->isLongerThan(ms)
           + r90Sensor->isLongerThan(ms) >= 5;
}

