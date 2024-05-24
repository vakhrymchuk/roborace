#pragma once

#include "hal/Mechanics.h"
#include "hal/Lidar.h"
#include "hal/MPU.h"
#include "model/LaserScan.h"
#include "model/PhysicData.h"
#include "logic/LidarSimpleSolution.h"

class Roborace
{
public:
    LaserScan scan;
    Solution solution;
    MPU mpu;

    Roborace()
    {
        Lidar::init();
        mpu.init();
    }

    void loop()
    {
        scan.data.clear();
        Stopwatch wait;
        Interval show(200);
        while (!Lidar::getData(scan.data))
        {
            mechanics.run(speed, turn);
            mpu.readData();
            if (wait.isMoreThan(150))
            {
                speed = 0;
                turn = 50 * sin(0.005 * millis());
                if (show.isReady())
                    DEBUGF("Scan is not received\n");
            }
        }

        if (scan.data.size() > 50)
        {
            PhysicalData data = {scan, mechanics.getEngine().getSpeed(), mpu.yaw, mpu.pitch, mpu.roll, mpu.absoluteAngle()};
            solution.logic(data, speed, turn);
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
