#pragma once

#include <Arduino.h>
#include "Stopwatch.h"

#define LINE_SENSOR_PIN 35  // GPIO35 - ADC1, поддерживает прерывания

// Скорость робота ~150 см/с = 1.5 см/мс
// Черная линия 5 см -> ~33 мс
// Белый промежуток 20 см -> ~133 мс  
// Стоп-линия 25 см -> ~167 мс

enum class TrackEvent {
    NONE,           // Ничего не обнаружено
    OBSTACLE,       // 2 линии + белый > 15см = камни, ехать 1м без чтения
    STOP_AHEAD      // 1 линия = через 1м стоп-линия, снизить скорость
};

enum class LineSensorState {
    IDLE,               // Ожидание
    COUNTING_LINES,     // Подсчет линий
    WAITING_DECISION,   // Ожидание решения (белый после линий)
    OBSTACLE_IGNORE,    // Игнорирование после камней (1м)
    APPROACH_STOP,      // Подъезд к стоп-линии (сниженная скорость)
    STOPPED             // Остановка на стоп-линии
};

class LineSensor {
public:
    static constexpr int ROBOT_SPEED_CM_S = 150;
    static constexpr int LINE_WIDTH_CM = 5;
    static constexpr int WHITE_GAP_MIN_CM = 15;
    static constexpr int STOP_LINE_WIDTH_CM = 25;
    static constexpr int DISTANCE_TO_OBSTACLE_CM = 100;
    static constexpr int DISTANCE_TO_STOP_CM = 100;
    
    // Временные константы (мс) при скорости 150 см/с
    static constexpr unsigned long LINE_MIN_MS = 7;
    static constexpr unsigned long LINE_MAX_MS = 25;
    static constexpr unsigned long WHITE_GAP_MIN_MS = 100;
    static constexpr unsigned long DECISION_TIMEOUT_MS = 200;
    static constexpr unsigned long STOP_LINE_MIN_MS = 100;
    static constexpr unsigned long IGNORE_DISTANCE_MS = 1500;
    static constexpr unsigned long STOP_DURATION_MS = 1000;

    LineSensor() {
        instance = this;
    }

    void init() {
        pinMode(LINE_SENSOR_PIN, INPUT);
        state = LineSensorState::IDLE;
        lineCount = 0;
        lastLineValue = digitalRead(LINE_SENSOR_PIN);
        attachInterrupt(digitalPinToInterrupt(LINE_SENSOR_PIN), onLineChange, CHANGE);
    }

    TrackEvent update() {
        TrackEvent event = TrackEvent::NONE;
        unsigned long now = millis();
        
        switch (state) {
            case LineSensorState::IDLE:
            case LineSensorState::COUNTING_LINES:
                break;
                
            case LineSensorState::WAITING_DECISION:
                if (now - lastEdgeTime > DECISION_TIMEOUT_MS) {
                    if (lineCount >= 2) {
                        event = TrackEvent::OBSTACLE;
                        state = LineSensorState::OBSTACLE_IGNORE;
                        ignoreStartTime = now;
                        lineCount = 0;
                    } else if (lineCount == 1) {
                        event = TrackEvent::STOP_AHEAD;
                        state = LineSensorState::APPROACH_STOP;
                        ignoreStartTime = now;
                        lineCount = 0;
                    } else {
                        state = LineSensorState::IDLE;
                        lineCount = 0;
                    }
                }
                break;
                
            case LineSensorState::OBSTACLE_IGNORE:
                if (now - ignoreStartTime > IGNORE_DISTANCE_MS) {
                    state = LineSensorState::IDLE;
                }
                break;
                
            case LineSensorState::APPROACH_STOP:
                if (now - ignoreStartTime > 1000) {
                    state = LineSensorState::IDLE;
                }
                break;
                
            case LineSensorState::STOPPED:
                if (now - stopStartTime > STOP_DURATION_MS) {
                    state = LineSensorState::IDLE;
                }
                break;
        }
        
        return event;
    }
    
    LineSensorState getState() const { return state; }
    int getLineCount() const { return lineCount; }
    bool isOnBlackLine() const { return !lastLineValue; }
    bool shouldStop() const { return state == LineSensorState::STOPPED; }
    bool shouldSlowDown() const { return state == LineSensorState::APPROACH_STOP; }
    bool shouldIgnoreSensor() const { return state == LineSensorState::OBSTACLE_IGNORE; }

private:
    static LineSensor* instance;
    
    volatile LineSensorState state = LineSensorState::IDLE;
    volatile int lineCount = 0;
    volatile bool lastLineValue = true;
    volatile unsigned long lastEdgeTime = 0;
    volatile unsigned long blackStartTime = 0;
    
    unsigned long ignoreStartTime = 0;
    unsigned long stopStartTime = 0;
    
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
    bool currentValue = digitalRead(LINE_SENSOR_PIN);
    unsigned long now = millis();
    // Serial.println("Interrupt: " + String(currentValue) + " at " + String(now));
    
    if (state == LineSensorState::OBSTACLE_IGNORE || 
        state == LineSensorState::STOPPED) {
        lastLineValue = currentValue;
        return;
    }
    
    if (currentValue != lastLineValue) {
        unsigned long duration = now - lastEdgeTime;
        
        if (!currentValue) {
            // белый -> черный
            blackStartTime = now;
            if (state == LineSensorState::COUNTING_LINES && duration > WHITE_GAP_MIN_MS) {
                state = LineSensorState::WAITING_DECISION;
            }
        } else {
            // черный -> белый
            unsigned long blackDuration = now - blackStartTime;
            Serial.println("Black duration: " + String(blackDuration) + " ms");
            
            if (state == LineSensorState::APPROACH_STOP) {
                if (blackDuration >= STOP_LINE_MIN_MS) {
                    state = LineSensorState::STOPPED;
                    stopStartTime = now;
                }
            } else if (blackDuration >= LINE_MIN_MS && blackDuration <= LINE_MAX_MS) {
                lineCount++;
                Serial.println("Line count: " + String(lineCount));
                if (state == LineSensorState::IDLE) {
                    state = LineSensorState::COUNTING_LINES;
                }
            } else if (blackDuration > LINE_MAX_MS && state == LineSensorState::COUNTING_LINES) {
                state = LineSensorState::WAITING_DECISION;
            }
        }
        
        lastEdgeTime = now;
        lastLineValue = currentValue;
    }
}
