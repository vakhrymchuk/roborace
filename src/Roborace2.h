#pragma once

#include "hal/Mechanics.h"
#include "hal/Lidar.h"
#include "Stopwatch.h"
#include "logic/LaserScan.h"

enum Strategy
{
    FORWARD,
    BACKWARD
};

class Roborace
{
public:
    int speedForward = 90;
    LaserScan scan;

    Roborace()
    {
        Lidar::init();
    }

    void loop()
    {
        scan.data.clear();
        while (!Lidar::getData(scan.data))
            mechanics.run(speed, turn);
        

        if (scan.data.size() > 50)
        {
            logic();
        }
        else
        {
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
    Interval debug = Interval(200);

    Mechanics mechanics;

    Strategy strategy = FORWARD;
    Stopwatch start;

    int speed = 0;
    int turn = ServoWrapper::CENTER;

    void logic()
    {
        switch (strategy)
        {
        case FORWARD:
            forward();
            break;
        case BACKWARD:
            backward();
            break;
        }
    }

    void forward()
    {
        int f = scan.findDistanceAtDegree(180);

        Serial.printf("forw = %d \t", f);

        if (f < 25 || scan.findDistanceAtDegree(180 - 10) < 25 || scan.findDistanceAtDegree(180 + 10) < 25)
        {
            newStrategy(BACKWARD);
            return;
        }

        double currentSpeed = mechanics.getEngine().getSpeed();
        // speed = map(constrain(f, 60, 200), 60, 200, 70, 80);
        speed = 70;

        int error = 0;

        if (f > 150)
        {
            speed = 85;
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
            speed = 65;
            for (size_t i = 1; i <= 3; i++)
            {
                int deg = 30 * i;
                int left = scan.findDistanceAtDegree(180 - deg);
                int right = scan.findDistanceAtDegree(180 + deg);
                error += left * log(left) - right * log(right);
                Serial.printf("left%d = %d \tright%d = %d \t", deg, left, deg, right);
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

#ifdef DEBUG
        Serial.printf("speed = %d \t turn = %d\n", speed, turn);
#endif
    }

    void backward()
    {
#ifdef DEBUG
        Serial.println("BACK");
#endif

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

    void rotate()
    {
#ifdef DEBUG
        Serial.print("          rotate!!!!     ");
        Serial.println(start.time());
#endif
        int rotateSpeed = 90;
        if (start.isLessThan(200) && scan.findDistanceAtDegree(180-30) > 40)
        {
            speed = rotateSpeed;
            turn = ServoWrapper::FULL_LEFT;
        }
        else if (start.isLessThan(1600))
        {
            speed = -100;
            turn = ServoWrapper::FULL_RIGHT;
        }
        else if (start.isLessThan(2300) && scan.findDistanceAtDegree(180-30) > 40 && scan.findDistanceAtDegree(180+30) > 25)
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
#ifdef DEBUG
        Serial.println("NEW STRATEGY");
#endif
        strategy = newStrategy;
        start.start();
        logic();
    }
};
