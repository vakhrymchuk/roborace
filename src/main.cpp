#include <Arduino.h>

//#define DEBUG true
#define WAIT_5S true

#include "Roborace2.h"
// #include "hal/EngineConfig.h"

Roborace *roborace;

// EngineConfig * engine;

void setup() {

#ifdef DEBUG
    Serial.begin(115200);
    Serial.println("start");
#endif
#ifdef FREE_RUN_MODE
    ADC_setup();
#endif

    roborace = new Roborace();
    // engine = new EngineConfig();
}

void loop() {
    roborace->loop();
    // engine->loop();
}
