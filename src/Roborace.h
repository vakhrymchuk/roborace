#ifndef ROBORACE_H
#define ROBORACE_H

#include <Arduino.h>
#include "value/TimingValue.h"
#include "mechanics/SensorsHolder.h"
#include "mechanics/Mechanics.h"
#include "strategy/Strategy.h"


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
    Interval debugInterval = Interval(1000);
#endif

private:

    void waitForEngineInit() const {
        delay(1300);
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
        Serial.print(F("\n\nloop time mcs = "));
        Serial.println(finish - start);

        Serial.println(F("\n"));
        Serial.print(F("forwardLeftDistance = "));
        Serial.println(sensors->forwardLeftDistance);
        Serial.print(F("rightDistance = "));
        Serial.println(sensors->rightDistance);
        Serial.print(F("leftDistance = "));
        Serial.println(sensors->leftDistance);
        Serial.print(F("forwardRightDistance = "));
        Serial.println(sensors->forwardRightDistance);
    }
#endif
}

void Roborace::waitForMainLoop() const {
    while (!mainLoopChange->isReady()) {}
}

#endif
