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

    int32_t rotateLimit = ServoWrapper::FULL_LEFT * 30 * 2.0;
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
        bool isFBack = sensors.forwardD < 20 && sensors.isForwardLongerThan(200);
        bool isLBack = sensors.l45d < 20 && sensors.isLeftLongerThan(200);
        bool isRBack = sensors.r45d < 20 && sensors.isRightLongerThan(200);
        if ((isFBack || isLBack || isRBack) && isLonger2Sec) {
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
        speed = 3.1;
        if (sensors.r45d >= 130 && sensors.r45d > sensors.l45d) {
            turn = 0.6 * ServoWrapper::FULL_RIGHT;
        } else if (sensors.l45d >= 130) {
            turn = 0.6 * ServoWrapper::FULL_LEFT;
        } else {
            speed = 3.8;
//            turn = sensors.l45d > sensors.r45d ? ServoWrapper::FULL_LEFT : ServoWrapper::FULL_RIGHT;
/*            if (sensors.forwardD >= 80) {
                speed = 3.0;
                 turn = 1.0 * error;
//                turn = 0;
                turn = constrain(turn, -10, 10);
            } else */{
                turn = 1.0 * error;

//                int angle = map(sensors.forwardD, 30, 70, 100, 0);
//                turn = angle * sign(error);

                turn = constrain(turn, -10, 10);
//                turn = 0;
            }
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
            (sensors.forwardD > 30 || sensors.r45d > 80 || sensors.l45d > 80 || start.isMoreThan(2, SECOND))) {
            newStrategy(FORWARD);
            return;
        }
        speed = -2.0;
        turn = ServoWrapper::CENTER;

    }


    void rotate() {
#ifdef DEBUG
        Serial.print("          rotate!!!!     ");
        Serial.println(start.time());
#endif
        if (start.isLessThan(1000)) {
            speed = 2.0;
            turn = ServoWrapper::FULL_LEFT;
        } else if (start.isLessThan(2500)) {
            speed = -2.0;
            turn = ServoWrapper::FULL_RIGHT;
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
