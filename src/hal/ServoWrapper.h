#pragma once

#include <Servo.h>

//#define SERVO_CHECK true

class ServoWrapper : public Servo {
public:

    static const int CENTER = 0;
    static const int FULL_RIGHT = -100;
    static const int FULL_LEFT = 100;

    explicit ServoWrapper(byte pin, int centralPosition = 90, int fullRight = -56, int fullLeft = 58)
            : Servo(),
              centralPosition(centralPosition),
              fullRight(fullRight),
              fullLeft(fullLeft) {
        Servo::attach(pin);
#ifdef SERVO_CHECK
        for (int i = 0; i >= FULL_RIGHT; i-=5) {
            turn(i);
            delay(1);
        }
        for (int i = FULL_RIGHT; i <= FULL_LEFT; i+=5) {
            turn(i);
            delay(1);
        }
        for (int i = FULL_LEFT; i >= 0; i-=5) {
            turn(i);
            delay(1);
        }
#endif
    }

    void turn(int turn) {
        turn = constrain(turn, FULL_RIGHT, FULL_LEFT);
        int angle = (int) (turn >= 0 ? map(turn, 0, FULL_LEFT, 0, fullLeft) : map(turn, FULL_RIGHT, 0, fullRight, 0));
        Servo::write(angle + centralPosition);
    }

private:
    const int centralPosition;
    const int fullRight;
    const int fullLeft;
};
