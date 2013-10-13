#include <cstdlib>
#include <cstring>
#include <iostream>

#ifdef _MSC_VER
#pragma warning (disable: 4290)
#endif

#include <framework/tester.h>
#include <framework/unittest.h>
#include <variabletest.h>
#include <constrainttest.h>
#include <matrixtest.h>
#include <manualmodelbuildertest.h>
#include <vectortest.h>
#include <indexlisttest.h>
#include <modeltest.h>
#include <mpsproblemtest.h>
#include <timertest.h>
#include <outputhandlertest.h>
#include <linalgparameterhandlertest.h>
#include <simplexparameterhandlertest.h>

using namespace std;

// Main test program
//

int main(int, char** )
{
    setbuf(stdout, 0);
    Tester::addUnitTest(new VariableTestSuite("Variable Test"));
    Tester::addUnitTest(new ConstraintTestSuite("Constraint Test"));
//    Tester::addUnitTest(new IndexListTestSuite("IndexList Test"));
    Tester::addUnitTest(new VectorTestSuite("Vector Test"));
    Tester::addUnitTest(new MatrixTestSuite("Matrix Test"));
//    Tester::addUnitTest(new ManualModelBuilderTestSuite("ManualModelBuilder Test"));
//    Tester::addUnitTest(new ModelTestSuite("Model Test"));

//    Tester::addUnitTest(new MpsProblemTestSuite("MpsProblem Test"));
    Tester::addUnitTest(new OutputHandlerTestSuite("Output Handler Test"));
//    Tester::addUnitTest(new TimerTestSuite("Timer Test"));


    Tester::addUnitTest(new LinalgParameterHandlerTestSuite("Linalg Parameter Handler Test"));
    Tester::addUnitTest(new SimplexParameterHandlerTestSuite("Simplex Parameter Handler Test"));

    Tester::run();
    return EXIT_SUCCESS;
}

