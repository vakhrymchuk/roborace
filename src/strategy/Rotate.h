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

    static const int ROTATE_SPEED = 70;

    virtual Strategy *init(Strategy *callback, unsigned int minMs, int param = 0) final {
        Strategy::init(callback, minMs);
        stage = 0;
        return this;
    }

    virtual Strategy *check(SensorsHolder *sensors) final {
        if (stage == 3)
            return callback->init(this, 0);
        return this;
    }

    virtual void calc(SensorsHolder *sensors) final {

        switch (stage) {
            case 0:
                if (/*sensors->leftDistance > 10 && */stopwatch->isLessThan(800)) {
                    angle = Mechanics::FULL_RIGHT;
                    power = ROTATE_SPEED;
                } else {
                    nextStage();
                }
                break;
            case 1:
                if (/*sensors->rightDistance < 50 && */stopwatch->isLessThan(1000)) {
                    angle = Mechanics::FULL_LEFT;
                    power = -ROTATE_SPEED;
                } else {
                    nextStage();
                }
                break;
            case 2:
                if (/*sensors->rightDistance < 50 &&*/ stopwatch->isLessThan(400)) {
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
