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
    Strategy *activeStrategy;

    boolean enabled = true;

private:

    void waitForEngineInit() {
        delay(1300);
    }

    void initStrategies();
};


void Roborace::initStrategies() {
    forward->backward = backward;
    forward->rotate = rotate;

    backward->forward = forward;

    rotate->forward = forward;

    activeStrategy = forward->init();
}


void Roborace::loop() {

    while (!mainLoopChange->isReady()) {
        // play buzzer!
    }

#ifdef DEBUG
    unsigned long start = micros();
#endif

    sensors->readDistances();
    activeStrategy = activeStrategy->check(sensors);
    activeStrategy->calc(sensors);
    activeStrategy->run(mechanics);

#ifdef DEBUG
    unsigned long finish = micros();
    Serial.print("\n\n\nloop time mcs = ");
    Serial.println(finish - start);
#endif
}

#endif
