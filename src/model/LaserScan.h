#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <map>

class LaserScan
{
public:
    std::map<int, int> data;

private:
public:
    int findDistanceAtDegree(int degree)
    {
        int d = 5000;
        int count = 0;
        int inc = 0;
        int c = getDataAtDegree(degree);
        if (c > 0)
        {
            d = min(d, c);
            count++;
        }
        while (count < 8 && inc++ < 10)
        {
            c = getDataAtDegree(degree - inc);
            if (c > 0)
            {
                d = min(d, c);
                count++;
            }
            c = getDataAtDegree(degree + inc);
            if (c > 0)
            {
                d = min(d, c);
                count++;
            }
        }
        if (d == 5000)
            return 0;
        return constrain(d, 0, 500);
    }

    int getDataAtDegree(int degree)
    {
        if (data.count(degree))
            return data[degree];
        return 0;
    }

private:
};
