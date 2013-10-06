#ifndef OUTPUTHANDLERTEST_H
#define OUTPUTHANDLERTEST_H

#include "framework/tester.h"
#include "framework/unittest.h"
#include <debug.h>
#include <utils/outputhandler.h>

class OutputHandlerTestSuite: public UnitTest
{
public:
    OutputHandlerTestSuite(const char * name);

private:
    void init();
    void change();
};

#endif // OUTPUTHANDLERTEST_H
