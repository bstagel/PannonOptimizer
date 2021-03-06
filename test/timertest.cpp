#include <timertest.h>
#include <ctime>
#include <iostream>
#include <globals.h>
#include <debug.h>

using namespace std;

TimerTestSuite::TimerTestSuite(const char * name):
    UnitTest(name)
{
    ADD_TEST(TimerTestSuite::elapsedTime);
}

void TimerTestSuite::elapsedTime() {

    const double testTimes[] = {0.5, 1.0, 2.0, 5.0};
    const double tolerance = 0.05;
    const unsigned int count = STATIC_ARRAY_SIZE(testTimes);
    unsigned int index;
    Timer timer;
    double totalTime = 0;
    for (index = 0; index < count; index++) {
        const double lowerTime = (testTimes[index] - tolerance) * 1000000;
        const double upperTime = (testTimes[index] + tolerance) * 1000000;
        totalTime += testTimes[index];
        clock_t start, end, endPlusOne;
        LPINFO("Testing time interval: " << testTimes[index] << " sec");
        timer.start();
        start = clock();
        do {
            end = clock();
        } while (((end - start) / (double)CLOCKS_PER_SEC) < testTimes[index] );
        endPlusOne = clock();
        timer.stop();
        end = (end + endPlusOne) / 2;

        TEST_ASSERT( timer.getLastElapsed() >= lowerTime );
        TEST_ASSERT( timer.getLastElapsed() <= upperTime );
    }
    const double totalTolerance = 0.05;
    const double lowerTime = (totalTime - totalTolerance) * 1000000;
    const double upperTime = (totalTime + totalTolerance) * 1000000;

    TEST_ASSERT( timer.getTotalElapsed() >= lowerTime );
    TEST_ASSERT( timer.getTotalElapsed() <= upperTime );
}
