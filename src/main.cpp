/**
 * @author Valery Akhrymchuk
 */

#include <Arduino.h>
#include <Wire.h>
#include "Roborace.h"

//#define DEBUG true

Roborace *roborace;

void setup() {

#ifdef VL53
    Wire.begin();
    Wire.setClock(400000);
#endif

#ifdef DEBUG
    Serial.begin(115200);
#endif

    roborace = new Roborace();

}

void loop() {
    roborace->loop();
}
