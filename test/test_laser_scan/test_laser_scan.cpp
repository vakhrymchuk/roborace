#include <map>
#include <unity.h>

#ifndef ARDUINO
#define min(a, b) ((a < b) ? a : b)
#define constrain(a, b, c) ((a < b) ? b : ((a > c) ? c : a))
#endif

#include "model/LaserScan.h"

void test_laser_scan(void)
{
    LaserScan scan;
    scan.data[21] = 444;
    scan.data[25] = 350;
    TEST_ASSERT_EQUAL(2, scan.data.size());
    TEST_ASSERT_EQUAL(444, scan.getDataAtDegree(21));
    TEST_ASSERT_EQUAL(350, scan.getDataAtDegree(25));
    TEST_ASSERT_EQUAL(0, scan.getDataAtDegree(2));
    TEST_ASSERT_EQUAL(350, scan.findDistanceAtDegree(30));
    TEST_ASSERT_EQUAL(350, scan.findDistanceAtDegree(15));
    TEST_ASSERT_EQUAL(444, scan.findDistanceAtDegree(12));
    TEST_ASSERT_EQUAL(0, scan.findDistanceAtDegree(0));
    TEST_ASSERT_EQUAL(2, scan.data.size());
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_laser_scan);
    UNITY_END();
}
