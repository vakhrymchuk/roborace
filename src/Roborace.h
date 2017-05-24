#ifndef ROBORACE_H
#define ROBORACE_H

#include <Arduino.h>
#include "value/TimingValue.h"
#include "mechanics/SensorsHolder.h"
#include "mechanics/Mechanics.h"
#include "strategy/Strategy.h"
#include "strategy/Forward.h"
#include "strategy/Backward.h"
#include "strategy/Rotate.h"
#include "strategy/RightWall.h"


/**
 * ______      _
 * | ___ \    | |
 * | |_/ /___ | |__   ___  _ __ __ _  ___ ___
 * |    // _ \| '_ \ / _ \| '__/ _` |/ __/ _ \
 * | |\ \ (_) | |_) | (_) | | | (_| | (_|  __/
 * \_| \_\___/|_.__/ \___/|_|  \__,_|\___\___|
 */
class Roborace {
public:

    static const int RUN_INTERVAL_MS = 10;

    Roborace() {
        waitForEngineInit();
        initStrategies();
    }

    virtual void loop();

protected:

    Interval *mainLoopChange = new Interval(RUN_INTERVAL_MS);

    SensorsHolder *sensors = new SensorsHolder();

    Mechanics *mechanics = new Mechanics();

    Forward *forward = new Forward;
    Backward *backward = new Backward;
    Rotate *rotate = new Rotate;
    RightWall *rightWall = new RightWall;
    Strategy *activeStrategy;

    boolean enabled = true;

    void waitForMainLoop() const;

#ifdef DEBUG
    Interval debugInterval = Interval(200);
#endif

private:

    void waitForEngineInit() const {
        delay(1500);
    }

    void initStrategies();
};


void Roborace::initStrategies() {
    forward->backward = backward;
    forward->rotate = rotate;
    forward->rightWall = rightWall;

    backward->forward = forward;

    rotate->forward = forward;

    rightWall->forward = forward;

    activeStrategy = forward->init();
//    activeStrategy = rightWall->init();
}


void Roborace::loop() {

    waitForMainLoop();

#ifdef DEBUG
    unsigned long start = micros();
#endif

    sensors->readDistances();
    activeStrategy = activeStrategy->check(sensors);
    activeStrategy->calc(sensors);
    activeStrategy->run(mechanics);

#ifdef DEBUG
    if (debugInterval.isReady()) {
        unsigned long finish = micros();
        Serial.print(F("loop time mcs = "));
        Serial.print(finish - start);

        Serial.print(F("\tFL = "));
        Serial.print(sensors->forwardLeftDistance);
        Serial.print(F("\tL = "));
        Serial.print(sensors->leftDistance);
        Serial.print(F("\tR = "));
        Serial.print(sensors->rightDistance);
        Serial.print(F("\tFR = "));
        Serial.print(sensors->forwardRightDistance);
//        Serial.print(F("\tLV = "));
//        Serial.print(mechanics->logicVoltage.readFloatKalman());
//        Serial.print(F("\tEV = "));
//        Serial.print(mechanics->engineVoltage.readFloatKalman());
        Serial.println();
    }
#endif
}

void Roborace::waitForMainLoop() const {
    while (!mainLoopChange->isReady()) {}
}

#endif
