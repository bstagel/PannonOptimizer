#ifndef SIMPLEXPARAMETERHANDLERTEST_H
#define SIMPLEXPARAMETERHANDLERTEST_H

#include <framework/tester.h>
#include <framework/unittest.h>
#include <simplex/simplexparameterhandler.h>

class SimplexParameterHandlerTestSuite : public UnitTest
{
public:

    SimplexParameterHandlerTestSuite(const char * name);

private:
    void getInstance();
};

#endif // SIMPLEXPARAMETERHANDLERTEST_H
