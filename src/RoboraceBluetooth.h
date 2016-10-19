#ifndef ROBORACE_BLUETOOTH_H
#define ROBORACE_BLUETOOTH_H

#include <HardwareSerial.h>
#include "Roborace.h"
#include "value/ValueEditor.h"

class RoboraceBluetooth : public Roborace {
public:

    RoboraceBluetooth() {
        initBluetooth();
        initValueEditor();
        readConfigFromEEPROM();
    }

    virtual void loop() override final {
        Roborace::loop();
        processConfig();
        sendPackage();
        sendAdaptedValues();
    }

private:

    ValueEditor *valueEditor;

    HardwareSerial *bluetooth = &Serial;

    IntervalValue *sendPackageInterval = new IntervalValue(new ValueInt(1000));
    ValueInt *sendPackageValue = new ValueInt(0);

    void initBluetooth() const;

    void initValueEditor();

    void readConfigFromEEPROM() const;

    void processConfig();

    void sendState() const;

    void sendPackage();

    void sendAdaptedValues();

    void printValue(const char *string, int value) const;

    void writeInteger(const int integer) const;

    void checkAdaptation(Adaptation *adaptation, const char *string) const;
};

void RoboraceBluetooth::initBluetooth() const {
    bluetooth->begin(115200);
    bluetooth->println(F("bluetooth init"));
//    bluetooth->flush();
}

void RoboraceBluetooth::initValueEditor() {
    valueEditor = new ValueEditor(bluetooth);

    // mechanics
//    valueEditor->add(mechanics->turnMaxAngle, "TURN MAX ANGLE");
//    valueEditor->add(mechanics->turnCentralPosition, "TURN CENTRAL POSITION");

//    valueEditor->add(mechanics->engine->engineHelper->maxCorrectionRun, "ENG MAX COR RUN");
//    valueEditor->add(mechanics->engine->engineHelper->maxCorrectionBrake, "ENG MAX COR BRAKE");
//    valueEditor->add(mechanics->engine->engineHelper->correctionFactor, "ENG COR FACTOR");

//    valueEditor->add(&kalmanFactorValue, "KALMAN FACTOR");

//    valueEditor->add(sensors->samePlaceTime, "SAME PLACE TIME");
//    valueEditor->add((ValueBase *) sameValueFactor, "SAME VALUE FACTOR");

    // strategies
    valueEditor->add(forward->forwardSpeed->valueInt, "FRWD SPEED");
    valueEditor->add(forward->forwardAcceleration->valueInt, "FRWD SPEED ACCELERATION");
    valueEditor->add(forward->distStartTurn, "FRWD DIST START TURN");
    valueEditor->add(forward->distFullTurn, "FRWD DIST FULL TURN");
    valueEditor->add(forward->mediumModeMaxTurn, "FRWD MEDIUM MODE ANGLE MAX TURN");
    valueEditor->add(forward->turboModeDist, "FRWD TURBO DIST ENABLE");
    valueEditor->add(forward->turboTurn, "FRWD TURBO ANGLE TURN");
    valueEditor->add(forward->turboMaxTurn, "FRWD TURBO ANGLE MAX");
    valueEditor->add(forward->distWall, "FRWD DIST WALL");

    valueEditor->add(backward->backwardSpeed, "BACKWARD SPEED");

//    valueEditor->add(forwardRunMinDelay, "FORWARD RUN MIN DELAY");
//    valueEditor->add(backwardRunMaxDelay, "BACK RUN MAX DELAY");
//    valueEditor->add(angleRunMaxDelay, "ANGLE RUN MAX DELAY");

    valueEditor->add(sendPackageValue, "SEND PACKAGE BOOL");
    valueEditor->add(sendPackageInterval->getValueInt(), "SEND PACKAGE INTERVAL");

}

void RoboraceBluetooth::readConfigFromEEPROM() const {
    valueEditor->displayAll("Config code values:");
//    valueEditor->writeAllEEPROM();
//    valueEditor->readAllEEPROM();
//    valueEditor->displayAll("Config EEPROM values:");
}

void RoboraceBluetooth::processConfig() {
    if (bluetooth->available()) {
        const String &command = bluetooth->readString();
        if (command == "n") {
            valueEditor->displayElem(valueEditor->next());
        } else if (command == "p") {
            valueEditor->displayElem(valueEditor->prev());
        } else if (command == "c") {
            valueEditor->displayElem(valueEditor->current());
        } else if (command == "a") {
            valueEditor->displayAll("Config params:");
        } else if (command == "s") {
            sendState();
        } else if (command == "read") {
            valueEditor->readAllEEPROM();
            valueEditor->displayAll("Read");
        } else if (command == "write") {
            valueEditor->writeAllEEPROM();
            valueEditor->displayAll("Write");
        } else if (command == "stop") {
            enabled = false;
        } else if (command == "start") {
            enabled = true;
        } else {
            // edit value
            valueEditor->current()->parseValue(command.c_str());
            valueEditor->displayCurrent();
//            valueEditor->displayElem(valueEditor->next());
        }
    }
}

void RoboraceBluetooth::sendState() const {
    printValue("frwdLeftDst", sensors->forwardLeftDistance);
    printValue("frwdRightDst", sensors->forwardRightDistance);
    printValue("leftDst", sensors->leftDistance);
    printValue("rightDst", sensors->rightDistance);
    printValue("angle", activeStrategy->angle);
    printValue("power", activeStrategy->power);
    printValue("pos", mechanics->engine->engineEncoder->lastPos);
//    bluetooth->flush();
}

void RoboraceBluetooth::printValue(const char *string, int value) const {
    bluetooth->print(string);
    bluetooth->print("=");
    bluetooth->println(value);
}

void RoboraceBluetooth::sendPackage() {
    if (sendPackageValue->value && sendPackageInterval->isReady()) {
//        bluetooth->print("pos=");
//        bluetooth->println(mechanics->engine->engineEncoder->getPosition());

        bluetooth->write(0xDE);
        bluetooth->write(0xAD);
        bluetooth->write(sensors->forwardLeftDistance);  // 1
        bluetooth->write(sensors->forwardRightDistance); // 2

        bluetooth->write(sensors->leftDistance);         // 3
        bluetooth->write(sensors->rightDistance);        // 4

        writeInteger(activeStrategy->power);                     // 5 6
        writeInteger(mechanics->engine->engineEncoder->lastPos); // 7 8

        bluetooth->write(activeStrategy->angle & 0xFF);          // 9

        bluetooth->write(0xBE);
        bluetooth->write(0xAF);


/*        int size = 16;
        int8_t message[size];
        int i = 0;
        message[i++] = 0xDE;
        message[i++] = 0xAD;

        message[i++] = sensors->forwardLeftDistance;
        message[i++] = sensors->forwardRightDistance;
        message[i++] = sensors->leftDistance;
        message[i++] = sensors->rightDistance;

        message[i++] = activeStrategy->power >> 8 & 0xFF;
        message[i++] = activeStrategy->power & 0xFF;
        message[i++] = activeStrategy->angle & 0xFF;
        message[i++] = 0;

        message[i++] = 0;
        message[i++] = 0;
        message[i++] = 0;
        message[i++] = 0;

        message[i++] = 0xBE;
        message[i] = 0xAF;

        if (i >= size) {
            bluetooth->print("alarm");
            delay(5000);
        }

        for (auto &&item : message) {
            bluetooth->write((byte)item);
        }*/
//        bluetooth->flush();
    }
}

void RoboraceBluetooth::writeInteger(const int integer) const {
    bluetooth->write(integer >> 8 & 0xFF);
    bluetooth->write(integer & 0xFF);
}

void RoboraceBluetooth::sendAdaptedValues() {
    checkAdaptation(forward->forwardSpeed, "ADAPTED FORWARD SPEED");
    checkAdaptation(forward->forwardAcceleration, "ADAPTED FORWARD ACCELERATION");
}

void RoboraceBluetooth::checkAdaptation(Adaptation *adaptation, const char *string) const {
    if (adaptation->adapted) {
        printValue(string, adaptation->valueInt->value);
        adaptation->adapted = false;
    }
}

#endif
