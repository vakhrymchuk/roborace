#ifndef ROBORACE_ROBORACEJOYSTICK_H
#define ROBORACE_ROBORACEJOYSTICK_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Joystick.h>
#include "Roborace.h"
#include "remote/Transceiver.h"

class RoboraceJoystick : public Roborace {
public:

    RoboraceJoystick() {
//        lcd.init();
//        lcd.backlight();
//        lcd.setCursor(0, 0);
//        lcd.print("Roborace!");

        transceiver.init("joy01");

//        delay(1000);
//        lcd.clear();
    }

    void loop() override {

        if (transceiver.receiveWithTimeout(5)) {
            Serial.print('.');
            sensors->setDistances(message);
            activeStrategy = activeStrategy->check(sensors);
            activeStrategy->calc(sensors);

            message.angle = activeStrategy->angle;
            message.power = activeStrategy->power;
            transceiver.send("car01");

#ifdef DEBUG
            if (serialInterval->isReady()) {
                Serial.print("\tA=");
                Serial.print(message.angle);
                Serial.print("\tP=");
                Serial.print(message.power);
                Serial.print("\tFL=");
                Serial.print(message.forwardLeftDistance);
                Serial.print("\tL=");
                Serial.print(message.leftDistance);
                Serial.print("\tR=");
                Serial.print(message.rightDistance);
                Serial.print("\tFR=");
                Serial.print(message.forwardRightDistance);
                Serial.print("\tRT=");
                Serial.print(message.rotate);
//                Serial.print("\tLV=");
//                Serial.print(message.logicVoltage);
//                Serial.print("\tEV=");
//                Serial.print(message.engineVoltage);
                Serial.println();
                Serial.print(F("free memory="));
                Serial.println(freeMemory());
            }
#endif
        }

//        if (displayInterval.isReady()) {
//            sprintf(buffer, "%3d L%3dR%3d %3d",
//                    message.forwardLeftDistance,
//                    message.leftDistance,
//                    message.rightDistance,
//                    message.forwardRightDistance);
//            lcd.setCursor(0, 0);
//            lcd.print(buffer);
//
//            sprintf(buffer, "A%3d P%3d M%3d", message.angle, message.power, message.rotate);
//            lcd.setCursor(0, 1);
//            lcd.print(buffer);
//        }
    }

private:

//    LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

    Transceiver transceiver = Transceiver(Serial);

    Message &message = transceiver.message;

    Joystick joystick = JoystickCorrection(A2, A1, A0);

    Interval *displayInterval = new Interval(800);
#ifdef DEBUG
    Interval *serialInterval = new Interval(500);
#endif
    char buffer[17];

};

#endif
