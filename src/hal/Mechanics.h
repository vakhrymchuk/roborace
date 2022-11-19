#pragma once

#include "Engine.h"
#include "ServoWrapper.h"

class Mechanics {
public:

    void run(float speed, int turn) {
        engine.run(speed);
        forward.turn(turn * 4);
        backward.turn(turn);
    }

    Engine &getEngine() {
        return engine;
    }

private:

    Engine engine;
    ServoWrapper forward = ServoWrapper(10, 97, -56, 58);
    ServoWrapper backward = ServoWrapper(11, 89, -35, 30);

};