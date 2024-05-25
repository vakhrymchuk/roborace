#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "Stopwatch.h"

enum Strategy
{
    FORWARD,
    BACKWARD,
    ROTATE,
    ROTATE_PITCH,
};

class Solution
{
public:
private:
    Strategy strategy = FORWARD;
    Stopwatch start;

    int absoluteAngleMax = 0;
    int absoluteAngleMin = 0;

public:
    void logic(PhysicalData &data, int &speed, int &turn)
    {
        switch (strategy)
        {
        case FORWARD:
            forward(data, speed, turn);
            break;
        case BACKWARD:
            backward(data, speed, turn);
            break;
        case ROTATE:
            rotate(data, speed, turn);
            break;
        case ROTATE_PITCH:
            rotatePitch(data, speed, turn);
            break;
        }
    }

private:
    void forward(PhysicalData &data, int &speed, int &turn)
    {
        int degRange = 30;
        DEBUGF("size = %d \tpitch = %d \tyaw = %d\t", data.scan.data.size(), data.pitch, data.yaw);
        // if (data.pitch > 10 && (data.yaw > 180 - degRange || data.yaw < -180 + degRange))
        if (millis() > 10000 && data.pitch > 7 && abs(data.yaw - 0) < degRange)
        {
            newStrategy(ROTATE_PITCH);
            absoluteAngleMax = data.absoluteAngle;
            absoluteAngleMin = data.absoluteAngle;
            return;
        }

        // if (isCounterClockWise(data, 330))
        // {
        //     newStrategy(ROTATE);
        //     absoluteAngleMax = data.absoluteAngle;
        //     absoluteAngleMin = data.absoluteAngle;
        //     return;
        // }

        int f = data.scan.findDistanceAtDegree(180);

        DEBUGF("forw = %d \t", f);

        if (data.pitch < 5 && (f < 25 || data.scan.findDistanceAtDegree(180 - 10) < 25 || data.scan.findDistanceAtDegree(180 + 10) < 25))
        {
            newStrategy(BACKWARD);
            return;
        }

        double currentSpeed = data.speed;
        // speed = map(constrain(f, 60, 200), 60, 200, 70, 80);
        speed = 70;

        int error = 0;

        if (f > 120)
        {
            speed = 95;
            int left = data.scan.findDistanceAtDegree(180 - 55);
            int right = data.scan.findDistanceAtDegree(180 + 55);
            error = left - right;
            int maxError = 200;
            error = constrain(error, -maxError, maxError);
            turn = map(error, -maxError, maxError, -100, 100);
            int maxTurn = 8;
            turn = constrain(turn, -maxTurn, maxTurn);
        }
        else
        {
            speed = 80;
            for (size_t i = 1; i <= 3; i++)
            {
                int deg = 30 * i;
                int left = data.scan.findDistanceAtDegree(180 - deg);
                int right = data.scan.findDistanceAtDegree(180 + deg);
                error += left * log(left) - right * log(right);
                DEBUGF("left%d = %d \tright%d = %d \t", deg, left, deg, right);
            }

            int maxError = 1100;
            error = constrain(error, -maxError, maxError);
            turn = map(error, -maxError, maxError, -100, 100);
        }

        // if (scan.findDistanceAtDegree(180 - 90) > 140)
        // {
        //     turn = 80;
        // }
        // else if (scan.findDistanceAtDegree(180 + 90) > 140)
        // {
        //     turn = -80;
        // }
        // else if (scan.findDistanceAtDegree(180 - 45) > 140)
        // {
        //     turn = 60;
        // }
        // else if (scan.findDistanceAtDegree(180 + 45) > 140)
        // {
        //     turn = -60;
        // }

        DEBUGF("speed = %d \t turn = %d\n", speed, turn);
    }

    bool isCounterClockWise(PhysicalData &data, int threshold)
    {
        absoluteAngleMax = max(absoluteAngleMax, data.absoluteAngle);
        DEBUGF("absoluteAngleMax = %d\n", absoluteAngleMax);
        return absoluteAngleMax - data.absoluteAngle > threshold;
    }

    bool isClockWise(PhysicalData &data, int threshold)
    {
        absoluteAngleMin = min(absoluteAngleMin, data.absoluteAngle);
        return absoluteAngleMin - data.absoluteAngle < -threshold;
    }

    void backward(PhysicalData &data, int &speed, int &turn)
    {
        DEBUGF("BACK\n");
        speed = -50;

        int left = data.scan.findDistanceAtDegree(180 - 55);
        int right = data.scan.findDistanceAtDegree(180 + 55);
        int error = left - right;
        int maxError = 100;
        error = constrain(error, -maxError, maxError);
        turn = map(error, -maxError, maxError, -100, 100);
        int maxTurn = 40;
        turn = constrain(turn, -maxTurn, maxTurn);

        if (start.isMoreThan(2000) || (data.scan.findDistanceAtDegree(180 - 10) > 40 && data.scan.findDistanceAtDegree(180 + 10) > 40))
        {
            newStrategy(FORWARD);
        }
    }

    void rotate(PhysicalData &data, int &speed, int &turn)
    {
        DEBUGF("   rotate!!  %d", start.time());
        int rotateSpeed = 60;
        if (start.isLessThan(200) && data.scan.findDistanceAtDegree(180 - 30) > 40)
        {
            speed = -rotateSpeed;
            turn = 0;
        }
        else if (start.isLessThan(1600))
        {
            speed = -rotateSpeed;
            turn = ServoWrapper::FULL_RIGHT;
        }
        else if (start.isLessThan(2300) && data.scan.findDistanceAtDegree(180 - 30) > 40 && data.scan.findDistanceAtDegree(180 + 30) > 25)
        {
            speed = rotateSpeed;
            turn = ServoWrapper::FULL_LEFT;
        }
        else
        {
            newStrategy(FORWARD);
        }
    }

    void rotatePitch(PhysicalData &data, int &speed, int &turn)
    {
        DEBUGF("   rotate pitch!!  %d", start.time());
        int rotateSpeed = 60;
        if (start.isMoreThan(1000) && (start.isLessThan(1000) || data.pitch > 5))
        {
            speed = -rotateSpeed;
            turn = 0;
        }
        else if (start.isLessThan(2000) && data.scan.findDistanceAtDegree(0) > 40)
        {
            speed = -rotateSpeed;
            turn = 30;
        }
        else if (start.isLessThan(2500))
        {
            speed = rotateSpeed;
            turn = ServoWrapper::FULL_RIGHT;
            if (data.scan.findDistanceAtDegree(180-10) > 150)
            {
                newStrategy(FORWARD);
            }
        }
        else
        {
            newStrategy(FORWARD);
        }
    }

    void newStrategy(const Strategy newStrategy)
    {
        DEBUGF("NEW STRATEGY\n");
        strategy = newStrategy;
        start.start();
    }
};
