/**
 * @author Valery Akhrymchuk
 */

#include <Arduino.h>
#include <Wire.h>

//#define DEBUG true

#include "Roborace.h"

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
