#pragma once

#include "Engine.h"
#include "ServoWrapper.h"

class Mechanics {
public:

    void run(int speed, int turn) {
        engine.run(speed);
        forward.turn(turn * 2);
        backward.turn(turn);
    }

    Engine &getEngine() {
        return engine;
    }

private:

    Engine engine;
    ServoWrapper forward = ServoWrapper(10, 90, -58, 60);
    ServoWrapper backward = ServoWrapper(11, 90, -30, 30);

};