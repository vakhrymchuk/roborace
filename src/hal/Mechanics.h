#pragma once

#include "Engine.h"
#include "ServoWrapper.h"

class Mechanics
{
public:
    void run(int speed, int turn)
    {
        engine.run(speed);
        forward.turn(turn * 2);
        if (turn <= 50 && turn >= -50)
            backward.turn(0);
        else if (turn > 50)
            backward.turn(2 * (turn - 50));
        else // < -50
            backward.turn(2 * (turn + 50));
    }

    Engine &getEngine()
    {
        return engine;
    }

private:
    Engine engine;
    ServoWrapper forward = ServoWrapper(26, 93, -58, 60);
    ServoWrapper backward = ServoWrapper(27, 103, -25, 25);
};
