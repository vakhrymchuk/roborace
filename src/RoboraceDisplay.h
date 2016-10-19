#ifndef ROBORACE_DISPLAY_H
#define ROBORACE_DISPLAY_H

#include <Adafruit_SSD1306.h>
#include <Joystick.h>
#include "Roborace.h"
#include "ValueEditor.h"

class RoboraceDisplay : public Roborace {

    static const int DISPLAY_LINES_INTERVAL = 200;
    static const int DISPLAY_RESET_PIN = 4;
    static const int THRESHOLD = 40;

    static const int PARAM_CHANGE_TIMEOUT = 300;
    static const int VALUE_CHANGE_TIMEOUT = 50;

public:
    RoboraceDisplay();

    virtual void loop() override final;

private:

    ButtonPullUp *displayButton;
    Interval *displayOut;
    Adafruit_SSD1306 *display;

    ButtonPullUp *configButton;
    IntervalValue *configIntervalOut;
    ValueEditor *valueEditor;

    Joystick *joystick;
    TimeoutValue *paramChangeTimeout;
    TimeoutValue *valueChangeTimeout;

    void initDisplay();

    void displayDistancesLines();

    void processConfig();

    void displayCurrent(const ValueBase *value) const;
};

RoboraceDisplay::RoboraceDisplay() : Roborace() {

    initDisplay();
    displayButton = new ButtonPullUp(DISPLAY_PIN);
    displayOut = new Interval(RoboraceDisplay::DISPLAY_LINES_INTERVAL);

    configButton = new ButtonPullUp(CONFIG_PIN);

    configIntervalOut = new IntervalValue(new ValueInt(20));
    valueEditor = new ValueEditor();

    joystick = new Joystick(A7, A7, 7);

    paramChangeTimeout = new TimeoutValue(new ValueInt(PARAM_CHANGE_TIMEOUT));
    valueChangeTimeout = new TimeoutValue(new ValueInt(VALUE_CHANGE_TIMEOUT));

    valueEditor->add(turnMaxAngle, "TURN MAX  ANGLE");
    valueEditor->add(forwardSpeed, "BACKWARD  SPEED");
    valueEditor->add(backwardSpeed, "FORWARD   SPEED");

    valueEditor->add(mainLoopChange->getValueInt(), "RUN INTERVAL");

    valueEditor->add(engine->avg->correctionFactor, "ENGINE COR FACTOR");
    valueEditor->add(engine->avg->maxCorrection, "MAX ENGINECORRECTION");

    valueEditor->add(configIntervalOut->getValueInt(), "CONFIG    INTERVAL");

    valueEditor->add(paramChangeTimeout->getValueInt(), "PARAM     CHANGE MS");
    valueEditor->add(valueChangeTimeout->getValueInt(), "VALUE     CHANGE MS");

    valueEditor->add((ValueBase *) kalmanFactorValue, "KALMAN    FACTOR");

}

void RoboraceDisplay::loop() {

    Roborace::loop();

    if (configIntervalOut->isReady() && configButton->read()) {
        processConfig();
    } else if (displayOut->isReady() && displayButton->read()) {
        displayDistancesLines();
    }
}


void RoboraceDisplay::initDisplay() {
    display = new Adafruit_SSD1306(DISPLAY_RESET_PIN);
    display->begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display->clearDisplay();
    display->setTextSize(2);
    display->setTextColor(WHITE);
    display->print("start");
    display->display();
    delay(200);
}

void RoboraceDisplay::displayDistancesLines() {
    short W_MID = SSD1306_LCDWIDTH >> 1;
    short L_HALF = 10;
    short L_FULL = 20;

    display->clearDisplay();

    short x, y;
    y = mapType<short>(sensorsHolder->forwardDistance, Sharp::MIN_DISTANCE, Sharp::MAX_DISTANCE, SSD1306_LCDHEIGHT - 30,
                       0);
    display->drawFastHLine(W_MID - L_HALF, y, L_FULL, WHITE);
    display->drawFastHLine(W_MID - L_HALF, y + 1, L_FULL, WHITE);

    y = mapType<short>(sensorsHolder->rightDistance, Sharp::MIN_DISTANCE, Sharp::MAX_DISTANCE, SSD1306_LCDHEIGHT - 30,
                       0);
    display->drawLine(W_MID + L_HALF, y, W_MID + 3 * L_HALF, y + L_HALF, WHITE);
    ++y;
    display->drawLine(W_MID + L_HALF, y, W_MID + 3 * L_HALF, y + L_HALF, WHITE);

    y = mapType<short>(sensorsHolder->leftDistance, Sharp::MIN_DISTANCE, Sharp::MAX_DISTANCE, SSD1306_LCDHEIGHT - 30,
                       0);
    display->drawLine(W_MID - 3 * L_HALF, y + L_HALF, W_MID - L_HALF, y, WHITE);
    ++y;
    display->drawLine(W_MID - 3 * L_HALF, y + L_HALF, W_MID - L_HALF, y, WHITE);
//    display->fillTriangle(34, (byte) (y + 10), 54, y, 34, 0, WHITE);

    x = mapType<short>(sensorsHolder->leftTurnDistance,
                       SharpMini::MIN_DISTANCE, SharpMini::MAX_DISTANCE, 44, 0);
    display->drawLine(x, SSD1306_LCDHEIGHT - 1, x + L_HALF, SSD1306_LCDHEIGHT - L_FULL, WHITE);
    display->setCursor(x + L_HALF, SSD1306_LCDHEIGHT - L_FULL);
    display->print(sensorsHolder->leftTurnDistance);

    x = mapType<short>(sensorsHolder->rightTurnDistance,
                       SharpMini::MIN_DISTANCE, SharpMini::MAX_DISTANCE, 84, SSD1306_LCDWIDTH - 1);
    display->drawLine(x, SSD1306_LCDHEIGHT - 1, x - L_HALF, SSD1306_LCDHEIGHT - L_FULL, WHITE);

    display->setCursor(x - L_HALF, SSD1306_LCDHEIGHT - L_FULL);
    display->print(sensorsHolder->rightTurnDistance);


    if (sensorsHolder->rightClashDistance < SharpMicro::MAX_DISTANCE) {
        y = mapType<short>(sensorsHolder->rightClashDistance,
                           SharpMicro::MIN_DISTANCE, SharpMicro::MAX_DISTANCE, SSD1306_LCDHEIGHT - 10, 10);
        display->drawFastHLine(W_MID, y, L_FULL, WHITE);
        display->setCursor(W_MID, y);
        display->print(sensorsHolder->rightClashDistance);
    }
    if (sensorsHolder->leftClashDistance < SharpMicro::MAX_DISTANCE) {
        y = mapType<short>(sensorsHolder->leftClashDistance,
                           SharpMicro::MIN_DISTANCE, SharpMicro::MAX_DISTANCE, SSD1306_LCDHEIGHT - 10, 10);
        display->drawFastHLine(W_MID - L_FULL, y, L_FULL, WHITE);
        display->setCursor(W_MID - L_FULL, y);
        display->print(sensorsHolder->leftClashDistance);
    }


    double angleRad = (constrain(angle, -60, 60) + 90) * DEG_TO_RAD;
    x = (short) (64 * cos(angleRad));
    y = (short) (64 * sin(angleRad));

    display->drawLine(W_MID, SSD1306_LCDHEIGHT - 1, W_MID + x, SSD1306_LCDHEIGHT - y, WHITE);


/*
    display->setTextSize(2);
    display->setCursor(1, 1);
    display->print(herc);

    display->setCursor(40, 1);
    display->print(angle);

    display->setCursor(90, 1);
    display->print(power);

    display->setTextSize(2);
    display->setCursor(4, 27);
    display->print(leftDistance);
    display->setCursor(44, 27);
    display->print(forwardDistance);
    display->setCursor(84, 27);
    display->print(rightDistance);

    display->setTextSize(2);
    display->setCursor(4, 50);
    display->print(leftTurnDistance);
    display->setCursor(34, 45);
    display->print(leftClashDistance);
    display->setCursor(64, 45);
    display->print(rightClashDistance);
    display->setCursor(94, 50);
    display->print(rightTurnDistance);
*/

    switch (mode) {
        case FORWARD: {
            break;
        }
        case BACKWARD: {
            display->drawFastHLine(0, 0, SSD1306_LCDWIDTH, WHITE);
            break;
        }
        case ANGLE: {
            if (sensorsHolder->rightDistance > sensorsHolder->leftDistance) {
                display->drawFastVLine(SSD1306_LCDWIDTH - 1, 0, SSD1306_LCDHEIGHT, WHITE);
            } else {
                display->drawFastVLine(0, 0, SSD1306_LCDHEIGHT, WHITE);
            }
            break;
        }
    }

    display->display();
}

void RoboraceDisplay::processConfig() {

    if (paramChangeTimeout->isReady()) {

        if (joystick->readButton()) {
            valueEditor->getNext();
            paramChangeTimeout->start();
//            valueChangeTimeout->start(paramChangeTimeout->getValueInt()->value);
        }
/*
        int y = joystick->readY();
        if (y < ANALOG_RESOLUTION_HALF - THRESHOLD) {
            valueEditor->prev();
            paramChangeTimeout->start();
            valueChangeTimeout->start(paramChangeTimeout->getValueInt()->value);
        } else if (y > ANALOG_RESOLUTION_HALF + THRESHOLD) {
            valueEditor->next();
            paramChangeTimeout->start();
            valueChangeTimeout->start(paramChangeTimeout->getValueInt()->value);
        }
*/

    }
    if (valueChangeTimeout->isReady()) {
        int x = joystick->readX();
        if (x < ANALOG_RESOLUTION_HALF - THRESHOLD) {
            int size = mapType(x, 0, ANALOG_RESOLUTION_HALF - THRESHOLD, 4, 1);
            valueEditor->getCurrent()->dec(size);
            valueChangeTimeout->start();
        } else if (x > ANALOG_RESOLUTION_HALF + THRESHOLD) {
            int size = mapType(x, ANALOG_RESOLUTION_HALF + THRESHOLD, ANALOG_RESOLUTION_BASE, 1, 4);
            valueEditor->getCurrent()->inc(size);
            valueChangeTimeout->start();
        }
    }

    displayCurrent(valueEditor->getCurrent());
}

void RoboraceDisplay::displayCurrent(const ValueBase *value) const {
    display->clearDisplay();
    display->setCursor(0, 0);
    display->setTextSize(2);
    display->println(value->name);
    display->setTextSize(3);
    value->printValue(display);
    display->display();
}

#endif
