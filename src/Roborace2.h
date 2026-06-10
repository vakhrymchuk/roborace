#pragma once

#include "hal/Mechanics.h"
#include "hal/Lidar.h"
#include "hal/MPU.h"
#include "hal/LineSensor.h"
#include "model/LaserScan.h"
#include "model/PhysicData.h"
#include "logic/LidarSimpleSolution.h"
#include "PIDController.h"

class Roborace
{
public:
    LaserScan scan;
    Solution solution;
    MPU mpu;
    LineSensor lineSensor;
    long fps = 0;

private:
    Stopwatch wait;
    PIDController pid;

public:
    Roborace()
    {
        Lidar::init();
        mpu.init();
        lineSensor.init();
        pid.tune(1.6, 0, 3.2);
        pid.limit(-80, 80);
    }

    virtual void loop()
    {
        mpu.readData();
        
        lineSensor.update();
        
        if (lineSensor.shouldStop()) {
            speed = 0;
            mechanics.run(speed, turn);
            return;
        }
        
        if (Lidar::getData(scan.data))
        {
            fps = 1000 / wait.time();
            wait.start();
            if (scan.data.size() > 50)
            {
                PhysicalData data = createPhysicalData();
                solution.logic(data, speed, turn);
                desiredAngle = mpu.absoluteAngle() + turn;
                if(mpu.pitch > 8)
                  speed += mpu.pitch * 1;
                DEBUGF("speed = %d \t turn = %d\n", speed, turn);
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
            DEBUGRR("Scan is not received\n");
        }

#ifdef WAIT_5S
        if (millis() < 4500)
            speed = 0;
#endif

        // Уменьшаем скорость при yaw 160-200 градусов (в диапазоне -180..180 это 160..180 и -180..-160)
        if (mpu.yaw >= 160 || mpu.yaw < -160) {
            speed = min(speed, 55);
        }

        pid.setpoint(desiredAngle);
        turn = (int) -pid.compute(mpu.absoluteAngle());

        DEBUGF("SERVO turn = %d\n", turn);

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
    int desiredAngle = 0;
};
