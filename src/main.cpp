/**
 * @author Valery Akhrymchuk
 */

#include <Arduino.h>
#include <MemoryFree.h>

//#define DEBUG true

#if defined(BLUETOOTH_ENABLE)
#include "RoboraceBluetooth.h"
#elif defined(DISPLAY_ENABLE)
#include "RoboraceDisplay.h"
#else
#include "Roborace.h"
#endif

Roborace *roborace;

void setup() {

#ifdef FREE_RUN_MODE
    ADC_setup();
#endif

#ifdef DEBUG
    Serial.begin(115200);
    Serial.print(F("free memory="));
    Serial.println(freeMemory());
#endif

#if defined(BLUETOOTH_ENABLE)
    roborace = new RoboraceBluetooth();
#elif defined(DISPLAY_ENABLE)
    roborace = new RoboraceDisplay();
#else
    roborace = new Roborace();
#endif

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
