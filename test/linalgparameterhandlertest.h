#ifndef LINALGPARAMETERHANDLERTEST_H
#define LINALGPARAMETERHANDLERTEST_H

#include <framework/tester.h>
#include <framework/unittest.h>

#include <linalg/linalgparameterhandler.h>

class LinalgParameterHandlerTestSuite : public UnitTest
{
public:

    LinalgParameterHandlerTestSuite(const char * name);

private:
    void getInstance();
};

#endif // LINALGPARAMETERHANDLERTEST_H
