#include <Arduino.h>

#define DEBUGRRF(...) //Serial.printf(__VA_ARGS__)
#define DEBUGRR(...) //Serial.println(__VA_ARGS__)
#define WAIT_5S true

#include "RoboraceWebserver.h"
// #include "hal/testconfig/EnginePidConfig.h"
// #include "hal/testconfig/ServoConfig.h"

Roborace *roborace;
// EnginePidConfig *roborace;
// ServoConfig *roborace;

void setup()
{
#ifdef DEBUGRR
    Serial.begin(115200);
    DEBUGRR("start");
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
