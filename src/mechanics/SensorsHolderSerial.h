#pragma once

class SensorsHolder {

public:

    int forwardLeftDistance = 0, forwardRightDistance = 0, forwardCenterDistance = 0;
    int leftDistance = 0, rightDistance = 0;
    int left45Distance = 0, right45Distance = 0;

    int minForwardDistance = 0, maxForwardDistance = 0;

    int minSideDistance = 0, maxSideDistance = 0;

    int maxDistance = 0, minDistance = 0;

    DynamicJsonDocument doc = DynamicJsonDocument(128);

public:

    SensorsHolder() {
        Serial2.begin(115200, SERIAL_8N1, GPIO_NUM_32, GPIO_NUM_33);
    }


    void readDistances();

    bool isSamePlace(unsigned long ms) const;

private:

    void calcMaxDistance();

    void calcMinDistance();

};

void SensorsHolder::readDistances() {

    while (Serial2.available()) {
        const String &data = Serial2.readStringUntil('\n');
//        Serial.printf("sensors data=%s\n", data.c_str());
        deserializeJson(doc, data);
//        serializeJson(doc, Serial);

        leftDistance = doc[0];
        left45Distance = doc[1];
        forwardLeftDistance = doc[2];
        forwardRightDistance = doc[3];
        right45Distance = doc[4];
        rightDistance = doc[5];

        calcMaxDistance();
        calcMinDistance();
    }

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
    return false;
//    return forwardLeftSensor->isLongerThan(ms)
//           || forwardRightSensor->isLongerThan(ms)
//           || leftSensor->isLongerThan(ms)
//           || rightSensor->isLongerThan(ms);
}

