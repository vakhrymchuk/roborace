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
    Param *speedParam = new Param(110, "speed", "solution");
    Param *maxErrorParam = new Param(1200, "pid-max-error", "solution");
    Param *rotationPitchDegParam = new Param(7, "rotation-pitch-deg", "solution");
    Param *backDistParam = new Param(20, "back-dist", "solution");

    Param *turboSpeedParam = new Param(100, "turbo-speed", "turbo");
    Param *turboDistParam = new Param(130, "turbo-dist", "turbo");
    Param *turboAngleParam = new Param(55, "turbo-angle", "turbo");
    Param *turboMaxErrorParam = new Param(180, "turbo-max-err", "turbo");
    Param *turboMaxAngleParam = new Param(8, "turbo-max-angle", "turbo");

private:
    Strategy strategy = FORWARD;
    Stopwatch start;
    Stopwatch gorka;
    Stopwatch backStopwatch;
    bool isBack = false;

    int absoluteAngleMax = 0;
    int absoluteAngleMin = 0;
    int lastError = 0;

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
        // if (millis() > 10000 && data.pitch > rotationPitchDegParam->value && abs(data.yaw - 0) < degRange)
        if (isCounterClockWise(data, 180))
        {
            desiredRotate = data.absoluteAngle - 170;
            newStrategy(ROTATE);
            absoluteAngleMax = data.absoluteAngle;
            absoluteAngleMin = data.absoluteAngle;
            return;
        }
        if (data.pitch > 10 && gorka.isMoreThan(10, SECOND))
        {
            gorka.start();
        }

        if (data.pitch > 10 && gorka.isMoreThan(3, SECOND))
        {
            newStrategy(BACKWARD);
            gorka.start();
            return;
        }

        int f = data.scan.findDistanceAtDegree(180);

        DEBUGRRF("forw = %d \t", f);

        if (data.pitch < 5 && (f < backDistParam->value ||
                               data.scan.findDistanceAtDegree(180 - 10) < backDistParam->value ||
                               data.scan.findDistanceAtDegree(180 - 20) < backDistParam->value ||
                               data.scan.findDistanceAtDegree(180 + 10) < backDistParam->value ||
                               data.scan.findDistanceAtDegree(180 + 20) < backDistParam->value))
        {
            if (!isBack)
            {
                isBack = true;
                backStopwatch.start();
            }
            if (backStopwatch.isMoreThan(500))
            {
                newStrategy(BACKWARD);
                return;
            }
        }
        else
            isBack = false;

        double currentSpeed = data.speed;

        speed = speedParam->value;
        if (this->start.isMoreThan(10, SECOND))
            speed += 10;

        int maxDist = 0;
        int maxDistAngle = 0;
        for (size_t i = 0; i <= 9; i++) // find longes dist and it angle
        {
            int deg = 10 * i;
            int left = data.scan.findDistanceAtDegree(180 - deg) - i * 0;
            int right = data.scan.findDistanceAtDegree(180 + deg) - i * 0;
            // if (data.pitch > 15) {
            //     left = min(300, left);
            //     right = min(300, right);
            // }
            if (gorka.isLessThan(5, SECOND)) // limit dist on gorka
            {
                left = min(left, 150);
                right = min(right, 150);
            }
            if (left > maxDist)
            {
                maxDist = left;
                maxDistAngle = -deg;
            }
            if (right > maxDist)
            {
                maxDist = right;
                maxDistAngle = deg;
            }
            if (maxDist > 160)
                break;
        }

        if(data.pitch < -5) {
            maxDistAngle /= 2;
        }

        int mid = 110;

        if (maxDist <= mid)
            speed += map(constrain(maxDist, 50, mid), 50, mid, -10, 0);
        // else if (f > 150)
        // speed += map(constrain(f, 150, 300), 150, 300, 0, 40);
        // if (f > 200)
        //     speed += 10;

        turn = maxDistAngle;

        // if (data.pitch > 5)
        // {
        //     if (data.scan.findDistanceAtDegree(180 + 90) < 30)
        //     {
        //         turn = -10;
        //     }
        //     if (data.scan.findDistanceAtDegree(180 - 90) < 30)
        //     {
        //         turn = 10;
        //     }
        // }

        DEBUGRRF("max dist angle = %d \t max dist = %d\n", maxDistAngle, maxDist);
        DEBUGRRF("speed = %d \t turn = %d\n", speed, turn);
    }

    bool isCounterClockWise(PhysicalData &data, int threshold)
    {
        absoluteAngleMax = max(absoluteAngleMax, data.absoluteAngle);
        DEBUGRRF("absoluteAngleMax = %d\n", absoluteAngleMax);
        return absoluteAngleMax - data.absoluteAngle > threshold;
    }

    bool isClockWise(PhysicalData &data, int threshold)
    {
        absoluteAngleMin = min(absoluteAngleMin, data.absoluteAngle);
        return absoluteAngleMin - data.absoluteAngle < -threshold;
    }

    void backward(PhysicalData &data, int &speed, int &turn)
    {
        DEBUGRR("BACK\n");
        speed = -70;

        int left = data.scan.findDistanceAtDegree(180 - 55);
        int right = data.scan.findDistanceAtDegree(180 + 55);
        int error = left - right;
        int maxError = 100;
        error = constrain(error, -maxError, maxError);
        turn = map(error, -maxError, maxError, -100, 100);
        int maxTurn = 30;
        turn = constrain(turn, -maxTurn, maxTurn);

        bool needToCheckMinTime = data.pitch > 10;
        bool minTimeOk = needToCheckMinTime == false || start.isMoreThan(1200);

        if (start.isMoreThan(2000) || (minTimeOk && data.scan.findDistanceAtDegree(180 - 10) > 40 && data.scan.findDistanceAtDegree(180 + 10) > 40))
        {
            newStrategy(FORWARD);
        }
    }

    int desiredRotate = 0;

    void rotate(PhysicalData &data, int &speed, int &turn)
    {
        DEBUGRRF("   rotate!!  %d", start.time());
        int rotateSpeed = 80;
        if (start.isLessThan(500) && data.scan.findDistanceAtDegree(180 - 30) > 20)
        {
            speed = rotateSpeed;
            turn = 90;
        }
        else if (start.isLessThan(1600))
        {
            speed = -rotateSpeed;
            turn = -90;
        }
        else if (start.isLessThan(2300) && data.scan.findDistanceAtDegree(180 - 30) > 40 && data.scan.findDistanceAtDegree(180 + 30) > 25)
        {
            speed = rotateSpeed;
            turn = 90;
        }
        else
        {
            newStrategy(FORWARD);
        }
    }

    void rotatePitch(PhysicalData &data, int &speed, int &turn)
    {
        DEBUGRRF("   rotate pitch!!  %d", start.time());
        int rotateSpeed = 90;
        if (start.isMoreThan(1000) && (start.isLessThan(1000) || data.pitch > 5))
        {
            speed = -rotateSpeed;
            turn = 0;
        }
        else if (start.isLessThan(2000) /* && data.scan.findDistanceAtDegree(0) > 40 */)
        {
            speed = -rotateSpeed;
            turn = -30;
        }
        else if (start.isLessThan(2500))
        {
            speed = rotateSpeed;
            turn = 50;
            if (data.scan.findDistanceAtDegree(180 - 10) > 150)
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
        DEBUGRR("NEW STRATEGY\n");
        strategy = newStrategy;
        start.start();
    }
};
