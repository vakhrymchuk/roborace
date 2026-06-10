#pragma once

#include <Arduino.h>

#define LINE_SENSOR_PIN 35

enum class LineSensorState {
    IDLE,
    BLACK_LINE
};

class LineSensor {
public:
    static constexpr unsigned long BLACK_LINE_MIN_MS = 33;  // ~5 см при 150 см/с
    static constexpr unsigned long STOP_DURATION_MS = 1000;

    LineSensor() {
        instance = this;
    }

    void init() {
        pinMode(LINE_SENSOR_PIN, INPUT);
        state = LineSensorState::IDLE;
        onBlack = !digitalRead(LINE_SENSOR_PIN);
        if (onBlack) blackStartTime = millis();
        attachInterrupt(digitalPinToInterrupt(LINE_SENSOR_PIN), onLineChange, CHANGE);
    }

    LineSensorState update() {
        unsigned long now = millis();
        
        if (state == LineSensorState::BLACK_LINE) {
            if (now - blackLineDetectedTime > STOP_DURATION_MS) {
                state = LineSensorState::IDLE;
            }
        } else if (onBlack && (now - blackStartTime >= BLACK_LINE_MIN_MS)) {
            state = LineSensorState::BLACK_LINE;
            blackLineDetectedTime = now;
        }
        
        return state;
    }
    
    LineSensorState getState() const { return state; }
    bool shouldStop() const { return state == LineSensorState::BLACK_LINE; }

private:
    static LineSensor* instance;
    
    volatile LineSensorState state = LineSensorState::IDLE;
    volatile bool onBlack = false;
    volatile unsigned long blackStartTime = 0;
    unsigned long blackLineDetectedTime = 0;
    
    static void onLineChange();
    void handleInterrupt();
};

LineSensor* LineSensor::instance = nullptr;

void IRAM_ATTR LineSensor::onLineChange() {
    if (instance) {
        instance->handleInterrupt();
    }
}

void IRAM_ATTR LineSensor::handleInterrupt() {
    if (digitalRead(LINE_SENSOR_PIN)) {
        // белый
        onBlack = false;
    } else {
        // чёрный
        blackStartTime = millis();
        onBlack = true;
    }
}
