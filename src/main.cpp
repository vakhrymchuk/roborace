/**
 * @author Valery Akhrymchuk
 */

#include <Arduino.h>
#include <Wire.h>
#include <avr/wdt.h>

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
    delay(3000);

    wdt_enable(WDTO_1S);

}

void loop() {
    wdt_reset();
    roborace->loop();
}
