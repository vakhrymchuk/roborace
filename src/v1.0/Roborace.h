#pragma once

#include <Arduino.h>
#include "v1.0/mechanics/SensorsHolder.h"
#include "v1.0/mechanics/Mechanics.h"
#include "v1.0/strategy/Forward.h"
#include "v1.0/strategy/Turbo.h"
#include "v1.0/strategy/Backward.h"
#include "v1.0/strategy/Rotate.h"
#include "v1.0/strategy/RightWall.h"
#include "v1.0/strategy/LeftWall.h"
#include "v1.0/value/TimingValue.h"


class Roborace {
public:

    static const int RUN_INTERVAL_MS = 33;
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
//    Turbo *turbo = new Turbo;
    Backward *backward = new Backward;
    Rotate *rotate = new Rotate;
//    RightWall *rightWall = new RightWall;
//    LeftWall *leftWall = new LeftWall;
    Strategy *activeStrategy = forward;

//    boolean enabled = true;

    Interval *fpsInterval = new Interval(1, SECOND);
#ifdef DEBUG
    IntervalValue *debugInterval = new IntervalValue(new Param(200, "debug-interval", "main"));
#endif

private:

    void initStrategies();
};


void Roborace::initStrategies() {
    activeStrategy = forward->init(nullptr, 1000, 0);
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

        Serial.print("FPS = ");
        Serial.print(fpsLastValue);
        Serial.print(" loop time = ");
        Serial.print(loopTime);
        Serial.print(" l90 = ");
        Serial.print(sensors->leftDistance);
        Serial.println();

/*        Serial.printf(
                "loop fps = %u ms = %4lu   L90 =%3u  L60 =%3u  L30 =%3u  FC =%3u  R30 =%3u  R60 =%3u  R90 =%3u   ang =% 4d  pow =% 4d  v=%.1f\n",
                fpsLastValue,
                loopTime,
                sensors->l90d,
                sensors->l45d,
                sensors->l30d,
                sensors->f00d,
                sensors->r30d,
                sensors->r60d,
                sensors->r90d,
                activeStrategy->angle,
                activeStrategy->power,
                mechanics->battery.readFloatKalman()
        );*/
    }
#endif
}
