#include <Arduino.h>

//#define DEBUG true
//#define WAIT_5S true

#include "Roborace2.h"

Roborace *roborace;

void setup() {

#ifdef DEBUG
    Serial.begin(115200);
    Serial.println("start");
#endif

    roborace = new Roborace();
}

void loop() {
    roborace->loop();
}
