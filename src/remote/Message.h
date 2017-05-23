#ifndef ROBORACE_MESSAGE_H
#define ROBORACE_MESSAGE_H

#include <Arduino.h>

class Message {
public:

    // to robot

    int8_t angle = 0;
    int16_t power = 0;
    bool enabled = true;

    // from robot

    char strategy = '0';

    uint8_t forwardRightDistance = 0;
    uint8_t leftDistance = 0;
    uint8_t rightDistance = 0;
    uint8_t forwardLeftDistance = 0;

    uint16_t rotate = 0;

    float logicVoltage = 0;
    float engineVoltage = 0;
};

#endif