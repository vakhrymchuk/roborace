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
        bool isFBack = sensors.forwardD < 20 && sensors.isForwardLongerThan(100);
        bool isLBack = sensors.l45d < 20 && sensors.isLeftLongerThan(100);
        bool isRBack = sensors.r45d < 20 && sensors.isRightLongerThan(100);
        if ((/*isFBack || */isLBack || isRBack) && isLonger2Sec) {
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
        speed = 3.0;
        if (sensors.r45d >= 130 && sensors.r45d > sensors.l45d) {
            turn = 0.6 * ServoWrapper::FULL_RIGHT;
        } else if (sensors.l45d >= 130) {
            turn = 0.6 * ServoWrapper::FULL_LEFT;
        } else {
            speed = 3.6;
            turn = 0.5 * error;
            turn = constrain(turn, -3, 3);
        }

        if (currentSpeed >= 3.4) turn = constrain(turn, -60, 60);

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
        if (start.isLessThan(38)) {
            speed = 4.0;
            turn = ServoWrapper::FULL_RIGHT;
        } else if (start.isLessThan(900)) {
            speed = 4.0;
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
