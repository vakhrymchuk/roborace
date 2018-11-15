#ifndef ROBORACE_ROTATE_H
#define ROBORACE_ROTATE_H

#include "Strategy.h"

/**
 * ______ _____ _____ ___ _____ _____
 * | ___ \  _  |_   _/ _ \_   _|  ___|
 * | |_/ / | | | | |/ /_\ \| | | |__
 * |    /| | | | | ||  _  || | |  __|
 * | |\ \\ \_/ / | || | | || | | |___
 * \_| \_|\___/  \_/\_| |_/\_/ \____/
 */
class Rotate : public Strategy {
public:

    static const int ROTATE_SPEED = 90;

    virtual Strategy *init(Strategy *callback, unsigned int minMs) final override {
        Strategy::init(callback, minMs);
        stage = 0;
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final override {
        if (stage == 3)
            return callback->init(this, 0);
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final override {

        switch (stage) {
            case 0:
                if (/*sensors->leftDistance > 10 && */stopwatch->isLessThan(1000)) {
                    angle = Mechanics::FULL_RIGHT;
                    power = ROTATE_SPEED;
                } else {
                    nextStage();
                }
                break;
            case 1:
                if (/*sensors->rightDistance < 50 && */stopwatch->isLessThan(1200)) {
                    angle = Mechanics::FULL_LEFT;
                    power = -ROTATE_SPEED;
                } else {
                    nextStage();
                }
                break;
            case 2:
                if (/*sensors->rightDistance < 50 &&*/ stopwatch->isLessThan(600)) {
                    angle = Mechanics::FULL_RIGHT;
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

#endif
