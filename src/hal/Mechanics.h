#pragma once

#include "Engine.h"
#include "ServoWrapper.h"

class Mechanics {
public:

    void run(float speed, int turn) {
        engine.run(speed);
        forward.turn(turn * 3);
        backward.turn(turn);
    }

    const Engine &getEngine() const {
        return engine;
    }

private:

    Engine engine;
    ServoWrapper forward = ServoWrapper(10, 95, -56, 58);
    ServoWrapper backward = ServoWrapper(11, 89, -35, 30);

};