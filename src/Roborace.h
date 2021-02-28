#ifndef ROBORACE_H
#define ROBORACE_H

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
    unsigned int fps = 0;

    Roborace() {
        mechanics->init();
        initStrategies();
    }

    virtual void loop();

protected:

    IntervalValue *mainLoopChange = new IntervalValue(new ValueInt(RUN_INTERVAL_MS));

    Mechanics *mechanics = new Mechanics();

    SensorsHolder *sensors = new SensorsHolder();

    Forward *forward = new Forward;
    Turbo *turbo = new Turbo;
    Backward *backward = new Backward;
    Rotate *rotate = new Rotate;
//    RightWall *rightWall = new RightWall;
//    LeftWall *leftWall = new LeftWall;
    Strategy *activeStrategy;

    boolean enabled = true;

#ifdef DEBUG
    Interval debugInterval = Interval(200);
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

    fps++;
#ifdef DEBUG
    unsigned long start = micros();
#endif

    sensors->readDistances();
    unsigned long finish = micros();

//    if (sensors->isSamePlace(6000)) {
//        mechanics->stop();
//        sensors->createSensors();
//    }
    activeStrategy = activeStrategy->check(sensors);
    activeStrategy->calc(sensors);
    activeStrategy->run(mechanics);

#ifdef DEBUG

    if (debugInterval.isReady()) {
        char buffer[200];
        fps = fps * 1000 / debugInterval.getInterval();
        sprintf(buffer,
                "loop fps = %u mcs = %4lu  L =%3u  L45 =%3u  FL =%3u  FC =%3u  FR =%3u  R45 =%3u  R =%3u   ang =% 4d  pow =% 4d",
                fps,
                finish - start,
                sensors->leftDistance,
                sensors->left45Distance,
                sensors->forwardLeftDistance,
                sensors->forwardCenterDistance,
                sensors->forwardRightDistance,
                sensors->right45Distance,
                sensors->rightDistance,
                activeStrategy->angle,
                activeStrategy->power
        );
        Serial.println(buffer);
        Serial.flush();
        fps = 0;
    }
#endif
}

#endif
