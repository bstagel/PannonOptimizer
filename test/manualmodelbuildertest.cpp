#include "manualmodelbuildertest.h"
#include <algorithm>

void ManualModelBuilderTestSuite::init()
{
    ManualModelBuilder builder;
    TEST_ASSERT(builder.m_objectiveConstant == 0.0);
    TEST_ASSERT(builder.getObjectiveConstant() == 0.0);

    TEST_ASSERT(builder.m_name == "");
    TEST_ASSERT(builder.getName() == "");

    TEST_ASSERT(builder.m_columns.size() == 0);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 0);
    TEST_ASSERT(builder.m_rows.size() == 0);
    TEST_ASSERT(builder.m_nonZerosInRows.size() == 0);
    TEST_ASSERT(builder.m_constraints.size() == 0);
    TEST_ASSERT(builder.m_variables.size() == 0);
    TEST_ASSERT(builder.m_costVector.size() == 0);
}

void ManualModelBuilderTestSuite::setGetName()
{
    const std::string testName = "Demo Model";
    ManualModelBuilder builder;
    builder.setName(testName);
    TEST_ASSERT(builder.m_name == testName);
    TEST_ASSERT(builder.getName() == testName);
}

void ManualModelBuilderTestSuite::setGetObjectiveFunctionConstant()
{
    const Numerical::Double testConstant = 12.4;
    ManualModelBuilder builder;
    builder.setObjectiveFunctionConstant(testConstant);
    TEST_ASSERT(builder.m_objectiveConstant == testConstant);
    TEST_ASSERT(builder.getObjectiveConstant() == testConstant);
    TEST_ASSERT(builder.m_variables.size() == 0);
    TEST_ASSERT(builder.m_constraints.size() == 0);
    TEST_ASSERT(builder.m_costVector.size() == 0);
    TEST_ASSERT(builder.m_columns.size() == 0);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 0);
    TEST_ASSERT(builder.m_rows.size() == 0);
    TEST_ASSERT(builder.m_nonZerosInRows.size() == 0);
}

void ManualModelBuilderTestSuite::addVariable1()
{
    /*******************************************************
     * First variable
     ******************************************************/

    ManualModelBuilder builder;
    Variable testVariable1 = Variable::createMinusTypeVariable("x1", 0.0, 10.0);
    const Numerical::Double testValues1[] = {1.0, -2.3, 0.13};
    const unsigned int testIndices1[] = {7, 3, 10};
    const unsigned int testNonZeros1 = 3;
    const Numerical::Double testCostCoefficient1 = 4.2;

    builder.addVariable(testVariable1, testCostCoefficient1,
        testNonZeros1, testValues1, testIndices1);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 4.2 ]
     * m_variables = [ variable1 ]
     * m_nonZerosInColumns : [ 3 ]
     * m_columns = 
     *        [  1.0  ; 7  ]
     *        [ -2.3  ; 3  ]
     *        [  0.13 ; 10 ]
     * 
     * m_constraints :    [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
     * m_nonZerosInRows : [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1  ]
     * m_rows = 
     * 0.:  [ ]
     * 1.:  [ ] 
     * 2.:  [ ]
     * 3.:  [ -2.3; 0 ]
     * 4.:  [ ]
     * 5.:  [ ]
     * 6.:  [ ]
     * 7.:  [ 1.0; 0 ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.: [ 0.13; 0]
     */

    const Numerical::Double outputObjectiveConstant1 = 0;
    const std::string outputName1 = "";
    const Numerical::Double outputCostVector1[] = {4.2};
    const Variable outputVariables1[] = {testVariable1};
    const unsigned int outputNonZerosInColumns1[] = {3};

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists1(1);
    outputColumnLists1[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputColumnLists1[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputColumnLists1[0].push_back(ManualModelBuilder::createPair(0.13, 10));

    const unsigned int outputNonZerosInRows1[] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1};
    const std::vector<Constraint> outputConstrains1(11,
        Constraint::createGreaterTypeConstraint(0, 0.0));

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows1(11);

    ManualModelBuilder::IndexValuePair testPair;
    testPair = ManualModelBuilder::createPair(-2.3, 0);
    outputRows1[3].push_back(testPair);
    testPair = ManualModelBuilder::createPair(1.0, 0);
    outputRows1[7].push_back(testPair);
    testPair = ManualModelBuilder::createPair(0.13, 0);
    outputRows1[10].push_back(testPair);

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant1);
    TEST_ASSERT(builder.m_name == outputName1);
    TEST_ASSERT(builder.m_costVector.size() == 1);
    TEST_ASSERT(std::equal(
        builder.m_costVector.begin(),
        builder.m_costVector.end(),
        outputCostVector1) == true);
    TEST_ASSERT(builder.m_variables.size() == 1);
    TEST_ASSERT(std::equal(
        builder.m_variables.begin(),
        builder.m_variables.end(),
        outputVariables1) == true);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 1);
    TEST_ASSERT(std::equal(
        builder.m_nonZerosInColumns.begin(),
        builder.m_nonZerosInColumns.end(),
        outputNonZerosInColumns1) == true);
    TEST_ASSERT(builder.m_columns == outputColumnLists1);

    TEST_ASSERT(std::equal(
        builder.m_nonZerosInRows.begin(),
        builder.m_nonZerosInRows.end(),
        outputNonZerosInRows1) == true);
    TEST_ASSERT(builder.m_constraints == outputConstrains1);
    TEST_ASSERT(builder.m_rows == outputRows1);

    /*******************************************************
     * Second variable
     ******************************************************/

    Variable testVariable2 = Variable::createPlusTypeVariable("x2", 1.3, -22.4);
    const Numerical::Double testValues2[] = {4.2, 1.03, 4.31, 12.1};
    const unsigned int testIndices2[] = {1, 5, 7, 12};
    const unsigned int testNonZeros2 = 4;
    const Numerical::Double testCostCoefficient2 = -0.2;

    builder.addVariable(testVariable2, testCostCoefficient2,
        testNonZeros2, testValues2, testIndices2);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 4.2, -0.2 ]
     * m_variables = [ variable1, variable2 ]
     * m_nonZerosInColumns : [ 3, 4 ]
     * m_columns = 
     *        [  1.0  ; 7  ]  [  4.2  ; 1  ]
     *        [ -2.3  ; 3  ]  [  1.03 ; 5  ]
     *        [  0.13 ; 10 ]  [  4.31 ; 7  ]
     *                        [  12.1 ; 12 ]
     * 
     * m_constraints :    [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInRows : [ 0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  0,  1  ]
     * m_rows = 
     * 0.:  [ ]
     * 1.:  [ 4.2; 1 ] 
     * 2.:  [ ]
     * 3.:  [ -2.3; 0 ]
     * 4.:  [ ]
     * 5.:  [ 1.03; 1 ]
     * 6.:  [ ]
     * 7.:  [ 1.0; 0 ] [ 4.31; 1 ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.: [ 0.13; 0]
     * 11.: [ ]
     * 12.: [ 12.1; 1]
     */

    const Numerical::Double outputObjectiveConstant2 = 0;
    const std::string outputName2 = "";
    const Numerical::Double outputCostVector2[] = {4.2, -0.2};
    const Variable outputVariables2[] = {testVariable1, testVariable2};
    const unsigned int outputNonZerosInColumns2[] = {3, 4};

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists2(2);
    outputColumnLists2[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputColumnLists2[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputColumnLists2[0].push_back(ManualModelBuilder::createPair(0.13, 10));

    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(1.03, 5));
    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(4.31, 7));
    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(12.1, 12));

    const unsigned int outputNonZerosInRows2[] = {0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1, 0, 1};
    const std::vector<Constraint> outputConstrains2(13,
        Constraint::createGreaterTypeConstraint(0, 0.0));

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows2(13);
    testPair = ManualModelBuilder::createPair(4.2, 1);
    outputRows2[1].push_back(testPair);
    testPair = ManualModelBuilder::createPair(-2.3, 0);
    outputRows2[3].push_back(testPair);
    testPair = ManualModelBuilder::createPair(1.03, 1);
    outputRows2[5].push_back(testPair);
    testPair = ManualModelBuilder::createPair(1.0, 0);
    outputRows2[7].push_back(testPair);
    testPair = ManualModelBuilder::createPair(4.31, 1);
    outputRows2[7].push_back(testPair);
    testPair = ManualModelBuilder::createPair(0.13, 0);
    outputRows2[10].push_back(testPair);
    testPair = ManualModelBuilder::createPair(12.1, 1);
    outputRows2[12].push_back(testPair);

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant2);
    TEST_ASSERT(builder.m_name == outputName2);
    TEST_ASSERT(builder.m_costVector.size() == 2);
    TEST_ASSERT(std::equal(
        builder.m_costVector.begin(),
        builder.m_costVector.end(),
        outputCostVector2) == true);
    TEST_ASSERT(builder.m_variables.size() == 2);
    TEST_ASSERT(std::equal(
        builder.m_variables.begin(),
        builder.m_variables.end(),
        outputVariables2) == true);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 2);
    TEST_ASSERT(std::equal(
        builder.m_nonZerosInColumns.begin(),
        builder.m_nonZerosInColumns.end(),
        outputNonZerosInColumns2) == true);
    TEST_ASSERT(builder.m_columns == outputColumnLists2);

    TEST_ASSERT(std::equal(
        builder.m_nonZerosInRows.begin(),
        builder.m_nonZerosInRows.end(),
        outputNonZerosInRows2) == true);
    TEST_ASSERT(builder.m_constraints == outputConstrains2);
    TEST_ASSERT(builder.m_rows == outputRows2);

    /*******************************************************
     * Third variable, empty column
     ******************************************************/

    Variable testVariable3 = Variable::createFreeTypeVariable("x3", 4.0);
    const unsigned int testNonZeros3 = 0;
    const Numerical::Double testCostCoefficient3 = 3.0;

    builder.addVariable(testVariable3, testCostCoefficient3,
        testNonZeros3, 0, 0);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 4.2, -0.2, 3.0 ]
     * m_variables = [ variable1, variable2, variable3 ]
     * m_nonZerosInColumns : [ 3, 4, 0 ]
     * m_columns = 
     *        [  1.0  ; 7  ]  [  4.2  ; 1  ]  [  ]
     *        [ -2.3  ; 3  ]  [  1.03 ; 5  ]
     *        [  0.13 ; 10 ]  [  4.31 ; 7  ]
     *                        [  12.1 ; 12 ]
     * 
     * m_constraints :    [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInRows : [ 0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  0,  1  ]
     * m_rows = 
     * 0.:  [ ]
     * 1.:  [ 4.2; 1 ] 
     * 2.:  [ ]
     * 3.:  [ -2.3; 0 ]
     * 4.:  [ ]
     * 5.:  [ 1.03; 1 ]
     * 6.:  [ ]
     * 7.:  [ 1.0; 0 ] [ 4.31; 1 ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.: [ 0.13; 0]
     * 11.: [ ]
     * 12.: [ 12.1; 1]
     */

    const Numerical::Double outputObjectiveConstant3 = 0;
    const std::string outputName3 = "";
    const Numerical::Double outputCostVector3[] = {4.2, -0.2, 3.0};
    const Variable outputVariables3[] = {testVariable1, testVariable2, testVariable3};
    const unsigned int outputNonZerosInColumns3[] = {3, 4, 0};

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists3 =
        outputColumnLists2;
    outputColumnLists3.resize(3);

    const unsigned int outputNonZerosInRows3[] = {0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1, 0, 1};
    const std::vector<Constraint> outputConstrains3(13,
        Constraint::createGreaterTypeConstraint(0, 0.0));

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows3 =
        outputRows2;

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant3);
    TEST_ASSERT(builder.m_name == outputName3);
    TEST_ASSERT(builder.m_costVector.size() == 3);
    TEST_ASSERT(std::equal(
        builder.m_costVector.begin(),
        builder.m_costVector.end(),
        outputCostVector3) == true);
    TEST_ASSERT(builder.m_variables.size() == 3);
    TEST_ASSERT(std::equal(
        builder.m_variables.begin(),
        builder.m_variables.end(),
        outputVariables3) == true);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 3);
    TEST_ASSERT(std::equal(
        builder.m_nonZerosInColumns.begin(),
        builder.m_nonZerosInColumns.end(),
        outputNonZerosInColumns3) == true);
    TEST_ASSERT(builder.m_columns == outputColumnLists3);

    TEST_ASSERT(std::equal(
        builder.m_nonZerosInRows.begin(),
        builder.m_nonZerosInRows.end(),
        outputNonZerosInRows3) == true);
    TEST_ASSERT(builder.m_constraints == outputConstrains3);
    TEST_ASSERT(builder.m_rows == outputRows3);

    /*******************************************************
     * Fourth variable
     ******************************************************/

    Variable testVariable4 = Variable::createBoundedTypeVariable("x4", 1.1, -0.3, 4.3);
    const Numerical::Double testValues4[] = {0.02, 31.02};
    const unsigned int testIndices4[] = {0, 11};
    const unsigned int testNonZeros4 = 2;
    const Numerical::Double testCostCoefficient4 = 0.0;

    builder.addVariable(testVariable4, testCostCoefficient4,
        testNonZeros4, testValues4, testIndices4);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 4.2, -0.2, 3.0, 0.0 ]
     * m_variables = [ variable1, variable2, variable3, variable4 ]
     * m_nonZerosInColumns : [ 3, 4, 0, 2 ]
     * m_columns = 
     *        [  1.0  ; 7  ]  [  4.2  ; 1  ]  [  ]  [  0.02  ; 0  ]
     *        [ -2.3  ; 3  ]  [  1.03 ; 5  ]        [  31.02 ; 11 ]
     *        [  0.13 ; 10 ]  [  4.31 ; 7  ]
     *                        [  12.1 ; 12 ]
     * 
     * m_constraints :    [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInRows : [ 1, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  1,  1  ]
     * m_rows = 
     * 0.:  [ 0.02; 3]
     * 1.:  [ 4.2; 1 ] 
     * 2.:  [ ]
     * 3.:  [ -2.3; 0 ]
     * 4.:  [ ]
     * 5.:  [ 1.03; 1 ]
     * 6.:  [ ]
     * 7.:  [ 1.0; 0 ] [ 4.31; 1 ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.: [ 0.13; 0]
     * 11.: [ 31.02; 3]
     * 12.: [ 12.1; 1]
     */

    const Numerical::Double outputObjectiveConstant4 = 0;
    const std::string outputName4 = "";
    const Numerical::Double outputCostVector4[] = {4.2, -0.2, 3.0, 0.0};
    const Variable outputVariables4[] = {testVariable1, testVariable2,
        testVariable3, testVariable4};
    const unsigned int outputNonZerosInColumns4[] = {3, 4, 0, 2};

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists4 =
        outputColumnLists3;
    outputColumnLists4.resize(4);
    outputColumnLists4[3].push_back(ManualModelBuilder::createPair(0.02, 0));
    outputColumnLists4[3].push_back(ManualModelBuilder::createPair(31.02, 11));

    const unsigned int outputNonZerosInRows4[] = {1, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1, 1, 1};
    const std::vector<Constraint> outputConstrains4(13,
        Constraint::createGreaterTypeConstraint(0, 0.0));

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows4 =
        outputRows3;
    outputRows4[0].push_back(ManualModelBuilder::createPair(0.02, 3));
    outputRows4[11].push_back(ManualModelBuilder::createPair(31.02, 3));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant4);
    TEST_ASSERT(builder.m_name == outputName4);
    TEST_ASSERT(builder.m_costVector.size() == 4);
    TEST_ASSERT(std::equal(
        builder.m_costVector.begin(),
        builder.m_costVector.end(),
        outputCostVector4) == true);
    TEST_ASSERT(builder.m_variables.size() == 4);
    TEST_ASSERT(std::equal(
        builder.m_variables.begin(),
        builder.m_variables.end(),
        outputVariables4) == true);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 4);
    TEST_ASSERT(std::equal(
        builder.m_nonZerosInColumns.begin(),
        builder.m_nonZerosInColumns.end(),
        outputNonZerosInColumns4) == true);
    TEST_ASSERT(builder.m_columns == outputColumnLists4);

    TEST_ASSERT(std::equal(
        builder.m_nonZerosInRows.begin(),
        builder.m_nonZerosInRows.end(),
        outputNonZerosInRows4) == true);
    TEST_ASSERT(builder.m_constraints == outputConstrains4);
    TEST_ASSERT(builder.m_rows == outputRows4);

}

void ManualModelBuilderTestSuite::addVariable2()
{
    /*******************************************************
     * First variable
     ******************************************************/

    ManualModelBuilder builder;
    Variable testVariable1 = Variable::createMinusTypeVariable("x1", 0.0, 10.0);
    const Numerical::Double testCostCoefficient1 = 4.2;
    Vector testVector1(11);
    testVector1.set(7, 1.0);
    testVector1.set(3, -2.3);
    testVector1.set(10, 0.13);

    builder.addVariable(testVariable1, testCostCoefficient1, testVector1);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 4.2 ]
     * m_variables = [ variable1 ]
     * m_nonZerosInColumns : [ 3 ]
     * m_columns = 
     *        [  1.0  ; 7  ]
     *        [ -2.3  ; 3  ]
     *        [  0.13 ; 10 ]
     * 
     * m_constraints :    [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
     * m_nonZerosInRows : [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1  ]
     * m_rows = 
     * 0.:  [ ]
     * 1.:  [ ] 
     * 2.:  [ ]
     * 3.:  [ -2.3; 0 ]
     * 4.:  [ ]
     * 5.:  [ ]
     * 6.:  [ ]
     * 7.:  [ 1.0; 0 ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.: [ 0.13; 0]
     */

    const Numerical::Double outputObjectiveConstant1 = 0;
    const std::string outputName1 = "";
    const Numerical::Double outputCostVector1[] = {4.2};
    const Variable outputVariables1[] = {testVariable1};
    const unsigned int outputNonZerosInColumns1[] = {3};

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists1(1);
    outputColumnLists1[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputColumnLists1[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputColumnLists1[0].push_back(ManualModelBuilder::createPair(0.13, 10));

    const unsigned int outputNonZerosInRows1[] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1};
    const std::vector<Constraint> outputConstrains1(11,
        Constraint::createGreaterTypeConstraint(0, 0.0));

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows1(11);

    ManualModelBuilder::IndexValuePair testPair;
    testPair = ManualModelBuilder::createPair(-2.3, 0);
    outputRows1[3].push_back(testPair);
    testPair = ManualModelBuilder::createPair(1.0, 0);
    outputRows1[7].push_back(testPair);
    testPair = ManualModelBuilder::createPair(0.13, 0);
    outputRows1[10].push_back(testPair);

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant1);
    TEST_ASSERT(builder.m_name == outputName1);
    TEST_ASSERT(builder.m_costVector.size() == 1);
    TEST_ASSERT(std::equal(
        builder.m_costVector.begin(),
        builder.m_costVector.end(),
        outputCostVector1) == true);
    TEST_ASSERT(builder.m_variables.size() == 1);
    TEST_ASSERT(std::equal(
        builder.m_variables.begin(),
        builder.m_variables.end(),
        outputVariables1) == true);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 1);
    TEST_ASSERT(std::equal(
        builder.m_nonZerosInColumns.begin(),
        builder.m_nonZerosInColumns.end(),
        outputNonZerosInColumns1) == true);
    TEST_ASSERT(builder.m_columns == outputColumnLists1);

    TEST_ASSERT(std::equal(
        builder.m_nonZerosInRows.begin(),
        builder.m_nonZerosInRows.end(),
        outputNonZerosInRows1) == true);
    TEST_ASSERT(builder.m_constraints == outputConstrains1);
    TEST_ASSERT(builder.m_rows == outputRows1);

    /*******************************************************
     * Second variable
     ******************************************************/

    Variable testVariable2 = Variable::createPlusTypeVariable("x2", 1.3, -22.4);
    const Numerical::Double testCostCoefficient2 = -0.2;

    Vector testVector2(13);
    testVector2.set(1, 4.2);
    testVector2.set(5, 1.03);
    testVector2.set(7, 4.31);
    testVector2.set(12, 12.1);

    builder.addVariable(testVariable2, testCostCoefficient2, testVector2);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 4.2, -0.2 ]
     * m_variables = [ variable1, variable2 ]
     * m_nonZerosInColumns : [ 3, 4 ]
     * m_columns = 
     *        [  1.0  ; 7  ]  [  4.2  ; 1  ]
     *        [ -2.3  ; 3  ]  [  1.03 ; 5  ]
     *        [  0.13 ; 10 ]  [  4.31 ; 7  ]
     *                        [  12.1 ; 12 ]
     * 
     * m_constraints :    [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInRows : [ 0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  0,  1  ]
     * m_rows = 
     * 0.:  [ ]
     * 1.:  [ 4.2; 1 ] 
     * 2.:  [ ]
     * 3.:  [ -2.3; 0 ]
     * 4.:  [ ]
     * 5.:  [ 1.03; 1 ]
     * 6.:  [ ]
     * 7.:  [ 1.0; 0 ] [ 4.31; 1 ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.: [ 0.13; 0]
     * 11.: [ ]
     * 12.: [ 12.1; 1]
     */

    const Numerical::Double outputObjectiveConstant2 = 0;
    const std::string outputName2 = "";
    const Numerical::Double outputCostVector2[] = {4.2, -0.2};
    const Variable outputVariables2[] = {testVariable1, testVariable2};
    const unsigned int outputNonZerosInColumns2[] = {3, 4};

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists2(2);
    outputColumnLists2[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputColumnLists2[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputColumnLists2[0].push_back(ManualModelBuilder::createPair(0.13, 10));

    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(1.03, 5));
    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(4.31, 7));
    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(12.1, 12));

    const unsigned int outputNonZerosInRows2[] = {0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1, 0, 1};
    const std::vector<Constraint> outputConstrains2(13,
        Constraint::createGreaterTypeConstraint(0, 0.0));

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows2(13);
    testPair = ManualModelBuilder::createPair(4.2, 1);
    outputRows2[1].push_back(testPair);
    testPair = ManualModelBuilder::createPair(-2.3, 0);
    outputRows2[3].push_back(testPair);
    testPair = ManualModelBuilder::createPair(1.03, 1);
    outputRows2[5].push_back(testPair);
    testPair = ManualModelBuilder::createPair(1.0, 0);
    outputRows2[7].push_back(testPair);
    testPair = ManualModelBuilder::createPair(4.31, 1);
    outputRows2[7].push_back(testPair);
    testPair = ManualModelBuilder::createPair(0.13, 0);
    outputRows2[10].push_back(testPair);
    testPair = ManualModelBuilder::createPair(12.1, 1);
    outputRows2[12].push_back(testPair);

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant2);
    TEST_ASSERT(builder.m_name == outputName2);
    TEST_ASSERT(builder.m_costVector.size() == 2);
    TEST_ASSERT(std::equal(
        builder.m_costVector.begin(),
        builder.m_costVector.end(),
        outputCostVector2) == true);
    TEST_ASSERT(builder.m_variables.size() == 2);
    TEST_ASSERT(std::equal(
        builder.m_variables.begin(),
        builder.m_variables.end(),
        outputVariables2) == true);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 2);
    TEST_ASSERT(std::equal(
        builder.m_nonZerosInColumns.begin(),
        builder.m_nonZerosInColumns.end(),
        outputNonZerosInColumns2) == true);
    TEST_ASSERT(builder.m_columns == outputColumnLists2);

    TEST_ASSERT(std::equal(
        builder.m_nonZerosInRows.begin(),
        builder.m_nonZerosInRows.end(),
        outputNonZerosInRows2) == true);
    TEST_ASSERT(builder.m_constraints == outputConstrains2);
    TEST_ASSERT(builder.m_rows == outputRows2);

    /*******************************************************
     * Third variable, empty column
     ******************************************************/

    Variable testVariable3 = Variable::createFreeTypeVariable("x3", 4.0);
    const Numerical::Double testCostCoefficient3 = 3.0;

    Vector testVector3(13);

    builder.addVariable(testVariable3, testCostCoefficient3, testVector3);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 4.2, -0.2, 3.0 ]
     * m_variables = [ variable1, variable2, variable3 ]
     * m_nonZerosInColumns : [ 3, 4, 0 ]
     * m_columns = 
     *        [  1.0  ; 7  ]  [  4.2  ; 1  ]  [  ]
     *        [ -2.3  ; 3  ]  [  1.03 ; 5  ]
     *        [  0.13 ; 10 ]  [  4.31 ; 7  ]
     *                        [  12.1 ; 12 ]
     * 
     * m_constraints :    [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInRows : [ 0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  0,  1  ]
     * m_rows = 
     * 0.:  [ ]
     * 1.:  [ 4.2; 1 ] 
     * 2.:  [ ]
     * 3.:  [ -2.3; 0 ]
     * 4.:  [ ]
     * 5.:  [ 1.03; 1 ]
     * 6.:  [ ]
     * 7.:  [ 1.0; 0 ] [ 4.31; 1 ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.: [ 0.13; 0]
     * 11.: [ ]
     * 12.: [ 12.1; 1]
     */

    const Numerical::Double outputObjectiveConstant3 = 0;
    const std::string outputName3 = "";
    const Numerical::Double outputCostVector3[] = {4.2, -0.2, 3.0};
    const Variable outputVariables3[] = {testVariable1, testVariable2, testVariable3};
    const unsigned int outputNonZerosInColumns3[] = {3, 4, 0};

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists3 =
        outputColumnLists2;
    outputColumnLists3.resize(3);

    const unsigned int outputNonZerosInRows3[] = {0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1, 0, 1};
    const std::vector<Constraint> outputConstrains3(13,
        Constraint::createGreaterTypeConstraint(0, 0.0));

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows3 =
        outputRows2;

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant3);
    TEST_ASSERT(builder.m_name == outputName3);
    TEST_ASSERT(builder.m_costVector.size() == 3);
    TEST_ASSERT(std::equal(
        builder.m_costVector.begin(),
        builder.m_costVector.end(),
        outputCostVector3) == true);
    TEST_ASSERT(builder.m_variables.size() == 3);
    TEST_ASSERT(std::equal(
        builder.m_variables.begin(),
        builder.m_variables.end(),
        outputVariables3) == true);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 3);
    TEST_ASSERT(std::equal(
        builder.m_nonZerosInColumns.begin(),
        builder.m_nonZerosInColumns.end(),
        outputNonZerosInColumns3) == true);
    TEST_ASSERT(builder.m_columns == outputColumnLists3);

    TEST_ASSERT(std::equal(
        builder.m_nonZerosInRows.begin(),
        builder.m_nonZerosInRows.end(),
        outputNonZerosInRows3) == true);
    TEST_ASSERT(builder.m_constraints == outputConstrains3);
    TEST_ASSERT(builder.m_rows == outputRows3);

    /*******************************************************
     * Fourth variable
     ******************************************************/

    Variable testVariable4 = Variable::createBoundedTypeVariable("x4", 1.1, -0.3, 4.3);
    const Numerical::Double testCostCoefficient4 = 0.0;

    Vector testVector4(13);
    testVector4.set(0, 0.02);
    testVector4.set(11, 31.02);

    builder.addVariable(testVariable4, testCostCoefficient4, testVector4);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 4.2, -0.2, 3.0, 0.0 ]
     * m_variables = [ variable1, variable2, variable3, variable4 ]
     * m_nonZerosInColumns : [ 3, 4, 0, 2 ]
     * m_columns = 
     *        [  1.0  ; 7  ]  [  4.2  ; 1  ]  [  ]  [  0.02  ; 0  ]
     *        [ -2.3  ; 3  ]  [  1.03 ; 5  ]        [  31.02 ; 11 ]
     *        [  0.13 ; 10 ]  [  4.31 ; 7  ]
     *                        [  12.1 ; 12 ]
     * 
     * m_constraints :    [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInRows : [ 1, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  1,  1  ]
     * m_rows = 
     * 0.:  [ 0.02; 3]
     * 1.:  [ 4.2; 1 ] 
     * 2.:  [ ]
     * 3.:  [ -2.3; 0 ]
     * 4.:  [ ]
     * 5.:  [ 1.03; 1 ]
     * 6.:  [ ]
     * 7.:  [ 1.0; 0 ] [ 4.31; 1 ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.: [ 0.13; 0]
     * 11.: [ 31.02; 3]
     * 12.: [ 12.1; 1]
     */

    const Numerical::Double outputObjectiveConstant4 = 0;
    const std::string outputName4 = "";
    const Numerical::Double outputCostVector4[] = {4.2, -0.2, 3.0, 0.0};
    const Variable outputVariables4[] = {testVariable1, testVariable2,
        testVariable3, testVariable4};
    const unsigned int outputNonZerosInColumns4[] = {3, 4, 0, 2};

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists4 =
        outputColumnLists3;
    outputColumnLists4.resize(4);
    outputColumnLists4[3].push_back(ManualModelBuilder::createPair(0.02, 0));
    outputColumnLists4[3].push_back(ManualModelBuilder::createPair(31.02, 11));

    const unsigned int outputNonZerosInRows4[] = {1, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1, 1, 1};
    const std::vector<Constraint> outputConstrains4(13,
        Constraint::createGreaterTypeConstraint(0, 0.0));

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows4 =
        outputRows3;
    outputRows4[0].push_back(ManualModelBuilder::createPair(0.02, 3));
    outputRows4[11].push_back(ManualModelBuilder::createPair(31.02, 3));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant4);
    TEST_ASSERT(builder.m_name == outputName4);
    TEST_ASSERT(builder.m_costVector.size() == 4);
    TEST_ASSERT(std::equal(
        builder.m_costVector.begin(),
        builder.m_costVector.end(),
        outputCostVector4) == true);
    TEST_ASSERT(builder.m_variables.size() == 4);
    TEST_ASSERT(std::equal(
        builder.m_variables.begin(),
        builder.m_variables.end(),
        outputVariables4) == true);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 4);
    TEST_ASSERT(std::equal(
        builder.m_nonZerosInColumns.begin(),
        builder.m_nonZerosInColumns.end(),
        outputNonZerosInColumns4) == true);
    TEST_ASSERT(builder.m_columns == outputColumnLists4);

    TEST_ASSERT(std::equal(
        builder.m_nonZerosInRows.begin(),
        builder.m_nonZerosInRows.end(),
        outputNonZerosInRows4) == true);
    TEST_ASSERT(builder.m_constraints == outputConstrains4);
    TEST_ASSERT(builder.m_rows == outputRows4);

}

void ManualModelBuilderTestSuite::addVariable3()
{
    /*******************************************************
     * First variable
     ******************************************************/

    ManualModelBuilder builder;
    Variable testVariable1 = Variable::createMinusTypeVariable("x1", 0.0, 10.0);
    const Numerical::Double testCostCoefficient1 = 4.2;

    builder.addVariable(testVariable1, testCostCoefficient1, 3, 1.0, 7,
        -2.3, 3, 0.13, 10);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 4.2 ]
     * m_variables = [ variable1 ]
     * m_nonZerosInColumns : [ 3 ]
     * m_columns = 
     *        [  1.0  ; 7  ]
     *        [ -2.3  ; 3  ]
     *        [  0.13 ; 10 ]
     * 
     * m_constraints :    [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
     * m_nonZerosInRows : [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1  ]
     * m_rows = 
     * 0.:  [ ]
     * 1.:  [ ] 
     * 2.:  [ ]
     * 3.:  [ -2.3; 0 ]
     * 4.:  [ ]
     * 5.:  [ ]
     * 6.:  [ ]
     * 7.:  [ 1.0; 0 ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.: [ 0.13; 0]
     */

    const Numerical::Double outputObjectiveConstant1 = 0;
    const std::string outputName1 = "";
    const Numerical::Double outputCostVector1[] = {4.2};
    const Variable outputVariables1[] = {testVariable1};
    const unsigned int outputNonZerosInColumns1[] = {3};

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists1(1);
    outputColumnLists1[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputColumnLists1[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputColumnLists1[0].push_back(ManualModelBuilder::createPair(0.13, 10));

    const unsigned int outputNonZerosInRows1[] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1};
    const std::vector<Constraint> outputConstrains1(11,
        Constraint::createGreaterTypeConstraint(0, 0.0));

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows1(11);

    ManualModelBuilder::IndexValuePair testPair;
    testPair = ManualModelBuilder::createPair(-2.3, 0);
    outputRows1[3].push_back(testPair);
    testPair = ManualModelBuilder::createPair(1.0, 0);
    outputRows1[7].push_back(testPair);
    testPair = ManualModelBuilder::createPair(0.13, 0);
    outputRows1[10].push_back(testPair);

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant1);
    TEST_ASSERT(builder.m_name == outputName1);
    TEST_ASSERT(builder.m_costVector.size() == 1);
    TEST_ASSERT(std::equal(
        builder.m_costVector.begin(),
        builder.m_costVector.end(),
        outputCostVector1) == true);
    TEST_ASSERT(builder.m_variables.size() == 1);
    TEST_ASSERT(std::equal(
        builder.m_variables.begin(),
        builder.m_variables.end(),
        outputVariables1) == true);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 1);
    TEST_ASSERT(std::equal(
        builder.m_nonZerosInColumns.begin(),
        builder.m_nonZerosInColumns.end(),
        outputNonZerosInColumns1) == true);
    TEST_ASSERT(builder.m_columns == outputColumnLists1);

    TEST_ASSERT(std::equal(
        builder.m_nonZerosInRows.begin(),
        builder.m_nonZerosInRows.end(),
        outputNonZerosInRows1) == true);
    TEST_ASSERT(builder.m_constraints == outputConstrains1);
    TEST_ASSERT(builder.m_rows == outputRows1);

    /*******************************************************
     * Second variable
     ******************************************************/

    Variable testVariable2 = Variable::createPlusTypeVariable("x2", 1.3, -22.4);
    const Numerical::Double testCostCoefficient2 = -0.2;

    builder.addVariable(testVariable2, testCostCoefficient2, 4,
        4.2, 1, 1.03, 5, 4.31, 7, 12.1, 12);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 4.2, -0.2 ]
     * m_variables = [ variable1, variable2 ]
     * m_nonZerosInColumns : [ 3, 4 ]
     * m_columns = 
     *        [  1.0  ; 7  ]  [  4.2  ; 1  ]
     *        [ -2.3  ; 3  ]  [  1.03 ; 5  ]
     *        [  0.13 ; 10 ]  [  4.31 ; 7  ]
     *                        [  12.1 ; 12 ]
     * 
     * m_constraints :    [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInRows : [ 0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  0,  1  ]
     * m_rows = 
     * 0.:  [ ]
     * 1.:  [ 4.2; 1 ] 
     * 2.:  [ ]
     * 3.:  [ -2.3; 0 ]
     * 4.:  [ ]
     * 5.:  [ 1.03; 1 ]
     * 6.:  [ ]
     * 7.:  [ 1.0; 0 ] [ 4.31; 1 ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.: [ 0.13; 0]
     * 11.: [ ]
     * 12.: [ 12.1; 1]
     */

    const Numerical::Double outputObjectiveConstant2 = 0;
    const std::string outputName2 = "";
    const Numerical::Double outputCostVector2[] = {4.2, -0.2};
    const Variable outputVariables2[] = {testVariable1, testVariable2};
    const unsigned int outputNonZerosInColumns2[] = {3, 4};

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists2(2);
    outputColumnLists2[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputColumnLists2[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputColumnLists2[0].push_back(ManualModelBuilder::createPair(0.13, 10));

    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(1.03, 5));
    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(4.31, 7));
    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(12.1, 12));

    const unsigned int outputNonZerosInRows2[] = {0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1, 0, 1};
    const std::vector<Constraint> outputConstrains2(13,
        Constraint::createGreaterTypeConstraint(0, 0.0));

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows2(13);
    testPair = ManualModelBuilder::createPair(4.2, 1);
    outputRows2[1].push_back(testPair);
    testPair = ManualModelBuilder::createPair(-2.3, 0);
    outputRows2[3].push_back(testPair);
    testPair = ManualModelBuilder::createPair(1.03, 1);
    outputRows2[5].push_back(testPair);
    testPair = ManualModelBuilder::createPair(1.0, 0);
    outputRows2[7].push_back(testPair);
    testPair = ManualModelBuilder::createPair(4.31, 1);
    outputRows2[7].push_back(testPair);
    testPair = ManualModelBuilder::createPair(0.13, 0);
    outputRows2[10].push_back(testPair);
    testPair = ManualModelBuilder::createPair(12.1, 1);
    outputRows2[12].push_back(testPair);

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant2);
    TEST_ASSERT(builder.m_name == outputName2);
    TEST_ASSERT(builder.m_costVector.size() == 2);
    TEST_ASSERT(std::equal(
        builder.m_costVector.begin(),
        builder.m_costVector.end(),
        outputCostVector2) == true);
    TEST_ASSERT(builder.m_variables.size() == 2);
    TEST_ASSERT(std::equal(
        builder.m_variables.begin(),
        builder.m_variables.end(),
        outputVariables2) == true);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 2);
    TEST_ASSERT(std::equal(
        builder.m_nonZerosInColumns.begin(),
        builder.m_nonZerosInColumns.end(),
        outputNonZerosInColumns2) == true);
    TEST_ASSERT(builder.m_columns == outputColumnLists2);

    TEST_ASSERT(std::equal(
        builder.m_nonZerosInRows.begin(),
        builder.m_nonZerosInRows.end(),
        outputNonZerosInRows2) == true);
    TEST_ASSERT(builder.m_constraints == outputConstrains2);
    TEST_ASSERT(builder.m_rows == outputRows2);

    /*******************************************************
     * Third variable, empty column
     ******************************************************/

    Variable testVariable3 = Variable::createFreeTypeVariable("x3", 4.0);
    const Numerical::Double testCostCoefficient3 = 3.0;

    builder.addVariable(testVariable3, testCostCoefficient3, 0);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 4.2, -0.2, 3.0 ]
     * m_variables = [ variable1, variable2, variable3 ]
     * m_nonZerosInColumns : [ 3, 4, 0 ]
     * m_columns = 
     *        [  1.0  ; 7  ]  [  4.2  ; 1  ]  [  ]
     *        [ -2.3  ; 3  ]  [  1.03 ; 5  ]
     *        [  0.13 ; 10 ]  [  4.31 ; 7  ]
     *                        [  12.1 ; 12 ]
     * 
     * m_constraints :    [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInRows : [ 0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  0,  1  ]
     * m_rows = 
     * 0.:  [ ]
     * 1.:  [ 4.2; 1 ] 
     * 2.:  [ ]
     * 3.:  [ -2.3; 0 ]
     * 4.:  [ ]
     * 5.:  [ 1.03; 1 ]
     * 6.:  [ ]
     * 7.:  [ 1.0; 0 ] [ 4.31; 1 ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.: [ 0.13; 0]
     * 11.: [ ]
     * 12.: [ 12.1; 1]
     */

    const Numerical::Double outputObjectiveConstant3 = 0;
    const std::string outputName3 = "";
    const Numerical::Double outputCostVector3[] = {4.2, -0.2, 3.0};
    const Variable outputVariables3[] = {testVariable1, testVariable2, testVariable3};
    const unsigned int outputNonZerosInColumns3[] = {3, 4, 0};

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists3 =
        outputColumnLists2;
    outputColumnLists3.resize(3);

    const unsigned int outputNonZerosInRows3[] = {0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1, 0, 1};
    const std::vector<Constraint> outputConstrains3(13,
        Constraint::createGreaterTypeConstraint(0, 0.0));

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows3 =
        outputRows2;

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant3);
    TEST_ASSERT(builder.m_name == outputName3);
    TEST_ASSERT(builder.m_costVector.size() == 3);
    TEST_ASSERT(std::equal(
        builder.m_costVector.begin(),
        builder.m_costVector.end(),
        outputCostVector3) == true);
    TEST_ASSERT(builder.m_variables.size() == 3);
    TEST_ASSERT(std::equal(
        builder.m_variables.begin(),
        builder.m_variables.end(),
        outputVariables3) == true);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 3);
    TEST_ASSERT(std::equal(
        builder.m_nonZerosInColumns.begin(),
        builder.m_nonZerosInColumns.end(),
        outputNonZerosInColumns3) == true);
    TEST_ASSERT(builder.m_columns == outputColumnLists3);

    TEST_ASSERT(std::equal(
        builder.m_nonZerosInRows.begin(),
        builder.m_nonZerosInRows.end(),
        outputNonZerosInRows3) == true);
    TEST_ASSERT(builder.m_constraints == outputConstrains3);
    TEST_ASSERT(builder.m_rows == outputRows3);

    /*******************************************************
     * Fourth variable
     ******************************************************/

    Variable testVariable4 = Variable::createBoundedTypeVariable("x4", 1.1, -0.3, 4.3);
    const Numerical::Double testCostCoefficient4 = 0.0;

    Vector testVector4(13);
    testVector4.set(0, 0.02);
    testVector4.set(11, 31.02);

    builder.addVariable(testVariable4, testCostCoefficient4, 2,
        0.02, 0, 31.02, 11);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 4.2, -0.2, 3.0, 0.0 ]
     * m_variables = [ variable1, variable2, variable3, variable4 ]
     * m_nonZerosInColumns : [ 3, 4, 0, 2 ]
     * m_columns = 
     *        [  1.0  ; 7  ]  [  4.2  ; 1  ]  [  ]  [  0.02  ; 0  ]
     *        [ -2.3  ; 3  ]  [  1.03 ; 5  ]        [  31.02 ; 11 ]
     *        [  0.13 ; 10 ]  [  4.31 ; 7  ]
     *                        [  12.1 ; 12 ]
     * 
     * m_constraints :    [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInRows : [ 1, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  1,  1  ]
     * m_rows = 
     * 0.:  [ 0.02; 3]
     * 1.:  [ 4.2; 1 ] 
     * 2.:  [ ]
     * 3.:  [ -2.3; 0 ]
     * 4.:  [ ]
     * 5.:  [ 1.03; 1 ]
     * 6.:  [ ]
     * 7.:  [ 1.0; 0 ] [ 4.31; 1 ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.: [ 0.13; 0]
     * 11.: [ 31.02; 3]
     * 12.: [ 12.1; 1]
     */

    const Numerical::Double outputObjectiveConstant4 = 0;
    const std::string outputName4 = "";
    const Numerical::Double outputCostVector4[] = {4.2, -0.2, 3.0, 0.0};
    const Variable outputVariables4[] = {testVariable1, testVariable2,
        testVariable3, testVariable4};
    const unsigned int outputNonZerosInColumns4[] = {3, 4, 0, 2};

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists4 =
        outputColumnLists3;
    outputColumnLists4.resize(4);
    outputColumnLists4[3].push_back(ManualModelBuilder::createPair(0.02, 0));
    outputColumnLists4[3].push_back(ManualModelBuilder::createPair(31.02, 11));

    const unsigned int outputNonZerosInRows4[] = {1, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1, 1, 1};
    const std::vector<Constraint> outputConstrains4(13,
        Constraint::createGreaterTypeConstraint(0, 0.0));

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows4 =
        outputRows3;
    outputRows4[0].push_back(ManualModelBuilder::createPair(0.02, 3));
    outputRows4[11].push_back(ManualModelBuilder::createPair(31.02, 3));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant4);
    TEST_ASSERT(builder.m_name == outputName4);
    TEST_ASSERT(builder.m_costVector.size() == 4);
    TEST_ASSERT(std::equal(
        builder.m_costVector.begin(),
        builder.m_costVector.end(),
        outputCostVector4) == true);
    TEST_ASSERT(builder.m_variables.size() == 4);
    TEST_ASSERT(std::equal(
        builder.m_variables.begin(),
        builder.m_variables.end(),
        outputVariables4) == true);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 4);
    TEST_ASSERT(std::equal(
        builder.m_nonZerosInColumns.begin(),
        builder.m_nonZerosInColumns.end(),
        outputNonZerosInColumns4) == true);
    TEST_ASSERT(builder.m_columns == outputColumnLists4);

    TEST_ASSERT(std::equal(
        builder.m_nonZerosInRows.begin(),
        builder.m_nonZerosInRows.end(),
        outputNonZerosInRows4) == true);
    TEST_ASSERT(builder.m_constraints == outputConstrains4);
    TEST_ASSERT(builder.m_rows == outputRows4);
}

void ManualModelBuilderTestSuite::setGetConstraint()
{

}

void ManualModelBuilderTestSuite::printVectors(
    const std::vector< std::list<ManualModelBuilder::IndexValuePair> > & vector) const
{
    std::vector< std::list<ManualModelBuilder::IndexValuePair> >::const_iterator iter =
        vector.begin();
    std::vector< std::list<ManualModelBuilder::IndexValuePair> >::const_iterator iterEnd =
        vector.end();

    unsigned int index = 0;
    for (; iter != iterEnd; iter++, index++) {
        std::cout << index << ".: ";
        std::list<ManualModelBuilder::IndexValuePair>::const_iterator piter = iter->begin();
        std::list<ManualModelBuilder::IndexValuePair>::const_iterator piterEnd = iter->end();
        for (; piter != piterEnd; piter++) {
            std::cout << "[ " << piter->m_value << "; " << piter->m_index << " ] ";
        }
        std::cout << std::endl;
    }
}