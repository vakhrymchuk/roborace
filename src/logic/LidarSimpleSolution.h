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
    Param *speedParam = new Param(80, "speed", "solution");
    Param *maxErrorParam = new Param(1200, "pid-max-error", "solution");
    Param *rotationPitchDegParam = new Param(7, "rotation-pitch-deg", "solution");
    Param *backDistParam = new Param(25, "back-dist", "solution");

    Param *turboSpeedParam = new Param(100, "turbo-speed", "turbo");
    Param *turboDistParam = new Param(130, "turbo-dist", "turbo");
    Param *turboAngleParam = new Param(55, "turbo-angle", "turbo");
    Param *turboMaxErrorParam = new Param(180, "turbo-max-err", "turbo");
    Param *turboMaxAngleParam = new Param(8, "turbo-max-angle", "turbo");

private:
    Strategy strategy = FORWARD;
    Stopwatch start;
    Stopwatch gorka;

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
        if (millis() > 10000 && data.pitch > rotationPitchDegParam->value && abs(data.yaw - 0) < degRange)
        {
            newStrategy(ROTATE_PITCH);
            absoluteAngleMax = data.absoluteAngle;
            absoluteAngleMin = data.absoluteAngle;
            return;
        }
        if (data.pitch > 5)
        {
            gorka.start();
        }

        // if (isCounterClockWise(data, 330))
        // {
        //     newStrategy(ROTATE);
        //     absoluteAngleMax = data.absoluteAngle;
        //     absoluteAngleMin = data.absoluteAngle;
        //     return;
        // }

        int f = data.scan.findDistanceAtDegree(180);

        DEBUGRRF("forw = %d \t", f);

        if (data.pitch < 5 && (f < backDistParam->value ||
                               data.scan.findDistanceAtDegree(180 - 10) < backDistParam->value ||
                               data.scan.findDistanceAtDegree(180 + 10) < backDistParam->value))
        {
            newStrategy(BACKWARD);
            return;
        }

        double currentSpeed = data.speed;
        // speed = map(constrain(f, 60, 200), 60, 200, 70, 80);

        int error = 0;

        if (f > turboDistParam->value /* && data.scan.findDistanceAtDegree(180-15) > turboDistParam->value
        && data.scan.findDistanceAtDegree(180+15) > turboDistParam->value */
        )
        {
            speed = turboSpeedParam->value;
            if (f > 150 && gorka.isMoreThan(3000))
                speed += map(constrain(f, 150, 400), 150, 400, 5, 10);
            int left = data.scan.findDistanceAtDegree(180 - turboAngleParam->value);
            int right = data.scan.findDistanceAtDegree(180 + turboAngleParam->value);
            left = constrain(left, 0, 100);
            right = constrain(right, 0, 100);
            error = left - right;
            int maxError = turboMaxErrorParam->value;
            error = constrain(error, -maxError, maxError);
            turn = map(error, -maxError, maxError, -100, 100);
            int maxTurn = turboMaxAngleParam->value;
            turn = constrain(turn, -maxTurn, maxTurn);
        }
        else
        {
            speed = speedParam->value;
            for (size_t i = 1; i <= 3; i++)
            {
                int deg = 30 * i;
                int left = data.scan.findDistanceAtDegree(180 - deg);
                int right = data.scan.findDistanceAtDegree(180 + deg);
                error += (left * log(left) - right * log(right));
                DEBUGRRF("left%d = %d \tright%d = %d \t", deg, left, deg, right);
            }

            int maxError = maxErrorParam->value;
            error = constrain(error, -maxError, maxError);
            if (f < 50)
            {
                error = error * 5;
                if (error == 0)
                    error = 2000;
            }

            int sum = 0.1 * error + 0.5 * (error - lastError);
            lastError = error;

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
        speed = -50;

        int left = data.scan.findDistanceAtDegree(180 - 55);
        int right = data.scan.findDistanceAtDegree(180 + 55);
        int error = left - right;
        int maxError = 100;
        error = constrain(error, -maxError, maxError);
        turn = map(error, -maxError, maxError, -100, 100);
        int maxTurn = 30;
        turn = constrain(turn, -maxTurn, maxTurn);

        if (start.isMoreThan(2000) || (data.scan.findDistanceAtDegree(180 - 10) > 40 && data.scan.findDistanceAtDegree(180 + 10) > 40))
        {
            newStrategy(FORWARD);
        }
    }

    void rotate(PhysicalData &data, int &speed, int &turn)
    {
        DEBUGRRF("   rotate!!  %d", start.time());
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
        DEBUGRRF("   rotate pitch!!  %d", start.time());
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
