#pragma once

#include "hal/Mechanics.h"
#include "hal/Lidar.h"
#include "model/LaserScan.h"
#include "logic/LidarSimpleSolution.h"

class Roborace
{
public:
    LaserScan scan;
    Solution solution;

    Roborace()
    {
        Lidar::init();
    }

    void loop()
    {
        scan.data.clear();
        Stopwatch wait;
        while (!Lidar::getData(scan.data))
        {
            mechanics.run(speed, turn);
            if (wait.isMoreThan(150))
            {
                speed = 0;
                turn = 50 * sin(0.005 * millis());
                DEBUGF("Scan is not received\n");
            }
        }

        if (scan.data.size() > 50)
        {
            solution.logic(scan, speed, turn);
        }
        else
        {
            DEBUGF("Not enough Data, size = %d\n", scan.data.size());
            speed = 0;
            turn = 0;
        }

#ifdef WAIT_5S
        if (millis() < 3500)
            speed = 0;
#endif

        mechanics.run(speed, turn);
    }

private:
    Mechanics mechanics;

    int speed = 0;
    int turn = ServoWrapper::CENTER;
};
