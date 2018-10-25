#ifndef ROBORACE_ENGINE_ENCODER_H
#define ROBORACE_ENGINE_ENCODER_H

#include <Interval.h>

#define ENCODER_PIN 2

volatile unsigned int pos = 0;

void doEncoder() {
    pos++;
}

class EngineEncoder {
public:
    EngineEncoder() {
        attachInterrupt((byte) digitalPinToInterrupt(ENCODER_PIN), doEncoder, RISING);
    }

    unsigned int getPosition() {
        if (interval->isReady()) {
            lastPos = pos;
            pos = 0;
        }
        return lastPos;
    }

    Interval *interval = new Interval(100);

    unsigned int lastPos;
};

#endif