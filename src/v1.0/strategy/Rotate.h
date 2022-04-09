#pragma once

#include "Strategy.h"

class Rotate : public Strategy {
public:

    static const int ROTATE_SPEED = 66;

    virtual Strategy *init(Strategy *callback, unsigned int minMs, int param = 0) final {
        Strategy::init(callback, minMs);
        stage = 0;
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final {
        if (stage == 3)
            return callback->init(this, 3000);
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final {

        switch (stage) {
            case 0:
                if (/*sensors->leftDistance > 10 && */stopwatch->isLessThan(800)) {
                    angle = Mechanics::FULL_LEFT;
                    power = ROTATE_SPEED;
                } else {
                    nextStage();
                }
                break;
            case 1:
                if (/*sensors->rightDistance < 50 && */stopwatch->isLessThan(1500)) {
                    angle = Mechanics::FULL_RIGHT;
                    power = -ROTATE_SPEED;
                } else {
                    nextStage();
                }
                break;
            case 2:
                if (/*sensors->rightDistance < 50 &&*/ stopwatch->isLessThan(400)) {
                    angle = Mechanics::FULL_LEFT;
                    power = ROTATE_SPEED;
                } else {
                    nextStage();
                }
                break;
            default:
                angle = 0;
                power = 0;
        }
    }

    void nextStage() {
        stage++;
        stopwatch->start();
    }

    Strategy *forward;

private:
    byte stage = 0;
};
