/**
 * @author Valery Akhrymchuk
 */

#include <Arduino.h>

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

    ADC_setup();

#ifdef DEBUG
    Serial.begin(115200);
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

}

void loop() {
    roborace->loop();
}
