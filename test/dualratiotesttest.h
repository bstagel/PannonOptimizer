#ifndef DUALRATIOTESTTEST_H
#define DUALRATIOTESTTEST_H

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
//phase 1 tests
    void testStagelPh1();
    void testMaros1Ph1();
    void testMaros2Ph1();
    void testStagelCf3Ph1();

    void testNum1Ph1();
    void testNum2Ph1();
    void testNum3Ph1();
    void testNum4Ph1();
//phase 2 tests
    void testStagelPh2();
    void testMaros1Ph2();
    void testMaros2Ph2();
    void testStagelCf3Ph2();
};

#endif // DUALRATIOTESTTEST_H
