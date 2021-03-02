#pragma once

#include <EEPROM.h>
#include "Param.h"

class ParamHolder {
public:
    static const int SIZE = 40;
private:

    Param *values[SIZE]{};
    int size = 0;
    DynamicJsonDocument doc = DynamicJsonDocument(1024);

public:

    ParamHolder() {
        EEPROM.begin(256);
    }

    void add(Param *value) {
        if (size < SIZE) {
            values[size++] = value;
        } else {
            Serial.println("Unable to store param!");
        }
    }

    DynamicJsonDocument &createMessageWithParams() {
        doc.clear();
        doc["t"] = "p";

        for (int i = 0; i < size; ++i) {
            Param *param = values[i];
            if (!doc.containsKey(param->group)) {
                doc.createNestedObject(param->group);
            }
            doc[param->group][param->name] = param->value;
        }

        return doc;
    }

    bool apply(DynamicJsonDocument &doc) {
        for (int i = 0; i < size; ++i) {
            Param *param = values[i];
            const char *name = param->name.c_str();
            if (doc.containsKey(param->name)) {
                param->value = doc[name];
                Serial.printf("Param changed %s %d\n", name, param->value);
            }
        }

        if (doc.containsKey("action")) {
            String action = doc["action"];
            Serial.printf("Need to perform action %s\n", action.c_str());
            if (action.equals("save")) {
                Serial.println("Writing");
                writeAllEeprom();
            } else if (action.equals("read")) {
                Serial.println("Reading");
                readAllEeprom();
                return true;
            }
        }
        return false;
    }


    void writeAllEeprom() {
        int address = 0;
        for (int i = 0; i < size; ++i) {
            if (EEPROM.readInt(address) != values[i]->value) {
                EEPROM.writeInt(address, values[i]->value);
            }
            address += sizeof(int);
        }
        EEPROM.commit();
    }

    void readAllEeprom() {
        int address = 0;
        for (int i = 0; i < size; ++i) {
            values[i]->value = EEPROM.readInt(address);
            address += sizeof(int);
        }
    }

};
