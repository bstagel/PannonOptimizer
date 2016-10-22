#ifndef PRESOLVERTEST_H
#define PRESOLVERTEST_H

#include <sstream>
#include <framework/tester.h>
#include <framework/unittest.h>
#include <lp/presolver.h>

class PresolverTestSuite : public UnitTest {
public:
    PresolverTestSuite(const char * name);

private:

    void init();
    void singletonRows();
};

#endif // PRESOLVERTEST_H
