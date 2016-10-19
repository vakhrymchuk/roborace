#ifndef ROBORACE_RIGHTWALL_H
#define ROBORACE_RIGHTWALL_H

#include "Strategy.h"

/**
 * ______ _       _     _   _    _       _ _
 * | ___ (_)     | |   | | | |  | |     | | |
 * | |_/ /_  __ _| |__ | |_| |  | | __ _| | |
 * |    /| |/ _` | '_ \| __| |/\| |/ _` | | |
 * | |\ \| | (_| | | | | |_\  /\  / (_| | | |
 * \_| \_|_|\__, |_| |_|\__|\/  \/ \__,_|_|_|
 *           __/ |
 *          |___/
 */
class RightWall : public Strategy {
public:

    virtual Strategy *check(SensorsHolder *sensors) final override {
        return this;
    }

    virtual void calc(SensorsHolder *sensors) {
        angle = (sensors->rightDistance - 50) * 80 / sensors->forwardRightDistance;
        power = 60;
    }

};

//RightWall rightWall;

#endif
