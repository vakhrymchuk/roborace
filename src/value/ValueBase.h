#ifndef ROBORACE_VALUE_H
#define ROBORACE_VALUE_H

#include <EEPROM.h>

class ValueBase {
public:

//    static const int STRING_LENGTH = 6;

    ValueBase(const __FlashStringHelper *name) : name(name) {}

    const __FlashStringHelper *name;

    virtual void inc(int size = 1) = 0;

    virtual void dec(int size = 1) = 0;

//    virtual const char *getStringValue() const = 0;

    virtual void printValue(Print *print) const = 0;

    virtual void parseValue(const char *stringValue) = 0;

    virtual void readValue(int &address) = 0;

    virtual void writeValue(int &address) = 0;

//protected:
//    char *buffer = new char[STRING_LENGTH];

};

class ValueInt : public ValueBase {
public:

    static const int INT_STEP = 1;

    ValueInt(int value, const __FlashStringHelper *name = NULL) : ValueBase(name), value(value) {}

    int value;

    void inc(int size) override {
        value += INT_STEP * size;
    }

    void dec(int size) override {
        value -= INT_STEP * size;
    }

    /*const char *getStringValue() const override {
        snprintf(buffer, STRING_LENGTH, "%5d", value);
        return buffer;
    }*/

    void printValue(Print *print) const override {
        print->println(value);
    }

    void parseValue(const char *stringValue) override {
        int newValue = atoi(stringValue);
        if (newValue != 0 || stringValue[0] == '0') {
            value = newValue;
        }
    }

    void readValue(int &address) override {
        byte data[sizeof(int)];
        for (unsigned int i = 0; i < sizeof(data); ++i) {
            data[i] = EEPROM.read(address++);
        }
        memcpy(&value, data, sizeof(value));
    };

    void writeValue(int &address) override {
        byte data[sizeof(int)];
        memcpy(data, &value, sizeof(value));
        for (unsigned int i = 0; i < sizeof(data); ++i) {
            EEPROM.update(address++, data[i]);
        }
    };
};

class ValueFloat : public ValueBase {
public:

    static constexpr float FLOAT_STEP = 0.05;

    ValueFloat(float value, const __FlashStringHelper *name = NULL) : ValueBase(name), value(value) {}

    float value;

    void inc(int size) override {
        value += FLOAT_STEP * size;
    }

    void dec(int size) override {
        value -= FLOAT_STEP * size;
    }

/*    const char *getStringValue() const override {
//        snprintf(buffer, STRING_LENGTH, "%5.2f", value);
        dtostrf(value, STRING_LENGTH, 2, buffer);
        return buffer;
    }*/

    void printValue(Print *print) const override {
        print->println(value);
    }

    void parseValue(const char *stringValue) override {
        float newValue = (float) atof(stringValue);
        if (fabs(newValue) > 0.001 || stringValue[0] == '0') {
            value = newValue;
        }
    }

    void readValue(int &address) override {
        byte data[sizeof(float)];
        for (unsigned int i = 0; i < sizeof(data); ++i) {
            data[i] = EEPROM.read(address++);
        }
        memcpy(&value, data, sizeof(value));
    };

    void writeValue(int &address) override {
        byte data[sizeof(float)];
        memcpy(data, &value, sizeof(value));
        for (unsigned int i = 0; i < sizeof(data); ++i) {
            EEPROM.write(address++, data[i]);
        }
    };
};

#endif
