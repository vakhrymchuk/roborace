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
        bool isLonger2Sec = start.isMoreThan(250);
        bool isFBack = sensors.forwardD <= 60 && sensors.isForwardLongerThan(100);
        bool isLBack = sensors.l45d < 20 && sensors.isLeftLongerThan(250);
        bool isRBack = sensors.r45d < 20 && sensors.isRightLongerThan(250);
        if ((/*isFBack &&*/ (isLBack || isRBack)) && isLonger2Sec) {
            newStrategy(BACKWARD);
            return;
        }

        if (rotateCounter >= rotateLimit && sensors.forwardD >= 120 && sensors.l45d >= 60) {
            newStrategy(ROTATE);
            rotateCounter = 0;
            return;
        }

//        int d = constrain(sensors.forwardD, 10, 150);
//        turn = map(d, 10, 150, -100, 100);
//        return;

        double currentSpeed = mechanics.getEngine().getSpeed();
        double error = (sensors.l45d - sensors.r45d) / 4.0;
        int diff = (int) (1.8 * error + 3.0 * (error - lastError));
        lastError = error;

        speed = 2.3;
        float turboSpeed = 3.5;
        if (sensors.forwardD >= 270) {
            turn = diff;
            turn = constrain(turn, -3, 3);
            speed = 2.6;
//            int md = 450;
//            long fd = constrain(sensors.forwardD, 250, md);
//            speed = map(fd, 220, md, speed * 100, turboSpeed * 100) * 0.01;
        } else if (sensors.r45d >= 140 && sensors.r45d > sensors.l45d && sensors.forwardD <= 180) {
            turn = 0.62 * ServoWrapper::FULL_RIGHT;
        } else if (sensors.l45d >= 140 && sensors.forwardD <= 180) {
            turn = 0.62 * ServoWrapper::FULL_LEFT;
        } else {
            turn = diff;
            turn = constrain(turn, -12, 12);
        }

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
        if (start.isLessThan(60)) {
            speed = 1.5;
            turn = ServoWrapper::FULL_RIGHT;
        } else if (start.isLessThan(800)) {
            speed = 1.5;
            turn = ServoWrapper::FULL_LEFT;
        } else if (start.isLessThan(1700)) {
            speed = -4.0;
            turn = ServoWrapper::FULL_RIGHT;
        } else if (start.isLessThan(2000)) {
            speed = 1.5;
            turn = ServoWrapper::FULL_LEFT;
        } else {
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
