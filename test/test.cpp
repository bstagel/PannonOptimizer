#include <cstdlib>
#include <cstring>
#include <iostream>

#ifdef _MSC_VER
#pragma warning (disable: 4290)
#endif

#include "framework/tester.h"
#include "framework/unittest.h"
#include "variabletest.h"
#include "constrainttest.h"
#include "matrixtest.h"
#include "manualmodelbuildertest.h"
#include "vectortest.h"
#include "indexlisttest.h"
#include "modeltest.h"
#include "mpsproblemtest.h"

using namespace std;

// Main test program
//

int main(int, char** )
{
    /*Tester::addUnitTest(new VariableTestSuite("Variable Test"));
    Tester::addUnitTest(new ConstraintTestSuite("Constraint Test"));
    Tester::addUnitTest(new VectorTestSuite("Vector test"));
    Tester::addUnitTest(new MatrixTestSuite("Matrix Test"));
    Tester::addUnitTest(new ManualModelBuilderTestSuite("ManualModelBuilder Test"));
    Tester::addUnitTest(new ModelTestSuite("Model Test"));*/
    //Tester::addUnitTest(new IndexListTestSuite("IndexList Test"));
    Tester::addUnitTest(new MpsProblemTestSuite("MpsProblem Test"));
    Tester::run();
    return EXIT_SUCCESS;
}

