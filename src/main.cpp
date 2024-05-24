#include <Arduino.h>

#define DEBUGF(...) //Serial.printf(__VA_ARGS__)
#define WAIT_5S true

#include "Roborace2.h"
// #include "hal/EnginePidConfig.h"
// #include "hal/ServoConfig.h"

Roborace *roborace;
// EnginePidConfig *roborace;
// ServoConfig *roborace;

void setup()
{
#ifdef DEBUGF
    Serial.begin(115200);
#endif
    DEBUGF("start\n");
#ifdef FREE_RUN_MODE
    ADC_setup();
#endif

    roborace = new Roborace();
    // roborace = new EnginePidConfig();
    // roborace = new ServoConfig();
}

void loop()
{
    roborace->loop();
}
