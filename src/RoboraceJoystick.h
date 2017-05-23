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
        lcd.println("Roborace!");
        lcd.setCursor(0, 1);
        transceiver.init("joy01", lcd);

        delay(500);
        lcd.clear();

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
#ifdef DEBUG
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
#endif
        }

        if (displayInterval.isReady()) {
            sprintf(buffer, "%3d L%3dR%3d %3d",
                    transceiver.message.forwardLeftDistance,
                    transceiver.message.leftDistance,
                    transceiver.message.rightDistance,
                    transceiver.message.forwardRightDistance);
            lcd.setCursor(0, 0);
            lcd.print(buffer);

            sprintf(buffer, "A%3d P%3d R%3d",
                    transceiver.message.angle,
                    transceiver.message.power,
                    transceiver.message.rotate);
            lcd.setCursor(0, 1);
            lcd.print(buffer);
        }
    }

private:

    Transceiver transceiver;

    LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

    Joystick joystick = JoystickCorrection(A2, A1, A0);

    Interval displayInterval = Interval(100);
#ifdef DEBUG
    Interval serialInterval = Interval(500);
#endif
    char buffer[17];

};

#endif
