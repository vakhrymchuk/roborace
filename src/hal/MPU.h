#pragma once

#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <Interval.h>

class MPU
{
public:
    int yaw = 0;   // ось рысканья Z
    int pitch = 0; // ось тангажа Y
    int roll = 0;  // ось крена X

private:
    MPU6050 mpu;
    uint8_t fifoBuffer[45]{};
    Quaternion q;
    VectorFloat gravity;
    float ypr[3]{};

    Interval read = Interval(22);
    int rotates = 0; // абсолютное значение поворотов
    int prevYaw = 0;

public:
    void init()
    {
        Wire.begin();
        mpu.initialize();
        mpu.dmpInitialize();
        mpu.setDMPEnabled(true);
    }

    int absoluteAngle()
    {
        return rotates * 360 + yaw;
    }

    void readData()
    {
        if (read.isReady() && mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
        {
            // расчёты
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

            // выводим результат в радианах (-3.14, 3.14)
            yaw = degrees(ypr[0]);
            pitch = degrees(ypr[1]);
            roll = degrees(ypr[2]);

            if (yaw * prevYaw < 0 && abs(yaw) > 150)
                rotates += (yaw < 0) ? 1 : -1;
            prevYaw = yaw;

            DEBUGRRF("y=%d\tp=%d\tr=%d\trot=%d\tangle=%d\n", yaw, pitch, roll, rotates, absoluteAngle());
        }
    }
};
