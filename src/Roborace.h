#ifndef ROBORACE_H
#define ROBORACE_H

#include <Arduino.h>
#include "value/TimingValue.h"
#include "mechanics/SensorsHolder.h"
#include "mechanics/Mechanics.h"
#include "strategy/Strategy.h"
#include "strategy/Forward.h"
#include "strategy/Turbo.h"
#include "strategy/Backward.h"
#include "strategy/Rotate.h"
#include "strategy/RightWall.h"
#include "strategy/LeftWall.h"


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

    static const int RUN_INTERVAL_MS = 20;

    Roborace() {
        waitForEngineInit();
        initStrategies();
    }

    virtual void loop();

protected:

    Interval *mainLoopChange = new Interval(RUN_INTERVAL_MS);

#ifndef JOYSTICK_ENABLE
    Mechanics *mechanics = new Mechanics();
#endif

    SensorsHolder *sensors = new SensorsHolder();

    Forward *forward = new Forward;
    Turbo *turbo = new Turbo;
    Backward *backward = new Backward;
    Rotate *rotate = new Rotate;
    RightWall *rightWall = new RightWall;
    LeftWall *leftWall = new LeftWall;
    Strategy *activeStrategy;

    boolean enabled = true;

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
    forward->turbo = turbo;
    forward->backward = backward;
    forward->rotate = rotate;
    forward->rightWall = rightWall;
    forward->leftWall = leftWall;

    turbo->forward = forward;

    backward->forward = forward;

    rotate->forward = forward;

    rightWall->forward = forward;
    leftWall->forward = forward;

    activeStrategy = forward->init(nullptr, 1000);
//    activeStrategy = rightWall->init();
//    activeStrategy = leftWall->init(leftWall);
}


void Roborace::loop() {

    if (!mainLoopChange->isReady()) return;

#ifdef DEBUG
    unsigned long start = micros();
#endif

    sensors->readDistances();
//    if (sensors->isSamePlace(6000)) {
//        mechanics->stop();
//        sensors->initSensors();
//    }
    activeStrategy = activeStrategy->check(sensors);
    activeStrategy->calc(sensors);
#ifndef JOYSTICK_ENABLE
    activeStrategy->run(mechanics);
#endif

#ifdef DEBUG
    unsigned long finish = micros();
    if (debugInterval.isReady()) {
        char buffer[100];
        sprintf(buffer,
                "loop mcs =%4lu  FL =%3u  L =%3u  R =%3u  FR =%3u   ang =% 4d  pow =% 4d minfrd =% 4d",
                finish - start,
                sensors->forwardLeftDistance,
                sensors->leftDistance,
                sensors->rightDistance,
                sensors->forwardRightDistance,
                activeStrategy->angle,
                activeStrategy->power,
                sensors->minForwardDistance);
        Serial.println(buffer);
        Serial.flush();
    }
#endif
}

#endif
