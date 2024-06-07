#pragma once

#include <EEPROM.h>
#include "Param.h"

class ParamHolder {
public:
    static const int SIZE = 40;
private:

    Param *values[SIZE]{};
    int size = 0;
    JsonDocument doc = JsonDocument();

public:

    ParamHolder() {
        EEPROM.begin(256);
    }

    void add(Param *value) {
        if (size < SIZE) {
            values[size++] = value;
        } else {
            DEBUGRR("Unable to store param!");
        }
    }

    JsonDocument &createMessageWithParams() {
        doc.clear();
        doc["t"] = "p";

        for (int i = 0; i < size; ++i) {
            Param *param = values[i];
            if (!doc.containsKey(param->group)) {
                doc[param->group].to<JsonObject>();
            }
            doc[param->group][param->name] = param->value;
        }

        return doc;
    }

    bool apply(JsonDocument &doc) {
        for (int i = 0; i < size; ++i) {
            Param *param = values[i];
            const char *name = param->name.c_str();
            if (doc.containsKey(param->name)) {
                param->value = doc[name];
                DEBUGRRF("Param changed %s %d\n", name, param->value);
            }
        }

        if (doc.containsKey("action")) {
            String action = doc["action"];
            DEBUGRRF("Need to perform action %s\n", action.c_str());
            if (action.equals("save")) {
                DEBUGRR("Writing");
                writeAllEeprom();
            } else if (action.equals("read")) {
                DEBUGRR("Reading");
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
