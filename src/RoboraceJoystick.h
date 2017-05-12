#ifndef ROBORACE_ROBORACEJOYSTICK_H
#define ROBORACE_ROBORACEJOYSTICK_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Joystick.h>
#include <Timing.h>
#include "remote/Transceiver.h"

class RoboraceJoystick {
public:

    void init() {
        lcd.init();
        lcd.backlight();
        lcd.setCursor(0, 0);
        lcd.println("Hello, Roborace!");
        lcd.setCursor(0, 1);
        transceiver.init("joy01", lcd);

        delay(2000);

        transceiver.message.rotate = 123;
        transceiver.message.logicVoltage = 7.84;
        transceiver.message.engineVoltage = 12.59;

        transceiver.message.forwardLeftDistance = 180;
        transceiver.message.leftDistance = 180;
        transceiver.message.rightDistance = 180;
        transceiver.message.forwardRightDistance = 180;
    }

    void process() {

        if (transceiver.receiveWithTimeout(5)) {
            if (serialInterval.isReady()) {
                Serial.println("Received");
                Serial.print("\tFL=");
                Serial.print(transceiver.message.forwardLeftDistance);
                Serial.print("\tL=");
                Serial.print(transceiver.message.leftDistance);
                Serial.print("\tR=");
                Serial.print(transceiver.message.rightDistance);
                Serial.print("\tFR=");
                Serial.print(transceiver.message.forwardRightDistance);
                Serial.print("\tRT=");
                Serial.print(transceiver.message.rotate);
                Serial.print("\tLV=");
                Serial.print(transceiver.message.logicVoltage);
                Serial.print("\tEV=");
                Serial.println(transceiver.message.engineVoltage);
            }
        }

        if (displayInterval->isReady()) {
            lcd.setCursor(0, 0);
            lcd.print("F");
            lcd.print(transceiver.message.forwardLeftDistance);
            lcd.print("  ");

            lcd.setCursor(4, 0);
            lcd.print("L");
            lcd.print(transceiver.message.leftDistance);
            lcd.print("  ");

            lcd.setCursor(8, 0);
            lcd.print("R");
            lcd.print(transceiver.message.rightDistance);
            lcd.print("  ");

            lcd.setCursor(12, 0);
            lcd.print("F");
            lcd.print(transceiver.message.forwardRightDistance);
            lcd.print("  ");

            lcd.setCursor(0, 1);
            lcd.print('V');
            lcd.print(transceiver.message.logicVoltage);
            lcd.print('/');
            lcd.print(transceiver.message.engineVoltage);
            lcd.print(" R");
            lcd.print(transceiver.message.rotate);
            lcd.print("    ");
        }
    }

private:

    Transceiver transceiver;

    LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

    Joystick joystick = JoystickCorrection(A2, A1, A0);

    Interval *displayInterval = new Interval(1000);
    Interval serialInterval = Interval(500);

};

#endif
