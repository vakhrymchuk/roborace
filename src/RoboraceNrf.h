#ifndef ROBORACE_ROBORACE_NRF_H
#define ROBORACE_ROBORACE_NRF_H

#include "Roborace.h"
#include "remote/Transceiver.h"
#include "PrintFake.h"

class RoboraceNrf : public Roborace {
public:

    RoboraceNrf() {
        transceiver.init("car01");
    }

    void loop() override {
        if (transceiver.receiveWithTimeout(5)) {
            mechanics->run(transceiver.message.angle, transceiver.message.power);
            runStopwatch.start();
        }

        if (mainLoopChange->isReady()) {
            sensors->readDistances();
        }

        if (nrfInterval->isReady()) {
//            transceiver.message.angle = activeStrategy->angle;
//            transceiver.message.power = activeStrategy->power;

            transceiver.message.forwardRightDistance = sensors->forwardRightDistance;
            transceiver.message.leftDistance = sensors->leftDistance;
            transceiver.message.rightDistance = sensors->rightDistance;
            transceiver.message.forwardLeftDistance = sensors->forwardLeftDistance;

            transceiver.message.rotate = mechanics->engine->engineEncoder->getPosition();
//        transceiver.message.logicVoltage = mechanics->logicVoltage.readFloatKalman();
//        transceiver.message.engineVoltage = mechanics->engineVoltage.readFloatKalman();

            transceiver.send("joy01");
        }

//        if (runStopwatch.isMoreThan(100)) {
//            mechanics->stop();
//        }

    }

private:
#ifdef DEBUG
    Print &print = Serial;
#else
    PrintFake print;
#endif

    Transceiver transceiver = Transceiver(print);

    Interval * nrfInterval = new Interval(200);

    Stopwatch runStopwatch;

};

#endif
