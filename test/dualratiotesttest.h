#ifndef DUALRATIOTEST_H
#define DUALRATIOTEST_H

#include <framework/tester.h>
#include <framework/unittest.h>
#include <lp/model.h>
#include <simplex/dualratiotest.h>


class DualRatiotestTestSuite: public UnitTest{
public:
    DualRatiotestTestSuite(const char * name);
private:
    void initVariable(Model& model,
                      Numerical::Double lb,
                      Numerical::Double ub);
    void  initTestDefines();

    void testStagelPh1();
    void testMaros1Ph1();
    void testMaros2Ph1();
    void testStagelCf3Ph1();

    void testStagelPh2();
    void testMaros1Ph2();
    void testMaros2Ph2();
    void testStagelCf3Ph2();
};

#endif // DUALRATIOTEST_H
