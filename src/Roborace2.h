#pragma once

#include "hal/Mechanics.h"
#include "hal/Sensors.h"
#include "Stopwatch.h"


template<typename T>
int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

enum Strategy {
    FORWARD, BACKWARD, LEFT_WALL, RIGHT_WALL, ROTATE
};

class Roborace {
public:

    void loop() {
        if (!main.isReady()) return;

        sensors.read();

        logic();

#ifdef WAIT_5S
        if (millis() < 3500) speed = 0;
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

    int speed = 0;
    int turn = ServoWrapper::CENTER;
    double lastError = 0;

    long rotateLimit = ServoWrapper::FULL_LEFT * 18 * 200L;
    long rotateCounter = -rotateLimit;


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
            case LEFT_WALL:
                leftWall();
                break;
            case RIGHT_WALL:
                rightWall();
                break;
        }
    }

    void forward() {

        if (isNeedBack()) {
            newStrategy(BACKWARD);
            return;
        }

        double currentSpeed = mechanics.getEngine().getSpeed();
        double error = sensors.l45d - sensors.r45d;
        int diff = (int) (1.0 * error + 2.0 * (error - lastError));
        lastError = error;

        int fd = constrain(min(sensors.l0d, sensors.r0d), 60, 150);
        speed = map(fd, 60, 150, 100, 190);
        speed = constrain(speed, 100, 190);

        if (sensors.r0d + sensors.l0d >= 250) {
            turn = constrain(diff, -10, 10);

            if (sensors.forwardD >= 200) {
                fd = constrain(sensors.forwardD, 200, 300);
                speed = map(fd, 200, 300, 200, 300);
                speed = constrain(speed, 200, 300);
            }
        } else if (sensors.r45d >= 150 && sensors.r45d > sensors.l45d) {
            turn = -70;
        } else if (sensors.l45d >= 150) {
            turn = 70;
        } else {
            turn = diff;
            turn = constrain(turn, -100, 100);
        };

//        int maxTurn = mechanics.getEngine().getSpeed();

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

        if (rotateCounter >= rotateLimit && sensors.forwardD >= 120 && sensors.l45d >= 60) {
            newStrategy(ROTATE);
            rotateCounter = 0;
            return;
        }

    }

    bool isNeedBack() const {
//        bool isLonger2Sec = start.isMoreThan(50);
//        bool isFBack = sensors.forwardD <= 60 && sensors.isForwardLongerThan(100);
        bool isL0Back = sensors.l0d < 35 && sensors.isLeftLongerThan(40);
        bool isR0Back = sensors.r0d < 35 && sensors.isLeftLongerThan(40);
//        bool isL45Back = sensors.l45d < 20 && sensors.isLeftLongerThan(250);
//        bool isR45Back = sensors.r45d < 20 && sensors.isRightLongerThan(250);
        return isL0Back || isR0Back;
    }

    void backward() {
#ifdef DEBUG
        Serial.println("BACK");
#endif
        speed = -100;
        turn = -30;
        if (start.isLessThan(50)) {
            speed = 0;
        } else if (start.isMoreThan(700) &&
                   (
                           sensors.r0d > 50 || sensors.l0d > 50
                           || sensors.l45d > 70 || sensors.r45d > 70
                           || start.isMoreThan(4, SECOND))
                ) {
            newStrategy(FORWARD);
        }
    }

    void leftWall() {
        speed = 200;
        double error = sensors.l45d - 45;

        int diff = (int) (0.5 * error + 1.8 * (error - lastError));
        lastError = error;
        turn = diff;

        if (sensors.l0d < 45 && sensors.l45d < 60) {
            turn = -35;
        }
    }

    void rightWall() {
        if (isNeedBack()) {
            newStrategy(BACKWARD);
            return;
        }

        speed = 300;
        double error = (50.0 - sensors.r45d);

        int diff = (int) (0.5 * error + 1.0 * (error - lastError));
        lastError = error;
        turn = diff;
        if (sensors.r0d < 120 && sensors.r45d < 80) {
            turn = 80;
        }
    }


    void rotate() {
#ifdef DEBUG
        Serial.print("          rotate!!!!     ");
        Serial.println(start.time());
#endif
        if (start.isLessThan(60)) {
            speed = 150;
            turn = ServoWrapper::FULL_RIGHT;
        } else if (start.isLessThan(800)) {
            speed = 150;
            turn = ServoWrapper::FULL_LEFT;
        } else if (start.isLessThan(2000)) {
            speed = -100;
            turn = ServoWrapper::FULL_RIGHT;
        } else if (start.isLessThan(2300)) {
            speed = 150;
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
