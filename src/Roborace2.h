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
    double lastError2 = 0;

    long rotateLimit = ServoWrapper::FULL_LEFT * 1800L;
    long rotateCounter = rotateLimit;


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

    int minSpeed = 100;
    int speedForward = 110;
    int speedTurbo = 170;

    void forward() {

//        if(start.isMoreThan(15, SECOND)) {
//            minSpeed += 2;
//            speedForward += 2;
//            speedTurbo += 2;
//            start.start();
//        }

        if (isNeedBack()) {
            newStrategy(BACKWARD);
            return;
        }

        int addSpeed = 0;
        if (start.isMoreThan(10, SECOND)) {
            addSpeed = 8;
        }

        double currentSpeed = mechanics.getEngine().getSpeed();
        double error = (sensors.l90d - sensors.r90d) + (sensors.l45d - sensors.r45d);
        int diff = (int) (0.5 * error + 1.5 * (error - lastError));
        lastError = error;

        int forwardD = sensors.calcForwardD(currentSpeed);
        if (forwardD < 110) {
//            speed = minSpeed;
            speed = map(forwardD, 50, 100, minSpeed + addSpeed / 2, speedForward + addSpeed);
        } else if (forwardD > 150) {
            speed = map(constrain(forwardD, 200, 400), 200, 400, speedForward + 30, speedTurbo);
            if (sensors.l90d < 40 && sensors.r90d < 40 && sensors.forwardD >= 150) {
                speed += 20;
            }
        } else {
            speed = speedForward + addSpeed;
        }
        turn = constrain(diff, -80, 80);


        if (sensors.l45d > 120 && sensors.r45d < 120) {
            turn = max(turn, 70);
        } else if (sensors.r45d > 120) {
            turn = min(turn, -70);
        } else if (sensors.l90d > 120 && sensors.r90d < 120) {
            turn = max(turn, 80);
        } else if (sensors.r90d > 120) {
            turn = min(turn, -80);
        }
        if (forwardD >= 130) {
            turn = constrain(turn, -15, 15);
        }
        if (forwardD >= 150) {
            error = (constrain(sensors.l45d, 0, 100) - 45);
            diff = (int) (0.5 * error + 1.2 * (error - lastError2));
            lastError2 = error;

            turn = diff;
            turn = constrain(turn, -10, 10);

#ifdef DEBUG
            Serial.print("error = ");
            Serial.print(error);
            Serial.print(" diff = ");
            Serial.println(diff);
#endif
        }

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

        if (rotateCounter <= -rotateLimit && sensors.l45d >= 50 && sensors.l90d >= 40) {
            newStrategy(ROTATE);
            rotateCounter = rotateLimit;
            return;
        }

    }

    bool isNeedBack() const {

        if (sensors.isSamePlace(3000)) return true;
//        bool isLonger2Sec = start.isMoreThan(50);
        if (start.isLessThan(1000)) return false;
//        bool isFBack = sensors.forwardD <= 60 && sensors.isForwardLongerThan(100);
        bool isL0Back = sensors.r45d < 25 && sensors.isLeftLongerThan(50);
        bool isR0Back = sensors.l45d < 25 && sensors.isLeftLongerThan(50);
//        bool isL45Back = sensors.l90d < 20 && sensors.isLeftLongerThan(250);
//        bool isR45Back = sensors.r90d < 20 && sensors.isRightLongerThan(250);
        return isL0Back || isR0Back;
//        return (isL0Back && isR0Back) || ((isL0Back || isR0Back) && start.isLessThan(1000));
    }

    void backward() {
#ifdef DEBUG
        Serial.println("BACK");
#endif

        double error = sensors.l90d - sensors.r90d;
        int diff = (int) (10.0 * error);

        turn = -diff;

        speed = -130;
//        turn = -10;
        if (start.isLessThan(50)) {
            speed = 0;
        } else if (start.isMoreThan(1000) &&
                   (
                           sensors.l45d > 80 || sensors.r45d > 80
                           //                           || sensors.l90d > 70 || sensors.r90d > 70
                           || start.isMoreThan(3, SECOND))
                ) {
            newStrategy(FORWARD);
        }
    }

    void leftWall() {
        speed = 100;
        double error = sensors.l90d - 45;

        int diff = (int) (0.5 * error + 1.8 * (error - lastError));
        lastError = error;
        turn = diff;

        if (sensors.r45d < 45 && sensors.l90d < 60) {
            turn = -35;
        }
    }

    void rightWall() {
        if (isNeedBack()) {
            newStrategy(BACKWARD);
            return;
        }

        speed = 100;
        double error = (50.0 - sensors.r90d);

        int diff = (int) (0.5 * error + 1.0 * (error - lastError));
        lastError = error;
        turn = diff;
        if (sensors.l45d < 120 && sensors.r90d < 80) {
            turn = 80;
        }
    }


    void rotate() {
#ifdef DEBUG
        Serial.print("          rotate!!!!     ");
        Serial.println(start.time());
#endif
        int rotateSpeed = 90;
        if (start.isLessThan(200) && sensors.l45d > 40) {
            speed = rotateSpeed;
            turn = ServoWrapper::FULL_LEFT;
        } else if (start.isLessThan(1600)) {
            speed = -100;
            turn = ServoWrapper::FULL_RIGHT;
        } else if (start.isLessThan(2300) && sensors.l45d > 40 && sensors.r45d > 25) {
            speed = rotateSpeed;
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
