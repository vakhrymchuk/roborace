#ifndef ROBORACE_VALUE_EDITOR_H
#define ROBORACE_VALUE_EDITOR_H

#include "ValueBase.h"

class ValueEditor {
public:

    ValueEditor(Print *print) : print(print) { };

    static const int SIZE = 32;

    void add(ValueBase *value) {
        if (size < SIZE) {
            values[size++] = value;
        }
    }

    void add(ValueBase *value, const char *name) {
        add(value);
        if (name) {
            value->name = name;
        }
    }

    ValueBase *current() {
        return values[currentIndex];
    }

    ValueBase *next() {
        currentIndex = (currentIndex + 1) % size;
        return current();
    }

    ValueBase *prev() {
        currentIndex = (currentIndex == 0 ? size : currentIndex) - 1;
        return current();
    }

    ValueBase *values[SIZE];

    int size = 0;

    void readAllEEPROM();

    void writeAllEEPROM();

    void displayCurrent() {
        displayElem(current());
    }

    void displayElem(const ValueBase *elem) const;

    void displayAll(const char *string) const;

    void printLine() const;

private:

    Print *print;

    int currentIndex = 0;

};

void ValueEditor::readAllEEPROM() {
    int address = 0;
    for (int i = 0; i < size; ++i) {
        values[i]->readValue(address);
    }
}

void ValueEditor::writeAllEEPROM() {
    int address = 0;
    for (int i = 0; i < size; ++i) {
        values[i]->writeValue(address);
    }
}

void ValueEditor::displayElem(const ValueBase *elem) const {
    print->print(elem->name);
    print->print("=");
    elem->printValue(print);
}

void ValueEditor::displayAll(const char *string) const {
    printLine();
    print->println(string);
    for (int i = 0; i < size; ++i) {
        displayElem(values[i]);
    }
//    bluetooth->flush();
}

void ValueEditor::printLine() const {
    print->println(F("===================="));
}

#endif
