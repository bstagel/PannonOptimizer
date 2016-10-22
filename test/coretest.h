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

    void denseToDenseAdd();
    void denseToDenseAddPerformance();




    void generateAddVectorInput(double * a,
                                double * b,
                                unsigned int count);
};

#endif // CORETEST_H
