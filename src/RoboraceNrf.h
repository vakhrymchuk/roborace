#ifndef ROBORACE_ROBORACE_NRF_H
#define ROBORACE_ROBORACE_NRF_H

#include "Roborace.h"
#include "remote/Transceiver.h"
#include "PrintFake.h"

class RoboraceNrf : public Roborace {
public:

    RoboraceNrf() {
#ifdef DEBUG
        Print &print = Serial;
#else
        PrintFake print;
#endif
        transceiver.init("car01", print);
    }

    void loop() {
        Roborace::loop();

        if (transceiver.receiveWithTimeout(5)) {
//            mechanics->run(transceiver.message.angle, transceiver.message.power);
        }

        if (nrfInterval.isReady()) {
            transceiver.message.angle = activeStrategy->angle;
            transceiver.message.power = activeStrategy->power;

            transceiver.message.forwardRightDistance = sensors->forwardRightDistance;
            transceiver.message.leftDistance = sensors->leftDistance;
            transceiver.message.rightDistance = sensors->rightDistance;
            transceiver.message.forwardLeftDistance = sensors->forwardLeftDistance;

            transceiver.message.rotate = mechanics->engine->engineEncoder->getPosition();
//        transceiver.message.logicVoltage = mechanics->logicVoltage.readFloatKalman();
//        transceiver.message.engineVoltage = mechanics->engineVoltage.readFloatKalman();

            transceiver.send("joy01");
        }

    }

private:

    Transceiver transceiver;

    Interval nrfInterval = Interval(200);

};

#endif
