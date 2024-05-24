#include <Arduino.h>

// #define DEBUG true
#define WAIT_5S true

#include "Roborace2.h"
// #include "hal/EnginePidConfig.h"
// #include "hal/ServoConfig.h"

Roborace *roborace;
// EnginePidConfig *roborace;
// ServoConfig *roborace;

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
    // roborace = new EnginePidConfig();
    // roborace = new ServoConfig();
}

void loop()
{
    roborace->loop();
}
