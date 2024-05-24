#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "Stopwatch.h"

enum Strategy
{
    FORWARD,
    BACKWARD
};

class Solution
{
public:
private:
    Strategy strategy = FORWARD;
    Stopwatch start;

public:
    void logic(LaserScan &scan, int &speed, int &turn)
    {
        switch (strategy)
        {
        case FORWARD:
            forward(scan, speed, turn);
            break;
        case BACKWARD:
            backward(scan, speed, turn);
            break;
        }
    }

private:
    void forward(LaserScan &scan, int &speed, int &turn)
    {
        int f = scan.findDistanceAtDegree(180);

        DEBUGF("forw = %d \t", f);

        if (f < 25 || scan.findDistanceAtDegree(180 - 10) < 25 || scan.findDistanceAtDegree(180 + 10) < 25)
        {
            newStrategy(BACKWARD);
            return;
        }

        // double currentSpeed = mechanics.getEngine().getSpeed();
        // speed = map(constrain(f, 60, 200), 60, 200, 70, 80);
        speed = 70;

        int error = 0;

        if (f > 120)
        {
            speed = 100;
            int left = scan.findDistanceAtDegree(180 - 55);
            int right = scan.findDistanceAtDegree(180 + 55);
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
                int left = scan.findDistanceAtDegree(180 - deg);
                int right = scan.findDistanceAtDegree(180 + deg);
                error += left * log(left) - right * log(right);
                DEBUGF("left%d = %d \tright%d = %d \t", deg, left, deg, right);
            }

            int maxError = 1200;
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

    void backward(LaserScan &scan, int &speed, int &turn)
    {
        DEBUGF("BACK");

        speed = -50;
        turn = 0;
        if (start.isLessThan(150))
        {
            speed = 0;
        }
        else if (start.isMoreThan(2000) || scan.findDistanceAtDegree(180) > 40)
        {
            newStrategy(FORWARD);
        }
    }

    void rotate(LaserScan &scan, int &speed, int &turn)
    {
        DEBUGF("   rotate!!  %d", start.time());
        int rotateSpeed = 90;
        if (start.isLessThan(200) && scan.findDistanceAtDegree(180 - 30) > 40)
        {
            speed = rotateSpeed;
            turn = ServoWrapper::FULL_LEFT;
        }
        else if (start.isLessThan(1600))
        {
            speed = -100;
            turn = ServoWrapper::FULL_RIGHT;
        }
        else if (start.isLessThan(2300) && scan.findDistanceAtDegree(180 - 30) > 40 && scan.findDistanceAtDegree(180 + 30) > 25)
        {
            speed = rotateSpeed;
            turn = ServoWrapper::FULL_LEFT;
        }
        else
        {
            newStrategy(FORWARD);
        }
    }

    void newStrategy(const Strategy newStrategy)
    {
        DEBUGF("NEW STRATEGY");
        strategy = newStrategy;
        start.start();
    }
};
