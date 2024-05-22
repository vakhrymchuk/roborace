#pragma once

#include "Engine.h"

class EnginePidConfig
{
public:
    EnginePidConfig()
    {
        Serial.begin(115200);
        Serial.println("Run engine 2second 100 cm/s and 2 second 200");
        Serial.print("Pid kp = ");
        Serial.print(engine.pid.Kp);
        Serial.print("\tkd=");
        Serial.print(engine.pid.Kd);
        Serial.print("\tki=");
        Serial.print(engine.pid.Ki);
        Serial.println();
    }

    void loop()
    {
        engine.run(speed);

        if (show.isReady())
        {
            Serial.print("Pid P = ");
            Serial.print(engine.pid.getP());
            Serial.print("\tD=");
            Serial.print(engine.pid.getD());
            Serial.print("\tI=");
            Serial.print(engine.pid.getI());
            Serial.print("\tspeed=");
            Serial.print(engine.getSpeed());
            Serial.println();
        }

        if (showConfig.isReady())
            showPidConfig();

        if (Serial.available())
        {
            String a = Serial.readString();
            a.trim();
            Serial.print("a=|");
            Serial.print(a);
            Serial.println("|");
            Serial.readString();
            if (a == "p")
            {
                mode = 'p';
                Serial.println("mode = p");
            }
            else if (a == "i")
            {
                mode = 'i';
                Serial.println("mode = i");
            }
            else if (a == "d")
            {
                mode = 'd';
                Serial.println("mode = d");
            }
            else if (a == "+")
            {
                Serial.println("UP");
                if (mode == 'p')
                    engine.pid.Kp *= 1.1;
                else if (mode == 'i')
                    engine.pid.Ki *= 1.1;
                else if (mode == 'd')
                    engine.pid.Kd *= 1.1;
                showPidConfig();
            }
            else if (a == "-")
            {
                Serial.println("DOWN");
                if (mode == 'p')
                    engine.pid.Kp *= 0.9;
                else if (mode == 'i')
                    engine.pid.Ki *= 0.9;
                else if (mode == 'd')
                    engine.pid.Kd *= 0.9;
                showPidConfig();
            }
            else
            {
                float k = a.toFloat();
                Serial.print("k=");
                Serial.println(k);
                if (k >= 0 && k <= 100)
                {
                    if (mode == 'p')
                        engine.pid.Kp = k;
                    else if (mode == 'i')
                        engine.pid.Ki = k;
                    else if (mode == 'd')
                        engine.pid.Kd = k;
                    else if (mode == 's')
                        speed = k;
                }

                showPidConfig();
            }
        }
    }

    void showPidConfig()
    {
        Serial.print("Pid kp = ");
        Serial.print(engine.pid.Kp);
        Serial.print("\tkd=");
        Serial.print(engine.pid.Kd);
        Serial.print("\tki=");
        Serial.print(engine.pid.Ki);
        Serial.println();
    }

private:
    Engine engine;

    Interval show = Interval(200);
    Interval showConfig = Interval(5000);

    char mode = 's';

    int speed = 100;
};
