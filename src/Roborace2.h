#pragma once

#include "hal/Mechanics.h"
#include "hal/Sensors.h"
#include "Stopwatch.h"


template<typename T>
int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

enum Strategy {
    FORWARD, BACKWARD, ROTATE
};

class Roborace {
public:

    void loop() {
        if (!main.isReady()) return;

        sensors.read();

        logic();

#ifdef WAIT_5S
        if (millis() < 4900) speed = 0;
#endif

        mechanics.run(speed, turn);
    }

private:
    Interval main = Interval(20);
    Interval debug = Interval(200);

    Mechanics mechanics;
    Sensors sensors;

    Strategy strategy = FORWARD;
    Stopwatch start;

    float speed = 0;
    int turn = ServoWrapper::CENTER;
    double lastError = 0;

    int32_t rotateLimit = ServoWrapper::FULL_LEFT * 14 * 2.0;
    int32_t rotateCounter = -rotateLimit;


    void logic() {
        switch (strategy) {
            case FORWARD:
                forward();
                break;
            case BACKWARD:
                backward();
                break;
            case ROTATE:
                rotate();
                break;
        }
    }

    void forward() {
        bool isLonger2Sec = start.isMoreThan(2, SECOND);
        bool isFBack = sensors.forwardD <= 60 && sensors.isForwardLongerThan(100);
        bool isLBack = sensors.l45d < 20 && sensors.isLeftLongerThan(250);
        bool isRBack = sensors.r45d < 20 && sensors.isRightLongerThan(250);
        if ((/*isFBack &&*/ (isLBack || isRBack)) && isLonger2Sec) {
            newStrategy(BACKWARD);
            return;
        }

        if (rotateCounter >= rotateLimit) {
            newStrategy(ROTATE);
            rotateCounter = 0;
            return;
        }

//        int d = constrain(sensors.forwardD, 10, 150);
//        turn = map(d, 10, 150, -100, 100);
//        return;

        double currentSpeed = mechanics.getEngine().getSpeed();
        double error = (sensors.l45d - sensors.r45d) / 4.0;
        int diff = (int) (0.5 * error + 2.0 * (error - lastError));
        lastError = error;

        speed = 2.3;
        if (sensors.forwardD > 200) {
            turn = diff;
            turn = constrain(turn, -10, 10);
        } else if (sensors.r45d >= 130 && sensors.r45d > sensors.l45d && sensors.forwardD <= 200) {
            turn = 0.80 * ServoWrapper::FULL_RIGHT;
        } else if (sensors.l45d >= 130 && sensors.forwardD <= 200) {
            turn = 0.80 * ServoWrapper::FULL_LEFT;
        } else {
//            speed = 2.5;
            turn = diff;
            turn = constrain(turn, -15, 15);
            if (sensors.forwardD >= 300) {
                speed = 2.5;
                turn = constrain(turn, -2, 2);
            }
        }

        int md = 350;
        long fd = constrain(sensors.forwardD, 50, md);
//        int maxTurn = map(fd, 50, 400, 100, 1);

//        turn = constrain(turn, -maxTurn, maxTurn);

//        speed = map(fd, 50, md, 24, 28) * 0.1;

//        if (currentSpeed >= 2.5) turn = constrain(turn, -60, 60);

#ifdef DEBUG
        if (debug.isReady()) {
            Serial.print(speed);
            Serial.print(" ");
            Serial.print(turn);
            Serial.print(" ");
            Serial.println(rotateCounter);
        }
#endif
//        speed = 0;

        rotateCounter += currentSpeed * turn;
        rotateCounter = constrain(rotateCounter, -rotateLimit, rotateLimit);

    }

    void backward() {
#ifdef DEBUG
        Serial.println("BACK");
#endif
        //        return min(l0d, forwardD);
        if (start.isMoreThan(1, SECOND) &&
            (sensors.forwardD > 60 || sensors.r45d > 80 || sensors.l45d > 80 || start.isMoreThan(2, SECOND))) {
            newStrategy(FORWARD);
            return;
        }
        speed = -2.5;
        turn = ServoWrapper::CENTER;

    }


    void rotate() {
#ifdef DEBUG
        Serial.print("          rotate!!!!     ");
        Serial.println(start.time());
#endif
        if (start.isLessThan(38)) {
            speed = 5.0;
            turn = ServoWrapper::FULL_RIGHT;
        } else if (start.isLessThan(900)) {
            speed = 5.0;
            turn = ServoWrapper::FULL_LEFT;
        } else /*if (start.isLessThan(2500)) {
            speed = -2.0;
            turn = ServoWrapper::FULL_RIGHT;
        } else */{
            newStrategy(FORWARD);
        }

    }

    void newStrategy(const Strategy newStrategy) {
#ifdef DEBUG
        Serial.println("NEW STRATEGY");
#endif
        strategy = newStrategy;
        start.start();
        logic();
    }

};
