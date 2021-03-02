#pragma once

#include <Arduino.h>
#include "value/ValueBase.h"
#include "mechanics/SensorsHolder.h"
#include "mechanics/Mechanics.h"
#include "strategy/Forward.h"
#include "strategy/Turbo.h"
#include "strategy/Backward.h"
#include "strategy/Rotate.h"
#include "strategy/RightWall.h"
#include "strategy/LeftWall.h"


class Roborace {
public:

    static const int RUN_INTERVAL_MS = 20;
    unsigned int fpsCounter = 0;
    unsigned int fpsLastValue = 0;

    Roborace() {
        mechanics->init();
        initStrategies();
    }

    virtual void loop();

protected:

    IntervalValue *mainLoopChange = new IntervalValue(new Param(RUN_INTERVAL_MS, "main-interval", "main"));

    Mechanics *mechanics = new Mechanics();

    SensorsHolder *sensors = new SensorsHolder();

    Forward *forward = new Forward;
    Turbo *turbo = new Turbo;
    Backward *backward = new Backward;
    Rotate *rotate = new Rotate;
//    RightWall *rightWall = new RightWall;
//    LeftWall *leftWall = new LeftWall;
    Strategy *activeStrategy = forward;

    boolean enabled = true;

    Interval *fpsInterval = new Interval(1, SECOND);
#ifdef DEBUG
    IntervalValue *debugInterval = new IntervalValue(new Param(200, "debug-interval", "main"));
#endif

private:

    void initStrategies();
};


void Roborace::initStrategies() {
    forward->turbo = turbo;
    forward->backward = backward;
    forward->rotate = rotate;
//    forward->rightWall = rightWall;
//    forward->leftWall = leftWall;

    turbo->forward = forward;

    backward->forward = forward;

    rotate->forward = forward;

//    rightWall->forward = forward;
//    leftWall->forward = forward;

    activeStrategy = forward->init(nullptr, 1000);
//    activeStrategy = rightWall->init();
//    activeStrategy = leftWall->init(leftWall);
}


void Roborace::loop() {

    if (!mainLoopChange->isReady()) return;

    fpsCounter++;
    if (fpsInterval->isReady()) {
        fpsLastValue = fpsCounter;
        fpsCounter = 0;
    }


    unsigned long start = millis();

    sensors->readDistances();

//    if (sensors->isSamePlace(6000)) {
//        mechanics->stop();
//        sensors->createSensors();
//    }
    activeStrategy = activeStrategy->check(sensors);
    activeStrategy->calc(sensors);
    activeStrategy->run(mechanics);

    unsigned long finish = millis();
    unsigned long loopTime = finish - start;

#ifdef DEBUG

    if (debugInterval->isReady()) {

        Serial.printf(
                "loop fps = %u ms = %4lu read0 = %4lu read1 = %4lu    L =%3u  L45 =%3u  FL =%3u  FC =%3u  FR =%3u  R45 =%3u  R =%3u   ang =% 4d  pow =% 4d  v=%.2f\n",
                fpsLastValue,
                loopTime,
                sensors->read0Time,
                sensors->read1Time,
                sensors->leftDistance,
                sensors->left45Distance,
                sensors->forwardLeftDistance,
                sensors->forwardCenterDistance,
                sensors->forwardRightDistance,
                sensors->right45Distance,
                sensors->rightDistance,
                activeStrategy->angle,
                activeStrategy->power,
                mechanics->battery.readFloatKalman()
        );
    }
#endif
}
