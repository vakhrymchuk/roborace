/**
 * @author Valery Akhrymchuk
 */

#include <Arduino.h>
#include <MemoryFree.h>

//#define DEBUG true

#ifndef JOYSTICK_ENABLE

#if defined(BLUETOOTH_ENABLE)
#include "RoboraceBluetooth.h"
#elif defined(DISPLAY_ENABLE)
#include "RoboraceDisplay.h"
#elif defined(CONFIG_VALUES_ENABLE)
#include "RoboraceConfigValues.h"
#else
#include "Roborace.h"
#endif

Roborace *roborace;

Roborace *createRoborace() {
#if defined(BLUETOOTH_ENABLE)
    return new RoboraceBluetooth();
#elif defined(DISPLAY_ENABLE)
    return new RoboraceDisplay();
#elif defined(CONFIG_VALUES_ENABLE)
    return new RoboraceConfigValues();
#else
    return new Roborace();
#endif

}

void setup() {

#ifdef FREE_RUN_MODE
    ADC_setup();
#endif

#ifdef DEBUG
    Serial.begin(115200);
    Serial.print(F("free memory="));
    Serial.println(freeMemory());
#endif

    roborace = createRoborace();

//    ButtonPullUp rightWallBtn(3);
//    if (rightWallBtn.read()) {
//        roborace->activeStrategy = &Strategy::rightWall;
//    }

#ifdef DEBUG
    Serial.print(F("free memory="));
    Serial.println(freeMemory());
#endif
}

void loop() {
    roborace->loop();
}

#else

#include "RoboraceJoystick.h"

RoboraceJoystick roboraceJoystick;

void setup() {

#ifdef DEBUG
    Serial.begin(115200);
    Serial.print(F("free memory="));
    Serial.println(freeMemory());
#endif

    roboraceJoystick.init();
}

void loop() {
    roboraceJoystick.process();
}

#endif
