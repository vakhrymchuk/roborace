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
    long fps = 0;

private:
    Stopwatch wait;

public:
    Roborace()
    {
        Lidar::init();
        mpu.init();
    }

    virtual void loop()
    {
        mpu.readData();
        if (Lidar::getData(scan.data))
        {
            fps = 1000 / wait.time();
            wait.start();
            if (scan.data.size() > 50)
            {
                PhysicalData data = createPhysicalData();
                solution.logic(data, speed, turn);
            }
            else
            {
                DEBUGRRF("Not enough Data, size = %d\n", scan.data.size());
                speed = 0;
                turn = 0;
            }
        }
        else if (wait.isMoreThan(150))
        {
            speed = 0;
            turn = 50 * sin(0.005 * millis());
            DEBUGRR("Scan is not received");
        }

#ifdef WAIT_5S
        if (millis() < 3500)
            speed = 0;
#endif
        mechanics.run(speed, turn);
    }

    const PhysicalData createPhysicalData()
    {
        return {scan, mechanics.getEngine().getSpeed(), mpu.yaw, mpu.pitch, mpu.roll, mpu.absoluteAngle()};
    }

protected:
    Mechanics mechanics;

    int speed = 0;
    int turn = ServoWrapper::CENTER;
};
