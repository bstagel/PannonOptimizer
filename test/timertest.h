#ifndef TIMERTESTSUITE_H
#define TIMERTESTSUITE_H

#include "framework/tester.h"
#include "framework/unittest.h"
#include <utils/timer.h>

class TimerTestSuite : public UnitTest
{
public:
    TimerTestSuite(const char * name);
private:
    void elapsedTime();
};

#endif // TIMERTESTSUITE_H
