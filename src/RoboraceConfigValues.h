#ifndef ROBORACE_CONFIG_VALUES_H
#define ROBORACE_CONFIG_VALUES_H

#include "Roborace.h"
#include "value/ValueEditor.h"

class RoboraceConfigValues : public Roborace {
public:

    RoboraceConfigValues() {
        initValueEditor();
        readConfigFromEEPROM();
    }

    ValueEditor valueEditor = ValueEditor(&Serial);

private:

    void initValueEditor();

    void readConfigFromEEPROM();

};

void RoboraceConfigValues::initValueEditor() {

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
    valueEditor.add(forward->forwardSpeed->valueInt, F("FRWD SPEED"));
    valueEditor.add(forward->forwardAcceleration->valueInt, F("ACCELERATION"));
    valueEditor.add(forward->distStartTurn, F("DIST START TURN"));
    valueEditor.add(forward->distFullTurn, F("DIST FULL TURN"));
    valueEditor.add(forward->distWall, F("DIST WALL"));
    valueEditor.add(forward->distPersecution, F("DIST PERSECUTION"));

    valueEditor.add(forward->turboModeDist, F("TRB DIST ENABLE"));
    valueEditor.add(forward->turboTurn, F("TRB ANGLE TURN"));
    valueEditor.add(forward->turboMaxTurn, F("TRB ANGLE MAX"));

    valueEditor.add(backward->backwardSpeed, F("BACKWARD SPEED"));

//    valueEditor->add(forwardRunMinDelay, "FORWARD RUN MIN DELAY");
//    valueEditor->add(backwardRunMaxDelay, "BACK RUN MAX DELAY");
//    valueEditor->add(angleRunMaxDelay, "ANGLE RUN MAX DELAY");

}

void RoboraceConfigValues::readConfigFromEEPROM() {
    valueEditor.displayAll("Config code values:");
//    valueEditor.writeAllEEPROM();
//    valueEditor.readAllEEPROM();
//    valueEditor.displayAll("Config EEPROM values:");
}

#endif
