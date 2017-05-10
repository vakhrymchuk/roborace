#ifndef ROBORACE_MESSAGE_H
#define ROBORACE_MESSAGE_H

#include <Arduino.h>

class Message {
public:

    // to robot

    uint8_t angle;
    uint16_t speed;
    bool enabled;

    // from robot

    char strategy;

    uint8_t forwardRightDistance;
    uint8_t leftDistance;
    uint8_t rightDistance;
    uint8_t forwardLeftDistance;

    uint16_t rotate;
};

#endif