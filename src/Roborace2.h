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

    int minSpeed = 110;
    int speedForward = 140;
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

        double currentSpeed = mechanics.getEngine().getSpeed();
        double error = 0.7 * (sensors.l90d - sensors.r90d) + sensors.l45d - sensors.r45d;
        int diff = (int) (0.5 * error + 1.4 * (error - lastError));
        lastError = error;

//        int fd = constrain((sensors.r45d + sensors.l45d) / 2, 60, 150);

        int forwardD = sensors.calcForwardD(currentSpeed);
        if (forwardD < 100) {
//            speed = minSpeed;
            speed = map(forwardD, 60, 100, 90, minSpeed);
            if (sensors.l45d < 120 && sensors.r45d < 120) {
                if (sensors.l90d > 110) {
                    turn = 100;
                } else if (sensors.r90d > 110) {
                    turn = -100;
                }

            }
        } else if (forwardD > 200) {
            speed = speedTurbo;
        } else {
//            speed = speedForward;
            speed = map(forwardD, 100, 150, minSpeed, speedForward);
            speed = constrain(speed, minSpeed, speedForward);

//            if(sensors.l90d < 30 && sensors.r90d < 30 && sensors.forwardD >= 150) {
//                speed += 10;
//            }
        }

/*        if (sensors.l45d + sensors.r45d >= 270 || sensors.forwardD >= 150) {
            diff = diff / 2;

//            diff = sensors.r45d - sensors.l45d;

//            if (sensors.r45d >= sensors.l45d + 30) {
//                diff = 10;
//            }
//            if (sensors.l45d >= sensors.r45d + 30) {
//                diff = -10;
//            }
            turn = constrain(diff, -10, 10);
            speed = speedForward;

            if (sensors.forwardD >= 150) {
                fd = constrain(sensors.forwardD, 150, 300);
                speed = map(fd, 150, 300, speedForward, speedTurbo);
                speed = constrain(speed, speedForward, speedTurbo);
            }
        } else*//* if (sensors.r90d >= 145 && sensors.r90d > sensors.l90d) {
            turn = -80;
        } else if (sensors.l90d >= 145 && sensors.l90d > sensors.r90d) {
            turn = 80;
        } else*/ {
            turn = diff;
            turn = constrain(turn, -100, 100);
        };

        if (forwardD >= 150) {
            turn = constrain(turn, -15, 15);
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

        if (rotateCounter >= rotateLimit && forwardD >= 120 && sensors.l90d >= 60) {
            newStrategy(ROTATE);
            rotateCounter = 0;
            return;
        }

    }

    bool isNeedBack() const {

        if (sensors.isSamePlace(3000)) return true;
//        bool isLonger2Sec = start.isMoreThan(50);
        if (start.isLessThan(1000)) return false;
//        bool isFBack = sensors.forwardD <= 60 && sensors.isForwardLongerThan(100);
        bool isL0Back = sensors.r45d < 30 && sensors.isLeftLongerThan(50);
        bool isR0Back = sensors.l45d < 30 && sensors.isLeftLongerThan(50);
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
        int rotateSpeed = 100;
/*        if (start.isLessThan(60)) {
            speed = rotateSpeed;
            turn = ServoWrapper::FULL_RIGHT;
        } else */if (start.isLessThan(800)) {
            speed = rotateSpeed;
            turn = ServoWrapper::FULL_LEFT;
        } else if (start.isLessThan(2000)) {
            speed = -100;
            turn = ServoWrapper::FULL_RIGHT;
        } else if (start.isLessThan(2600)) {
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
