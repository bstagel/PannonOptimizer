#ifndef CORETEST_H
#define CORETEST_H

#include <framework/tester.h>
#include <framework/unittest.h>

class CoreTestSuite : public UnitTest {
public:
    CoreTestSuite(const char * name);

private:
    void memcpy();
    void memset();

    void denseToDenseDotProduct();
    void denseToSparseDotProduct();
};

#endif // CORETEST_H
