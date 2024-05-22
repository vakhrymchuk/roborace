#include <Arduino.h>

// #define DEBUG true
#define WAIT_5S true

#include "Roborace2.h"
// #include "hal/EnginePidConfig.h"

Roborace *roborace;

// EnginePidConfig * engine;

void setup()
{

#ifdef DEBUG
    Serial.begin(115200);
    Serial.println("start");
#endif
#ifdef FREE_RUN_MODE
    ADC_setup();
#endif

    roborace = new Roborace();
    // engine = new EnginePidConfig();
}

void loop()
{
    roborace->loop();
    // engine->loop();
}
