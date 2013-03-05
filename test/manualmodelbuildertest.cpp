#include "manualmodelbuildertest.h"
#include <algorithm>

void ManualModelBuilderTestSuite::run()
{
    init();
    setGetName();
    setGetObjectiveFunctionConstant();
    addVariable1();
    addVariable2();
    addVariable3();
    setGetConstraint();
    addConstraint1();
    addConstraint2();
    addConstraint3();
    setGetVariable();
    addVariableAndConstraint();
    setCostCoefficient();
    buildRow();
    buildColumn();
    buildCostVector();
}

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

    TEST_ASSERT(builder.getVariableCount() == 1);
    TEST_ASSERT(builder.getConstraintCount() == 11);

    TEST_ASSERT(builder.getVariable(0) == testVariable1);

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
    const std::vector<Constraint> outputConstraints1(11,
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
    TEST_ASSERT(builder.m_constraints == outputConstraints1);
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

    TEST_ASSERT(builder.getVariableCount() == 2);
    TEST_ASSERT(builder.getConstraintCount() == 13);

    TEST_ASSERT(builder.getVariable(0) == testVariable1);
    TEST_ASSERT(builder.getVariable(1) == testVariable2);

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
    const std::vector<Constraint> outputConstraints2(13,
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
    TEST_ASSERT(builder.m_constraints == outputConstraints2);
    TEST_ASSERT(builder.m_rows == outputRows2);

    /*******************************************************
     * Third variable, empty column
     ******************************************************/

    Variable testVariable3 = Variable::createFreeTypeVariable("x3", 4.0);
    const unsigned int testNonZeros3 = 0;
    const Numerical::Double testCostCoefficient3 = 3.0;

    builder.addVariable(testVariable3, testCostCoefficient3,
        testNonZeros3, 0, 0);

    TEST_ASSERT(builder.getVariableCount() == 3);
    TEST_ASSERT(builder.getConstraintCount() == 13);

    TEST_ASSERT(builder.getVariable(0) == testVariable1);
    TEST_ASSERT(builder.getVariable(1) == testVariable2);
    TEST_ASSERT(builder.getVariable(2) == testVariable3);

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
    const std::vector<Constraint> outputConstraints3(13,
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
    TEST_ASSERT(builder.m_constraints == outputConstraints3);
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

    TEST_ASSERT(builder.getVariableCount() == 4);
    TEST_ASSERT(builder.getConstraintCount() == 13);

    TEST_ASSERT(builder.getVariable(0) == testVariable1);
    TEST_ASSERT(builder.getVariable(1) == testVariable2);
    TEST_ASSERT(builder.getVariable(2) == testVariable3);
    TEST_ASSERT(builder.getVariable(3) == testVariable4);

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
    const std::vector<Constraint> outputConstraints4(13,
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
    TEST_ASSERT(builder.m_constraints == outputConstraints4);
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

    TEST_ASSERT(builder.getVariableCount() == 1);
    TEST_ASSERT(builder.getConstraintCount() == 11);

    TEST_ASSERT(builder.getVariable(0) == testVariable1);

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
    const std::vector<Constraint> outputConstraints1(11,
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
    TEST_ASSERT(builder.m_constraints == outputConstraints1);
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

    TEST_ASSERT(builder.getVariableCount() == 2);
    TEST_ASSERT(builder.getConstraintCount() == 13);

    TEST_ASSERT(builder.getVariable(0) == testVariable1);
    TEST_ASSERT(builder.getVariable(1) == testVariable2);

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
    const std::vector<Constraint> outputConstraints2(13,
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
    TEST_ASSERT(builder.m_constraints == outputConstraints2);
    TEST_ASSERT(builder.m_rows == outputRows2);

    /*******************************************************
     * Third variable, empty column
     ******************************************************/

    Variable testVariable3 = Variable::createFreeTypeVariable("x3", 4.0);
    const Numerical::Double testCostCoefficient3 = 3.0;

    Vector testVector3(13);

    builder.addVariable(testVariable3, testCostCoefficient3, testVector3);

    TEST_ASSERT(builder.getVariableCount() == 3);
    TEST_ASSERT(builder.getConstraintCount() == 13);

    TEST_ASSERT(builder.getVariable(0) == testVariable1);
    TEST_ASSERT(builder.getVariable(1) == testVariable2);
    TEST_ASSERT(builder.getVariable(2) == testVariable3);

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
    const std::vector<Constraint> outputConstraints3(13,
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
    TEST_ASSERT(builder.m_constraints == outputConstraints3);
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

    TEST_ASSERT(builder.getVariableCount() == 4);
    TEST_ASSERT(builder.getConstraintCount() == 13);

    TEST_ASSERT(builder.getVariable(0) == testVariable1);
    TEST_ASSERT(builder.getVariable(1) == testVariable2);
    TEST_ASSERT(builder.getVariable(2) == testVariable3);
    TEST_ASSERT(builder.getVariable(3) == testVariable4);

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
    const std::vector<Constraint> outputConstraints4(13,
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
    TEST_ASSERT(builder.m_constraints == outputConstraints4);
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

    TEST_ASSERT(builder.getVariableCount() == 1);
    TEST_ASSERT(builder.getConstraintCount() == 11);

    TEST_ASSERT(builder.getVariable(0) == testVariable1);

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
    const std::vector<Constraint> outputConstraints1(11,
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
    TEST_ASSERT(builder.m_constraints == outputConstraints1);
    TEST_ASSERT(builder.m_rows == outputRows1);

    /*******************************************************
     * Second variable
     ******************************************************/

    Variable testVariable2 = Variable::createPlusTypeVariable("x2", 1.3, -22.4);
    const Numerical::Double testCostCoefficient2 = -0.2;

    builder.addVariable(testVariable2, testCostCoefficient2, 4,
        4.2, 1, 1.03, 5, 4.31, 7, 12.1, 12);

    TEST_ASSERT(builder.getVariableCount() == 2);
    TEST_ASSERT(builder.getConstraintCount() == 13);

    TEST_ASSERT(builder.getVariable(0) == testVariable1);
    TEST_ASSERT(builder.getVariable(1) == testVariable2);

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
    const std::vector<Constraint> outputConstraints2(13,
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
    TEST_ASSERT(builder.m_constraints == outputConstraints2);
    TEST_ASSERT(builder.m_rows == outputRows2);

    /*******************************************************
     * Third variable, empty column
     ******************************************************/

    Variable testVariable3 = Variable::createFreeTypeVariable("x3", 4.0);
    const Numerical::Double testCostCoefficient3 = 3.0;

    builder.addVariable(testVariable3, testCostCoefficient3, 0);

    TEST_ASSERT(builder.getVariableCount() == 3);
    TEST_ASSERT(builder.getConstraintCount() == 13);

    TEST_ASSERT(builder.getVariable(0) == testVariable1);
    TEST_ASSERT(builder.getVariable(1) == testVariable2);
    TEST_ASSERT(builder.getVariable(2) == testVariable3);

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
    const std::vector<Constraint> outputConstraints3(13,
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
    TEST_ASSERT(builder.m_constraints == outputConstraints3);
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

    TEST_ASSERT(builder.getVariableCount() == 4);
    TEST_ASSERT(builder.getConstraintCount() == 13);

    TEST_ASSERT(builder.getVariable(0) == testVariable1);
    TEST_ASSERT(builder.getVariable(1) == testVariable2);
    TEST_ASSERT(builder.getVariable(2) == testVariable3);
    TEST_ASSERT(builder.getVariable(3) == testVariable4);

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
    const std::vector<Constraint> outputConstraints4(13,
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
    TEST_ASSERT(builder.m_constraints == outputConstraints4);
    TEST_ASSERT(builder.m_rows == outputRows4);
}

void ManualModelBuilderTestSuite::setGetConstraint()
{
    ManualModelBuilder builder;
    Variable testVariable = Variable::createMinusTypeVariable("x1", 0.0, 10.0);
    const Numerical::Double testValues[] = {1.0, -2.3, 0.13};
    const unsigned int testIndices[] = {7, 3, 10};
    const unsigned int testNonZeros = 3;
    const Numerical::Double testCostCoefficient = 4.2;

    builder.addVariable(testVariable, testCostCoefficient,
        testNonZeros, testValues, testIndices);

    TEST_ASSERT(builder.getConstraintCount() == 11);

    const Constraint outputConstraintDefault = Constraint::createGreaterTypeConstraint(0, 0.0);
    unsigned int index;
    for (index = 0; index < builder.getConstraintCount(); index++) {
        TEST_ASSERT(builder.getConstraint(index) == outputConstraintDefault);
    }

    const Constraint testConstraint1 = Constraint::createEqualityTypeConstraint("r1", 1.0);
    builder.setConstraint(0, testConstraint1);

    for (index = 0; index < builder.getConstraintCount(); index++) {
        switch (index) {
            case 0:
                TEST_ASSERT(builder.getConstraint(index) == testConstraint1);
                break;
            default:
                TEST_ASSERT(builder.getConstraint(index) == outputConstraintDefault);
        }
    }

    std::vector<Constraint> outputConstraints(11,
        Constraint::createGreaterTypeConstraint(0, 0.0));
    outputConstraints[0] = testConstraint1;

    TEST_ASSERT(builder.m_constraints == outputConstraints);

    const Constraint testConstraint2 = Constraint::createNonBindingTypeConstraint("r4");
    builder.setConstraint(5, testConstraint2);

    for (index = 0; index < builder.getConstraintCount(); index++) {
        switch (index) {
            case 0:
                TEST_ASSERT(builder.getConstraint(index) == testConstraint1);
                break;
            case 5:
                TEST_ASSERT(builder.getConstraint(index) == testConstraint2);
                break;
            default:
                TEST_ASSERT(builder.getConstraint(index) == outputConstraintDefault);
        }
    }

    outputConstraints[5] = testConstraint2;
    TEST_ASSERT(builder.m_constraints == outputConstraints);

    const Constraint testConstraint3 = Constraint::createRangeTypeConstraint("r10", 2, 4);
    builder.setConstraint(10, testConstraint3);

    for (index = 0; index < builder.getConstraintCount(); index++) {
        switch (index) {
            case 0:
                TEST_ASSERT(builder.getConstraint(index) == testConstraint1);
                break;
            case 5:
                TEST_ASSERT(builder.getConstraint(index) == testConstraint2);
                break;
            case 10:
                TEST_ASSERT(builder.getConstraint(index) == testConstraint3);
                break;
            default:
                TEST_ASSERT(builder.getConstraint(index) == outputConstraintDefault);
        }
    }

    outputConstraints[10] = testConstraint3;
    TEST_ASSERT(builder.m_constraints == outputConstraints);

    const Constraint testConstraint4 = Constraint::createLessTypeConstraint("r4_v2", 42);
    builder.setConstraint(5, testConstraint4);

    for (index = 0; index < builder.getConstraintCount(); index++) {
        switch (index) {
            case 0:
                TEST_ASSERT(builder.getConstraint(index) == testConstraint1);
                break;
            case 5:
                TEST_ASSERT(builder.getConstraint(index) == testConstraint4);
                break;
            case 10:
                TEST_ASSERT(builder.getConstraint(index) == testConstraint3);
                break;
            default:
                TEST_ASSERT(builder.getConstraint(index) == outputConstraintDefault);
        }
    }

    outputConstraints[5] = testConstraint4;
    TEST_ASSERT(builder.m_constraints == outputConstraints);

}

void ManualModelBuilderTestSuite::addConstraint1()
{
    /*******************************************************
     * First constraint
     ******************************************************/

    ManualModelBuilder builder;
    Constraint testConstraint1 = Constraint::createEqualityTypeConstraint("r1", 4.4);
    const Numerical::Double testValues1[] = {1.0, -2.3, 0.13};
    const unsigned int testIndices1[] = {7, 3, 10};
    const unsigned int testNonZeros1 = 3;

    builder.addConstraint(testConstraint1, testNonZeros1, testValues1, testIndices1);

    TEST_ASSERT(builder.getVariableCount() == 11);
    TEST_ASSERT(builder.getConstraintCount() == 1);

    TEST_ASSERT(builder.getConstraint(0) == testConstraint1);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 ]
     * m_nonZerosInRows : [ 3 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
     * m_nonZerosInColumns : [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1  ]
     * m_columns = 
     * 0.: 1.: 2.:  3.:        4.: 5.: 6.:  7.:       8.: 9.:   10.:
     * [ ] [ ] [ ] [ -2.3; 0 ] [ ] [ ] [ ] [ 1.0; 0 ] [ ] [ ] [ 0.13; 0 ]
     */

    const Numerical::Double outputObjectiveConstant1 = 0;
    const std::string outputName1 = "";
    const std::vector<Numerical::Double> outputCostVector1(11, 0.0);
    const std::vector<Variable> outputVariables1(11, Variable::createPlusTypeVariable(0, 0, 0.0));
    std::vector<unsigned int> outputNonZerosInColumns1(11);
    outputNonZerosInColumns1[3] = 1;
    outputNonZerosInColumns1[7] = 1;
    outputNonZerosInColumns1[10] = 1;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists1(11);
    outputColumnLists1[3].push_back(ManualModelBuilder::createPair(-2.3, 0));
    outputColumnLists1[7].push_back(ManualModelBuilder::createPair(1.0, 0));
    outputColumnLists1[10].push_back(ManualModelBuilder::createPair(0.13, 0));

    const std::vector<unsigned int> outputNonZerosInRows1(1, 3);
    std::vector<Constraint> outputConstraints1(1);
    outputConstraints1[0] = testConstraint1;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows1(1);

    outputRows1[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows1[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows1[0].push_back(ManualModelBuilder::createPair(0.13, 10));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant1);
    TEST_ASSERT(builder.m_name == outputName1);
    TEST_ASSERT(builder.m_costVector == outputCostVector1);
    TEST_ASSERT(builder.m_variables == outputVariables1);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns1);
    TEST_ASSERT(builder.m_columns == outputColumnLists1);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows1);
    TEST_ASSERT(builder.m_constraints == outputConstraints1);
    TEST_ASSERT(builder.m_rows == outputRows1);

    /*******************************************************
     * Second constraint
     ******************************************************/

    Constraint testConstraint2 = Constraint::createNonBindingTypeConstraint("r2");
    const Numerical::Double testValues2[] = {4.2, 1.03, 4.31, 12.1};
    const unsigned int testIndices2[] = {1, 5, 7, 12};
    const unsigned int testNonZeros2 = 4;

    builder.addConstraint(testConstraint2, testNonZeros2, testValues2, testIndices2);

    TEST_ASSERT(builder.getVariableCount() == 13);
    TEST_ASSERT(builder.getConstraintCount() == 2);

    TEST_ASSERT(builder.getConstraint(0) == testConstraint1);
    TEST_ASSERT(builder.getConstraint(1) == testConstraint2);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 constraint2]
     * m_nonZerosInRows : [ 3 ] [ 4 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 1.:  [  4.2  ; 1  ] [ 1.03  ; 5  ] [  4.31 ; 7 ]  [  12.1 ; 12 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInColumns : [ 0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  0,  1 ]
     * m_columns = 
     * 0.: 1.:        2.:  3.:        4.: 5.:         6.:  7.:        8.: 9.:   10.:      11.:   12:
     * [ ] [ 4.2; 1 ] [ ] [ -2.3; 0 ] [ ] [ 1.03; 0 ] [ ] [ 1.0; 0 ]  [ ] [ ] [ 0.13; 0 ] [ ]    [ 12.1; 1 ]
     * [ ]            [ ] [ ]         [ ] [ ]         [ ] [ 4.31; 0 ] [ ] [ ] [ ]         [ ]    [ ]
     */

    const Numerical::Double outputObjectiveConstant2 = 0;
    const std::string outputName2 = "";
    const std::vector<Numerical::Double> outputCostVector2(13, 0.0);
    const std::vector<Variable> outputVariables2(13,
        Variable::createPlusTypeVariable(0, 0.0, 0.0));
    std::vector<unsigned int> outputNonZerosInColumns2(13, 0);
    outputNonZerosInColumns2[1] = 1;
    outputNonZerosInColumns2[3] = 1;
    outputNonZerosInColumns2[5] = 1;
    outputNonZerosInColumns2[7] = 2;
    outputNonZerosInColumns2[10] = 1;
    outputNonZerosInColumns2[12] = 1;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists2(13);
    outputColumnLists2[3].push_back(ManualModelBuilder::createPair(-2.3, 0));
    outputColumnLists2[7].push_back(ManualModelBuilder::createPair(1.0, 0));
    outputColumnLists2[10].push_back(ManualModelBuilder::createPair(0.13, 0));
    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputColumnLists2[5].push_back(ManualModelBuilder::createPair(1.03, 1));
    outputColumnLists2[7].push_back(ManualModelBuilder::createPair(4.31, 1));
    outputColumnLists2[12].push_back(ManualModelBuilder::createPair(12.1, 1));

    std::vector<unsigned int> outputNonZerosInRows2(2);
    outputNonZerosInRows2[0] = 3;
    outputNonZerosInRows2[1] = 4;
    std::vector<Constraint> outputConstraints2(2);
    outputConstraints2[0] = testConstraint1;
    outputConstraints2[1] = testConstraint2;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows2(2);
    outputRows2[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows2[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows2[0].push_back(ManualModelBuilder::createPair(0.13, 10));
    outputRows2[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputRows2[1].push_back(ManualModelBuilder::createPair(1.03, 5));
    outputRows2[1].push_back(ManualModelBuilder::createPair(4.31, 7));
    outputRows2[1].push_back(ManualModelBuilder::createPair(12.1, 12));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant2);
    TEST_ASSERT(builder.m_name == outputName2);
    TEST_ASSERT(builder.m_costVector == outputCostVector2);
    TEST_ASSERT(builder.m_variables == outputVariables2);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns2);
    TEST_ASSERT(builder.m_columns == outputColumnLists2);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows2);
    TEST_ASSERT(builder.m_constraints == outputConstraints2);
    TEST_ASSERT(builder.m_rows == outputRows2);

    /*******************************************************
     * Third constraint, empty row
     ******************************************************/

    Constraint testConstraint3 = Constraint::createLessTypeConstraint("r2", 3.0);
    const unsigned int testNonZeros3 = 0;

    builder.addConstraint(testConstraint3, testNonZeros3, 0, 0);

    TEST_ASSERT(builder.getVariableCount() == 13);
    TEST_ASSERT(builder.getConstraintCount() == 3);

    TEST_ASSERT(builder.getConstraint(0) == testConstraint1);
    TEST_ASSERT(builder.getConstraint(1) == testConstraint2);
    TEST_ASSERT(builder.getConstraint(2) == testConstraint3);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 constraint2 constraint3]
     * m_nonZerosInRows : [ 3 ] [ 4 ] [ 0 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 1.:  [  4.2  ; 1  ] [ 1.03  ; 5  ] [  4.31 ; 7 ]  [  12.1 ; 12 ]
     * 2.:  [ ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInColumns : [ 0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  0,  1 ]
     * m_columns = 
     * 0.: 1.:        2.:  3.:        4.: 5.:         6.:  7.:        8.: 9.:   10.:      11.:   12:
     * [ ] [ 4.2; 1 ] [ ] [ -2.3; 0 ] [ ] [ 1.03; 0 ] [ ] [ 1.0; 0 ]  [ ] [ ] [ 0.13; 0 ] [ ]    [ 12.1; 1 ]
     * [ ]            [ ] [ ]         [ ] [ ]         [ ] [ 4.31; 0 ] [ ] [ ] [ ]         [ ]    [ ]
     */

    const Numerical::Double outputObjectiveConstant3 = 0;
    const std::string outputName3 = "";
    const std::vector<Numerical::Double> outputCostVector3(13, 0.0);
    const std::vector<Variable> outputVariables3(13,
        Variable::createPlusTypeVariable(0, 0.0, 0.0));
    const std::vector<unsigned int> outputNonZerosInColumns3 = outputNonZerosInColumns2;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> >
        outputColumnLists3 = outputColumnLists2;

    std::vector<unsigned int> outputNonZerosInRows3(3);
    outputNonZerosInRows3[0] = 3;
    outputNonZerosInRows3[1] = 4;
    outputNonZerosInRows3[2] = 0;
    std::vector<Constraint> outputConstraints3(3);
    outputConstraints3[0] = testConstraint1;
    outputConstraints3[1] = testConstraint2;
    outputConstraints3[2] = testConstraint3;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows3(3);
    outputRows3[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows3[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows3[0].push_back(ManualModelBuilder::createPair(0.13, 10));
    outputRows3[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputRows3[1].push_back(ManualModelBuilder::createPair(1.03, 5));
    outputRows3[1].push_back(ManualModelBuilder::createPair(4.31, 7));
    outputRows3[1].push_back(ManualModelBuilder::createPair(12.1, 12));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant3);
    TEST_ASSERT(builder.m_name == outputName3);
    TEST_ASSERT(builder.m_costVector == outputCostVector3);
    TEST_ASSERT(builder.m_variables == outputVariables3);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns3);
    TEST_ASSERT(builder.m_columns == outputColumnLists3);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows3);
    TEST_ASSERT(builder.m_constraints == outputConstraints3);
    TEST_ASSERT(builder.m_rows == outputRows3);

    /*******************************************************
     * Fourth constraint
     ******************************************************/

    Constraint testConstraint4 = Constraint::createGreaterTypeConstraint("r4", 43.2);
    const Numerical::Double testValues4[] = {0.02, 31.02};
    const unsigned int testIndices4[] = {0, 11};
    const unsigned int testNonZeros4 = 2;

    builder.addConstraint(testConstraint4, testNonZeros4, testValues4, testIndices4);

    TEST_ASSERT(builder.getVariableCount() == 13);
    TEST_ASSERT(builder.getConstraintCount() == 4);

    TEST_ASSERT(builder.getConstraint(0) == testConstraint1);
    TEST_ASSERT(builder.getConstraint(1) == testConstraint2);
    TEST_ASSERT(builder.getConstraint(2) == testConstraint3);
    TEST_ASSERT(builder.getConstraint(3) == testConstraint4);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 constraint2 constraint3 constraint4]
     * m_nonZerosInRows : [ 3 ] [ 4 ] [ 0 ] [ 2 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 1.:  [  4.2  ; 1  ] [ 1.03  ; 5  ] [  4.31 ; 7 ]  [  12.1 ; 12 ]
     * 2.:  [ ]
     * 3.:  [ 0.02 ; 0 ] [ 31.02 ; 11 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInColumns : [ 1, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  1,  1 ]
     * m_columns = 
     * 0.:          1.:        2.:  3.:        4.: 5.:         6.:  7.:        8.: 9.:   10.:      11.:           12:
     * [ 0.02 ; 3 ] [ 4.2; 1 ] [ ] [ -2.3; 0 ] [ ] [ 1.03; 0 ] [ ] [ 1.0; 0 ]  [ ] [ ] [ 0.13; 0 ] [ 31.02; 3 ]  [ 12.1; 1 ]
     * [ ]                     [ ] [ ]         [ ] [ ]         [ ] [ 4.31; 0 ] [ ] [ ] [ ]         [ ]           [ ]
     * 
     */

    const Numerical::Double outputObjectiveConstant4 = 0;
    const std::string outputName4 = "";
    const std::vector<Numerical::Double> outputCostVector4(13, 0.0);
    const std::vector<Variable> outputVariables4(13,
        Variable::createPlusTypeVariable(0, 0.0, 0.0));
    std::vector<unsigned int> outputNonZerosInColumns4(13);
    outputNonZerosInColumns4[0] = 1;
    outputNonZerosInColumns4[1] = 1;
    outputNonZerosInColumns4[3] = 1;
    outputNonZerosInColumns4[5] = 1;
    outputNonZerosInColumns4[7] = 2;
    outputNonZerosInColumns4[10] = 1;
    outputNonZerosInColumns4[11] = 1;
    outputNonZerosInColumns4[12] = 1;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> >
        outputColumnLists4 = outputColumnLists3;
    outputColumnLists4[0].push_back(ManualModelBuilder::createPair(0.02, 3));
    outputColumnLists4[11].push_back(ManualModelBuilder::createPair(31.02, 3));

    std::vector<unsigned int> outputNonZerosInRows4(4);
    outputNonZerosInRows4[0] = 3;
    outputNonZerosInRows4[1] = 4;
    outputNonZerosInRows4[2] = 0;
    outputNonZerosInRows4[3] = 2;
    std::vector<Constraint> outputConstraints4(4);
    outputConstraints4[0] = testConstraint1;
    outputConstraints4[1] = testConstraint2;
    outputConstraints4[2] = testConstraint3;
    outputConstraints4[3] = testConstraint4;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows4(4);
    outputRows4[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows4[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows4[0].push_back(ManualModelBuilder::createPair(0.13, 10));
    outputRows4[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputRows4[1].push_back(ManualModelBuilder::createPair(1.03, 5));
    outputRows4[1].push_back(ManualModelBuilder::createPair(4.31, 7));
    outputRows4[1].push_back(ManualModelBuilder::createPair(12.1, 12));
    outputRows4[3].push_back(ManualModelBuilder::createPair(0.02, 0));
    outputRows4[3].push_back(ManualModelBuilder::createPair(31.02, 11));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant4);
    TEST_ASSERT(builder.m_name == outputName4);
    TEST_ASSERT(builder.m_costVector == outputCostVector4);
    TEST_ASSERT(builder.m_variables == outputVariables4);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns4);
    TEST_ASSERT(builder.m_columns == outputColumnLists4);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows4);
    TEST_ASSERT(builder.m_constraints == outputConstraints4);
    TEST_ASSERT(builder.m_rows == outputRows4);
}

void ManualModelBuilderTestSuite::addConstraint2()
{
    /*******************************************************
     * First constraint
     ******************************************************/

    ManualModelBuilder builder;
    Constraint testConstraint1 = Constraint::createEqualityTypeConstraint("r1", 4.4);
    Vector testVector1(11);
    testVector1.set(7, 1.0);
    testVector1.set(3, -2.3);
    testVector1.set(10, 0.13);

    builder.addConstraint(testConstraint1, testVector1);

    TEST_ASSERT(builder.getVariableCount() == 11);
    TEST_ASSERT(builder.getConstraintCount() == 1);

    TEST_ASSERT(builder.getConstraint(0) == testConstraint1);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 ]
     * m_nonZerosInRows : [ 3 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
     * m_nonZerosInColumns : [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1  ]
     * m_columns = 
     * 0.: 1.: 2.:  3.:        4.: 5.: 6.:  7.:       8.: 9.:   10.:
     * [ ] [ ] [ ] [ -2.3; 0 ] [ ] [ ] [ ] [ 1.0; 0 ] [ ] [ ] [ 0.13; 0 ]
     */

    const Numerical::Double outputObjectiveConstant1 = 0;
    const std::string outputName1 = "";
    const std::vector<Numerical::Double> outputCostVector1(11, 0.0);
    const std::vector<Variable> outputVariables1(11, Variable::createPlusTypeVariable(0, 0, 0.0));
    std::vector<unsigned int> outputNonZerosInColumns1(11);
    outputNonZerosInColumns1[3] = 1;
    outputNonZerosInColumns1[7] = 1;
    outputNonZerosInColumns1[10] = 1;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists1(11);
    outputColumnLists1[3].push_back(ManualModelBuilder::createPair(-2.3, 0));
    outputColumnLists1[7].push_back(ManualModelBuilder::createPair(1.0, 0));
    outputColumnLists1[10].push_back(ManualModelBuilder::createPair(0.13, 0));

    const std::vector<unsigned int> outputNonZerosInRows1(1, 3);
    std::vector<Constraint> outputConstraints1(1);
    outputConstraints1[0] = testConstraint1;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows1(1);

    outputRows1[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows1[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows1[0].push_back(ManualModelBuilder::createPair(0.13, 10));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant1);
    TEST_ASSERT(builder.m_name == outputName1);
    TEST_ASSERT(builder.m_costVector == outputCostVector1);
    TEST_ASSERT(builder.m_variables == outputVariables1);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns1);
    TEST_ASSERT(builder.m_columns == outputColumnLists1);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows1);
    TEST_ASSERT(builder.m_constraints == outputConstraints1);
    TEST_ASSERT(builder.m_rows == outputRows1);

    /*******************************************************
     * Second constraint
     ******************************************************/

    Constraint testConstraint2 = Constraint::createNonBindingTypeConstraint("r2");
    Vector testVector2(13);
    testVector2.set(1, 4.2);
    testVector2.set(5, 1.03);
    testVector2.set(7, 4.31);
    testVector2.set(12, 12.1);

    builder.addConstraint(testConstraint2, testVector2);

    TEST_ASSERT(builder.getVariableCount() == 13);
    TEST_ASSERT(builder.getConstraintCount() == 2);

    TEST_ASSERT(builder.getConstraint(0) == testConstraint1);
    TEST_ASSERT(builder.getConstraint(1) == testConstraint2);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 constraint2]
     * m_nonZerosInRows : [ 3 ] [ 4 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 1.:  [  4.2  ; 1  ] [ 1.03  ; 5  ] [  4.31 ; 7 ]  [  12.1 ; 12 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInColumns : [ 0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  0,  1 ]
     * m_columns = 
     * 0.: 1.:        2.:  3.:        4.: 5.:         6.:  7.:        8.: 9.:   10.:      11.:   12:
     * [ ] [ 4.2; 1 ] [ ] [ -2.3; 0 ] [ ] [ 1.03; 0 ] [ ] [ 1.0; 0 ]  [ ] [ ] [ 0.13; 0 ] [ ]    [ 12.1; 1 ]
     * [ ]            [ ] [ ]         [ ] [ ]         [ ] [ 4.31; 0 ] [ ] [ ] [ ]         [ ]    [ ]
     */

    const Numerical::Double outputObjectiveConstant2 = 0;
    const std::string outputName2 = "";
    const std::vector<Numerical::Double> outputCostVector2(13, 0.0);
    const std::vector<Variable> outputVariables2(13,
        Variable::createPlusTypeVariable(0, 0.0, 0.0));
    std::vector<unsigned int> outputNonZerosInColumns2(13, 0);
    outputNonZerosInColumns2[1] = 1;
    outputNonZerosInColumns2[3] = 1;
    outputNonZerosInColumns2[5] = 1;
    outputNonZerosInColumns2[7] = 2;
    outputNonZerosInColumns2[10] = 1;
    outputNonZerosInColumns2[12] = 1;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists2(13);
    outputColumnLists2[3].push_back(ManualModelBuilder::createPair(-2.3, 0));
    outputColumnLists2[7].push_back(ManualModelBuilder::createPair(1.0, 0));
    outputColumnLists2[10].push_back(ManualModelBuilder::createPair(0.13, 0));
    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputColumnLists2[5].push_back(ManualModelBuilder::createPair(1.03, 1));
    outputColumnLists2[7].push_back(ManualModelBuilder::createPair(4.31, 1));
    outputColumnLists2[12].push_back(ManualModelBuilder::createPair(12.1, 1));

    std::vector<unsigned int> outputNonZerosInRows2(2);
    outputNonZerosInRows2[0] = 3;
    outputNonZerosInRows2[1] = 4;
    std::vector<Constraint> outputConstraints2(2);
    outputConstraints2[0] = testConstraint1;
    outputConstraints2[1] = testConstraint2;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows2(2);
    outputRows2[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows2[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows2[0].push_back(ManualModelBuilder::createPair(0.13, 10));
    outputRows2[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputRows2[1].push_back(ManualModelBuilder::createPair(1.03, 5));
    outputRows2[1].push_back(ManualModelBuilder::createPair(4.31, 7));
    outputRows2[1].push_back(ManualModelBuilder::createPair(12.1, 12));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant2);
    TEST_ASSERT(builder.m_name == outputName2);
    TEST_ASSERT(builder.m_costVector == outputCostVector2);
    TEST_ASSERT(builder.m_variables == outputVariables2);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns2);
    TEST_ASSERT(builder.m_columns == outputColumnLists2);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows2);
    TEST_ASSERT(builder.m_constraints == outputConstraints2);
    TEST_ASSERT(builder.m_rows == outputRows2);

    /*******************************************************
     * Third constraint, empty row
     ******************************************************/

    Constraint testConstraint3 = Constraint::createLessTypeConstraint("r2", 3.0);
    Vector testVector3;

    builder.addConstraint(testConstraint3, testVector3);

    TEST_ASSERT(builder.getVariableCount() == 13);
    TEST_ASSERT(builder.getConstraintCount() == 3);

    TEST_ASSERT(builder.getConstraint(0) == testConstraint1);
    TEST_ASSERT(builder.getConstraint(1) == testConstraint2);
    TEST_ASSERT(builder.getConstraint(2) == testConstraint3);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 constraint2 constraint3]
     * m_nonZerosInRows : [ 3 ] [ 4 ] [ 0 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 1.:  [  4.2  ; 1  ] [ 1.03  ; 5  ] [  4.31 ; 7 ]  [  12.1 ; 12 ]
     * 2.:  [ ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInColumns : [ 0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  0,  1 ]
     * m_columns = 
     * 0.: 1.:        2.:  3.:        4.: 5.:         6.:  7.:        8.: 9.:   10.:      11.:   12:
     * [ ] [ 4.2; 1 ] [ ] [ -2.3; 0 ] [ ] [ 1.03; 0 ] [ ] [ 1.0; 0 ]  [ ] [ ] [ 0.13; 0 ] [ ]    [ 12.1; 1 ]
     * [ ]            [ ] [ ]         [ ] [ ]         [ ] [ 4.31; 0 ] [ ] [ ] [ ]         [ ]    [ ]
     */

    const Numerical::Double outputObjectiveConstant3 = 0;
    const std::string outputName3 = "";
    const std::vector<Numerical::Double> outputCostVector3(13, 0.0);
    const std::vector<Variable> outputVariables3(13,
        Variable::createPlusTypeVariable(0, 0.0, 0.0));
    const std::vector<unsigned int> outputNonZerosInColumns3 = outputNonZerosInColumns2;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> >
        outputColumnLists3 = outputColumnLists2;

    std::vector<unsigned int> outputNonZerosInRows3(3);
    outputNonZerosInRows3[0] = 3;
    outputNonZerosInRows3[1] = 4;
    outputNonZerosInRows3[2] = 0;
    std::vector<Constraint> outputConstraints3(3);
    outputConstraints3[0] = testConstraint1;
    outputConstraints3[1] = testConstraint2;
    outputConstraints3[2] = testConstraint3;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows3(3);
    outputRows3[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows3[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows3[0].push_back(ManualModelBuilder::createPair(0.13, 10));
    outputRows3[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputRows3[1].push_back(ManualModelBuilder::createPair(1.03, 5));
    outputRows3[1].push_back(ManualModelBuilder::createPair(4.31, 7));
    outputRows3[1].push_back(ManualModelBuilder::createPair(12.1, 12));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant3);
    TEST_ASSERT(builder.m_name == outputName3);
    TEST_ASSERT(builder.m_costVector == outputCostVector3);
    TEST_ASSERT(builder.m_variables == outputVariables3);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns3);
    TEST_ASSERT(builder.m_columns == outputColumnLists3);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows3);
    TEST_ASSERT(builder.m_constraints == outputConstraints3);
    TEST_ASSERT(builder.m_rows == outputRows3);

    /*******************************************************
     * Fourth constraint
     ******************************************************/

    Constraint testConstraint4 = Constraint::createGreaterTypeConstraint("r4", 43.2);
    Vector testVector4(12);
    testVector4.set(0, 0.02);
    testVector4.set(11, 31.02);
    builder.addConstraint(testConstraint4, testVector4);

    TEST_ASSERT(builder.getVariableCount() == 13);
    TEST_ASSERT(builder.getConstraintCount() == 4);

    TEST_ASSERT(builder.getConstraint(0) == testConstraint1);
    TEST_ASSERT(builder.getConstraint(1) == testConstraint2);
    TEST_ASSERT(builder.getConstraint(2) == testConstraint3);
    TEST_ASSERT(builder.getConstraint(3) == testConstraint4);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 constraint2 constraint3 constraint4]
     * m_nonZerosInRows : [ 3 ] [ 4 ] [ 0 ] [ 2 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 1.:  [  4.2  ; 1  ] [ 1.03  ; 5  ] [  4.31 ; 7 ]  [  12.1 ; 12 ]
     * 2.:  [ ]
     * 3.:  [ 0.02 ; 0 ] [ 31.02 ; 11 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInColumns : [ 1, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  1,  1 ]
     * m_columns = 
     * 0.:          1.:        2.:  3.:        4.: 5.:         6.:  7.:        8.: 9.:   10.:      11.:           12:
     * [ 0.02 ; 3 ] [ 4.2; 1 ] [ ] [ -2.3; 0 ] [ ] [ 1.03; 0 ] [ ] [ 1.0; 0 ]  [ ] [ ] [ 0.13; 0 ] [ 31.02; 3 ]  [ 12.1; 1 ]
     * [ ]                     [ ] [ ]         [ ] [ ]         [ ] [ 4.31; 0 ] [ ] [ ] [ ]         [ ]           [ ]
     * 
     */

    const Numerical::Double outputObjectiveConstant4 = 0;
    const std::string outputName4 = "";
    const std::vector<Numerical::Double> outputCostVector4(13, 0.0);
    const std::vector<Variable> outputVariables4(13,
        Variable::createPlusTypeVariable(0, 0.0, 0.0));
    std::vector<unsigned int> outputNonZerosInColumns4(13);
    outputNonZerosInColumns4[0] = 1;
    outputNonZerosInColumns4[1] = 1;
    outputNonZerosInColumns4[3] = 1;
    outputNonZerosInColumns4[5] = 1;
    outputNonZerosInColumns4[7] = 2;
    outputNonZerosInColumns4[10] = 1;
    outputNonZerosInColumns4[11] = 1;
    outputNonZerosInColumns4[12] = 1;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> >
        outputColumnLists4 = outputColumnLists3;
    outputColumnLists4[0].push_back(ManualModelBuilder::createPair(0.02, 3));
    outputColumnLists4[11].push_back(ManualModelBuilder::createPair(31.02, 3));

    std::vector<unsigned int> outputNonZerosInRows4(4);
    outputNonZerosInRows4[0] = 3;
    outputNonZerosInRows4[1] = 4;
    outputNonZerosInRows4[2] = 0;
    outputNonZerosInRows4[3] = 2;
    std::vector<Constraint> outputConstraints4(4);
    outputConstraints4[0] = testConstraint1;
    outputConstraints4[1] = testConstraint2;
    outputConstraints4[2] = testConstraint3;
    outputConstraints4[3] = testConstraint4;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows4(4);
    outputRows4[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows4[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows4[0].push_back(ManualModelBuilder::createPair(0.13, 10));
    outputRows4[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputRows4[1].push_back(ManualModelBuilder::createPair(1.03, 5));
    outputRows4[1].push_back(ManualModelBuilder::createPair(4.31, 7));
    outputRows4[1].push_back(ManualModelBuilder::createPair(12.1, 12));
    outputRows4[3].push_back(ManualModelBuilder::createPair(0.02, 0));
    outputRows4[3].push_back(ManualModelBuilder::createPair(31.02, 11));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant4);
    TEST_ASSERT(builder.m_name == outputName4);
    TEST_ASSERT(builder.m_costVector == outputCostVector4);
    TEST_ASSERT(builder.m_variables == outputVariables4);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns4);
    TEST_ASSERT(builder.m_columns == outputColumnLists4);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows4);
    TEST_ASSERT(builder.m_constraints == outputConstraints4);
    TEST_ASSERT(builder.m_rows == outputRows4);
}

void ManualModelBuilderTestSuite::addConstraint3()
{
    /*******************************************************
     * First constraint
     ******************************************************/

    ManualModelBuilder builder;
    Constraint testConstraint1 = Constraint::createEqualityTypeConstraint("r1", 4.4);

    builder.addConstraint(testConstraint1, 3, 1.0, 7, -2.3, 3, 0.13, 10);

    TEST_ASSERT(builder.getVariableCount() == 11);
    TEST_ASSERT(builder.getConstraintCount() == 1);

    TEST_ASSERT(builder.getConstraint(0) == testConstraint1);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 ]
     * m_nonZerosInRows : [ 3 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
     * m_nonZerosInColumns : [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1  ]
     * m_columns = 
     * 0.: 1.: 2.:  3.:        4.: 5.: 6.:  7.:       8.: 9.:   10.:
     * [ ] [ ] [ ] [ -2.3; 0 ] [ ] [ ] [ ] [ 1.0; 0 ] [ ] [ ] [ 0.13; 0 ]
     */

    const Numerical::Double outputObjectiveConstant1 = 0;
    const std::string outputName1 = "";
    const std::vector<Numerical::Double> outputCostVector1(11, 0.0);
    const std::vector<Variable> outputVariables1(11, Variable::createPlusTypeVariable(0, 0, 0.0));
    std::vector<unsigned int> outputNonZerosInColumns1(11);
    outputNonZerosInColumns1[3] = 1;
    outputNonZerosInColumns1[7] = 1;
    outputNonZerosInColumns1[10] = 1;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists1(11);
    outputColumnLists1[3].push_back(ManualModelBuilder::createPair(-2.3, 0));
    outputColumnLists1[7].push_back(ManualModelBuilder::createPair(1.0, 0));
    outputColumnLists1[10].push_back(ManualModelBuilder::createPair(0.13, 0));

    const std::vector<unsigned int> outputNonZerosInRows1(1, 3);
    std::vector<Constraint> outputConstraints1(1);
    outputConstraints1[0] = testConstraint1;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows1(1);

    outputRows1[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows1[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows1[0].push_back(ManualModelBuilder::createPair(0.13, 10));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant1);
    TEST_ASSERT(builder.m_name == outputName1);
    TEST_ASSERT(builder.m_costVector == outputCostVector1);
    TEST_ASSERT(builder.m_variables == outputVariables1);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns1);
    TEST_ASSERT(builder.m_columns == outputColumnLists1);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows1);
    TEST_ASSERT(builder.m_constraints == outputConstraints1);
    TEST_ASSERT(builder.m_rows == outputRows1);

    /*******************************************************
     * Second constraint
     ******************************************************/

    Constraint testConstraint2 = Constraint::createNonBindingTypeConstraint("r2");

    builder.addConstraint(testConstraint2, 4, 4.2, 1, 1.03, 5, 4.31, 7, 12.1, 12);

    TEST_ASSERT(builder.getVariableCount() == 13);
    TEST_ASSERT(builder.getConstraintCount() == 2);

    TEST_ASSERT(builder.getConstraint(0) == testConstraint1);
    TEST_ASSERT(builder.getConstraint(1) == testConstraint2);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 constraint2]
     * m_nonZerosInRows : [ 3 ] [ 4 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 1.:  [  4.2  ; 1  ] [ 1.03  ; 5  ] [  4.31 ; 7 ]  [  12.1 ; 12 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInColumns : [ 0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  0,  1 ]
     * m_columns = 
     * 0.: 1.:        2.:  3.:        4.: 5.:         6.:  7.:        8.: 9.:   10.:      11.:   12:
     * [ ] [ 4.2; 1 ] [ ] [ -2.3; 0 ] [ ] [ 1.03; 0 ] [ ] [ 1.0; 0 ]  [ ] [ ] [ 0.13; 0 ] [ ]    [ 12.1; 1 ]
     * [ ]            [ ] [ ]         [ ] [ ]         [ ] [ 4.31; 0 ] [ ] [ ] [ ]         [ ]    [ ]
     */

    const Numerical::Double outputObjectiveConstant2 = 0;
    const std::string outputName2 = "";
    const std::vector<Numerical::Double> outputCostVector2(13, 0.0);
    const std::vector<Variable> outputVariables2(13,
        Variable::createPlusTypeVariable(0, 0.0, 0.0));
    std::vector<unsigned int> outputNonZerosInColumns2(13, 0);
    outputNonZerosInColumns2[1] = 1;
    outputNonZerosInColumns2[3] = 1;
    outputNonZerosInColumns2[5] = 1;
    outputNonZerosInColumns2[7] = 2;
    outputNonZerosInColumns2[10] = 1;
    outputNonZerosInColumns2[12] = 1;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputColumnLists2(13);
    outputColumnLists2[3].push_back(ManualModelBuilder::createPair(-2.3, 0));
    outputColumnLists2[7].push_back(ManualModelBuilder::createPair(1.0, 0));
    outputColumnLists2[10].push_back(ManualModelBuilder::createPair(0.13, 0));
    outputColumnLists2[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputColumnLists2[5].push_back(ManualModelBuilder::createPair(1.03, 1));
    outputColumnLists2[7].push_back(ManualModelBuilder::createPair(4.31, 1));
    outputColumnLists2[12].push_back(ManualModelBuilder::createPair(12.1, 1));

    std::vector<unsigned int> outputNonZerosInRows2(2);
    outputNonZerosInRows2[0] = 3;
    outputNonZerosInRows2[1] = 4;
    std::vector<Constraint> outputConstraints2(2);
    outputConstraints2[0] = testConstraint1;
    outputConstraints2[1] = testConstraint2;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows2(2);
    outputRows2[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows2[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows2[0].push_back(ManualModelBuilder::createPair(0.13, 10));
    outputRows2[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputRows2[1].push_back(ManualModelBuilder::createPair(1.03, 5));
    outputRows2[1].push_back(ManualModelBuilder::createPair(4.31, 7));
    outputRows2[1].push_back(ManualModelBuilder::createPair(12.1, 12));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant2);
    TEST_ASSERT(builder.m_name == outputName2);
    TEST_ASSERT(builder.m_costVector == outputCostVector2);
    TEST_ASSERT(builder.m_variables == outputVariables2);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns2);
    TEST_ASSERT(builder.m_columns == outputColumnLists2);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows2);
    TEST_ASSERT(builder.m_constraints == outputConstraints2);
    TEST_ASSERT(builder.m_rows == outputRows2);

    /*******************************************************
     * Third constraint, empty row
     ******************************************************/

    Constraint testConstraint3 = Constraint::createLessTypeConstraint("r2", 3.0);

    builder.addConstraint(testConstraint3, 0);

    TEST_ASSERT(builder.getVariableCount() == 13);
    TEST_ASSERT(builder.getConstraintCount() == 3);

    TEST_ASSERT(builder.getConstraint(0) == testConstraint1);
    TEST_ASSERT(builder.getConstraint(1) == testConstraint2);
    TEST_ASSERT(builder.getConstraint(2) == testConstraint3);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 constraint2 constraint3]
     * m_nonZerosInRows : [ 3 ] [ 4 ] [ 0 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 1.:  [  4.2  ; 1  ] [ 1.03  ; 5  ] [  4.31 ; 7 ]  [  12.1 ; 12 ]
     * 2.:  [ ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInColumns : [ 0, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  0,  1 ]
     * m_columns = 
     * 0.: 1.:        2.:  3.:        4.: 5.:         6.:  7.:        8.: 9.:   10.:      11.:   12:
     * [ ] [ 4.2; 1 ] [ ] [ -2.3; 0 ] [ ] [ 1.03; 0 ] [ ] [ 1.0; 0 ]  [ ] [ ] [ 0.13; 0 ] [ ]    [ 12.1; 1 ]
     * [ ]            [ ] [ ]         [ ] [ ]         [ ] [ 4.31; 0 ] [ ] [ ] [ ]         [ ]    [ ]
     */

    const Numerical::Double outputObjectiveConstant3 = 0;
    const std::string outputName3 = "";
    const std::vector<Numerical::Double> outputCostVector3(13, 0.0);
    const std::vector<Variable> outputVariables3(13,
        Variable::createPlusTypeVariable(0, 0.0, 0.0));
    const std::vector<unsigned int> outputNonZerosInColumns3 = outputNonZerosInColumns2;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> >
        outputColumnLists3 = outputColumnLists2;

    std::vector<unsigned int> outputNonZerosInRows3(3);
    outputNonZerosInRows3[0] = 3;
    outputNonZerosInRows3[1] = 4;
    outputNonZerosInRows3[2] = 0;
    std::vector<Constraint> outputConstraints3(3);
    outputConstraints3[0] = testConstraint1;
    outputConstraints3[1] = testConstraint2;
    outputConstraints3[2] = testConstraint3;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows3(3);
    outputRows3[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows3[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows3[0].push_back(ManualModelBuilder::createPair(0.13, 10));
    outputRows3[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputRows3[1].push_back(ManualModelBuilder::createPair(1.03, 5));
    outputRows3[1].push_back(ManualModelBuilder::createPair(4.31, 7));
    outputRows3[1].push_back(ManualModelBuilder::createPair(12.1, 12));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant3);
    TEST_ASSERT(builder.m_name == outputName3);
    TEST_ASSERT(builder.m_costVector == outputCostVector3);
    TEST_ASSERT(builder.m_variables == outputVariables3);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns3);
    TEST_ASSERT(builder.m_columns == outputColumnLists3);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows3);
    TEST_ASSERT(builder.m_constraints == outputConstraints3);
    TEST_ASSERT(builder.m_rows == outputRows3);

    /*******************************************************
     * Fourth constraint
     ******************************************************/

    Constraint testConstraint4 = Constraint::createGreaterTypeConstraint("r4", 43.2);

    builder.addConstraint(testConstraint4, 2, 0.02, 0, 31.02, 11);

    TEST_ASSERT(builder.getVariableCount() == 13);
    TEST_ASSERT(builder.getConstraintCount() == 4);

    TEST_ASSERT(builder.getConstraint(0) == testConstraint1);
    TEST_ASSERT(builder.getConstraint(1) == testConstraint2);
    TEST_ASSERT(builder.getConstraint(2) == testConstraint3);
    TEST_ASSERT(builder.getConstraint(3) == testConstraint4);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 constraint2 constraint3 constraint4]
     * m_nonZerosInRows : [ 3 ] [ 4 ] [ 0 ] [ 2 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 1.:  [  4.2  ; 1  ] [ 1.03  ; 5  ] [  4.31 ; 7 ]  [  12.1 ; 12 ]
     * 2.:  [ ]
     * 3.:  [ 0.02 ; 0 ] [ 31.02 ; 11 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
     * m_nonZerosInColumns : [ 1, 1, 0, 1, 0, 1, 0, 2, 0, 0, 1,  1,  1 ]
     * m_columns = 
     * 0.:          1.:        2.:  3.:        4.: 5.:         6.:  7.:        8.: 9.:   10.:      11.:           12:
     * [ 0.02 ; 3 ] [ 4.2; 1 ] [ ] [ -2.3; 0 ] [ ] [ 1.03; 0 ] [ ] [ 1.0; 0 ]  [ ] [ ] [ 0.13; 0 ] [ 31.02; 3 ]  [ 12.1; 1 ]
     * [ ]                     [ ] [ ]         [ ] [ ]         [ ] [ 4.31; 0 ] [ ] [ ] [ ]         [ ]           [ ]
     * 
     */

    const Numerical::Double outputObjectiveConstant4 = 0;
    const std::string outputName4 = "";
    const std::vector<Numerical::Double> outputCostVector4(13, 0.0);
    const std::vector<Variable> outputVariables4(13,
        Variable::createPlusTypeVariable(0, 0.0, 0.0));
    std::vector<unsigned int> outputNonZerosInColumns4(13);
    outputNonZerosInColumns4[0] = 1;
    outputNonZerosInColumns4[1] = 1;
    outputNonZerosInColumns4[3] = 1;
    outputNonZerosInColumns4[5] = 1;
    outputNonZerosInColumns4[7] = 2;
    outputNonZerosInColumns4[10] = 1;
    outputNonZerosInColumns4[11] = 1;
    outputNonZerosInColumns4[12] = 1;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> >
        outputColumnLists4 = outputColumnLists3;
    outputColumnLists4[0].push_back(ManualModelBuilder::createPair(0.02, 3));
    outputColumnLists4[11].push_back(ManualModelBuilder::createPair(31.02, 3));

    std::vector<unsigned int> outputNonZerosInRows4(4);
    outputNonZerosInRows4[0] = 3;
    outputNonZerosInRows4[1] = 4;
    outputNonZerosInRows4[2] = 0;
    outputNonZerosInRows4[3] = 2;
    std::vector<Constraint> outputConstraints4(4);
    outputConstraints4[0] = testConstraint1;
    outputConstraints4[1] = testConstraint2;
    outputConstraints4[2] = testConstraint3;
    outputConstraints4[3] = testConstraint4;

    std::vector< std::list<ManualModelBuilder::IndexValuePair> > outputRows4(4);
    outputRows4[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows4[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows4[0].push_back(ManualModelBuilder::createPair(0.13, 10));
    outputRows4[1].push_back(ManualModelBuilder::createPair(4.2, 1));
    outputRows4[1].push_back(ManualModelBuilder::createPair(1.03, 5));
    outputRows4[1].push_back(ManualModelBuilder::createPair(4.31, 7));
    outputRows4[1].push_back(ManualModelBuilder::createPair(12.1, 12));
    outputRows4[3].push_back(ManualModelBuilder::createPair(0.02, 0));
    outputRows4[3].push_back(ManualModelBuilder::createPair(31.02, 11));

    TEST_ASSERT(builder.m_objectiveConstant == outputObjectiveConstant4);
    TEST_ASSERT(builder.m_name == outputName4);
    TEST_ASSERT(builder.m_costVector == outputCostVector4);
    TEST_ASSERT(builder.m_variables == outputVariables4);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns4);
    TEST_ASSERT(builder.m_columns == outputColumnLists4);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows4);
    TEST_ASSERT(builder.m_constraints == outputConstraints4);
    TEST_ASSERT(builder.m_rows == outputRows4);
}

void ManualModelBuilderTestSuite::setGetVariable()
{
    ManualModelBuilder builder;
    Constraint testConstraint = Constraint::createEqualityTypeConstraint("r1", 4.4);
    const Numerical::Double testValues[] = {1.0, -2.3, 0.13};
    const unsigned int testIndices[] = {7, 3, 10};
    const unsigned int testNonZeros = 3;

    builder.addConstraint(testConstraint, testNonZeros, testValues, testIndices);

    TEST_ASSERT(builder.getVariableCount() == 11);

    const Variable outputVariableDefault = Variable::createPlusTypeVariable(0, 0, 0.0);
    unsigned int index;
    for (index = 0; index < builder.getVariableCount(); index++) {
        TEST_ASSERT(builder.getVariable(index) == outputVariableDefault);
    }

    const Variable testVariable1 = Variable::createFixedTypeVariable("x1", 4.5);
    builder.setVariable(0, testVariable1);

    for (index = 0; index < builder.getVariableCount(); index++) {
        switch (index) {
            case 0:
                TEST_ASSERT(builder.getVariable(index) == testVariable1);
                break;
            default:
                TEST_ASSERT(builder.getVariable(index) == outputVariableDefault);
        }
    }

    std::vector<Variable> outputVariables(11,
        Variable::createPlusTypeVariable(0, 0, 0.0));
    outputVariables[0] = testVariable1;

    TEST_ASSERT(builder.m_variables == outputVariables);

    const Variable testVariable2 = Variable::createFreeTypeVariable("x2", 4.2);
    builder.setVariable(5, testVariable2);

    for (index = 0; index < builder.getVariableCount(); index++) {
        switch (index) {
            case 0:
                TEST_ASSERT(builder.getVariable(index) == testVariable1);
                break;
            case 5:
                TEST_ASSERT(builder.getVariable(index) == testVariable2);
                break;
            default:
                TEST_ASSERT(builder.getVariable(index) == outputVariableDefault);
        }
    }

    outputVariables[5] = testVariable2;
    TEST_ASSERT(builder.m_variables == outputVariables);

    const Variable testVariable3 = Variable::createBoundedTypeVariable("x3", 10.0, 4.5, 31.2);
    builder.setVariable(10, testVariable3);

    for (index = 0; index < builder.getVariableCount(); index++) {
        switch (index) {
            case 0:
                TEST_ASSERT(builder.getVariable(index) == testVariable1);
                break;
            case 5:
                TEST_ASSERT(builder.getVariable(index) == testVariable2);
                break;
            case 10:
                TEST_ASSERT(builder.getVariable(index) == testVariable3);
                break;
            default:
                TEST_ASSERT(builder.getVariable(index) == outputVariableDefault);
        }
    }

    outputVariables[10] = testVariable3;
    TEST_ASSERT(builder.m_variables == outputVariables);

    const Variable testVariable4 = Variable::createMinusTypeVariable("x4", -3.4, 43.2);
    builder.setVariable(5, testVariable4);

    for (index = 0; index < builder.getVariableCount(); index++) {
        switch (index) {
            case 0:
                TEST_ASSERT(builder.getVariable(index) == testVariable1);
                break;
            case 5:
                TEST_ASSERT(builder.getVariable(index) == testVariable4);
                break;
            case 10:
                TEST_ASSERT(builder.getVariable(index) == testVariable3);
                break;
            default:
                TEST_ASSERT(builder.getVariable(index) == outputVariableDefault);
        }
    }

    outputVariables[5] = testVariable4;
    TEST_ASSERT(builder.m_variables == outputVariables);
}

void ManualModelBuilderTestSuite::setCostCoefficient()
{
    ManualModelBuilder builder;
    Constraint testConstraint = Constraint::createEqualityTypeConstraint("r1", 4.4);
    const Numerical::Double testValues[] = {1.0, -2.3, 0.13};
    const unsigned int testIndices[] = {7, 3, 10};
    const unsigned int testNonZeros = 3;

    builder.addConstraint(testConstraint, testNonZeros, testValues, testIndices);

    const Numerical::Double outputCostValue1 = 4.5;
    const unsigned int index1 = 2;

    builder.setCostCoefficient(index1, outputCostValue1);

    std::vector<Numerical::Double> outputCostVector(11);
    outputCostVector[index1] = outputCostValue1;

    TEST_ASSERT(builder.m_costVector == outputCostVector);

    const Numerical::Double outputCostValue2 = -12.6;
    const unsigned int index2 = 0;

    builder.setCostCoefficient(index2, outputCostValue2);

    outputCostVector[index2] = outputCostValue2;

    TEST_ASSERT(builder.m_costVector == outputCostVector);

}

void ManualModelBuilderTestSuite::addVariableAndConstraint()
{
    ManualModelBuilder builder;
    Constraint testConstraint1 = Constraint::createEqualityTypeConstraint("r1", 4.4);
    Vector testRow1(11);
    testRow1.set(7, 1.0);
    testRow1.set(3, -2.3);
    testRow1.set(10, 0.13);

    builder.addConstraint(testConstraint1, testRow1);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
     * m_constraints = [ constraint1 ]
     * m_nonZerosInRows : [ 3 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
     * m_nonZerosInColumns : [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1  ]
     * m_columns = 
     * 0.: 1.: 2.:  3.:        4.: 5.: 6.:  7.:       8.: 9.:   10.:
     * [ ] [ ] [ ] [ -2.3; 0 ] [ ] [ ] [ ] [ 1.0; 0 ] [ ] [ ] [ 0.13; 0 ]
     */

    Variable testVariable1 = Variable::createMinusTypeVariable("x1", 0.0, 10.0);
    Vector testColumn1(14);
    testColumn1.set(0, 1.2);
    testColumn1.set(2, 2.3);
    testColumn1.set(11, 4.13);
    const Numerical::Double testCostCoefficient1 = 4.2;

    builder.addVariable(testVariable1, testCostCoefficient1,
        testColumn1);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4.2]
     * m_constraints =    [ c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12]
     * m_nonZerosInRows : [ 4,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ] [  1.2 ; 11  ]
     * 1.:  [ ]
     * 2.:  [  2.3 ; 11 ]
     * 3.:  [ ]
     * 4.:  [ ]
     * 5.:  [ ]
     * 6.:  [ ]
     * 7.:  [ ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.:  [ ]
     * 11.:  [ 4.13; 11 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ]
     * m_nonZerosInColumns : [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,  3  ]
     * m_columns = 
     * 0.: 1.: 2.:  3.:        4.: 5.: 6.:  7.:       8.: 9.:   10.:      11.:
     * [ ] [ ] [ ] [ -2.3; 0 ] [ ] [ ] [ ] [ 1.0; 0 ] [ ] [ ] [ 0.13; 0 ] [ 1.2; 0]
     *                                                                    [ 2.3; 2]                                                                  [ 4.13; 11]
     */

    TEST_ASSERT(builder.getVariableCount() == 12);
    TEST_ASSERT(builder.getConstraintCount() == 12);

    std::vector<Numerical::Double> outputCostVector(12);
    outputCostVector[11] = testCostCoefficient1;

    std::vector<Constraint> outputConstraints(12,
        Constraint::createGreaterTypeConstraint(0, 0.0));
    outputConstraints[0] = testConstraint1;

    std::vector<unsigned int> outputNonZerosInRows(12);
    outputNonZerosInRows[0] = 4;
    outputNonZerosInRows[2] = 1;
    outputNonZerosInRows[11] = 1;

    std::vector< std::list< ManualModelBuilder::IndexValuePair > > outputRows(12);
    outputRows[0].push_back(ManualModelBuilder::createPair(1.0, 7));
    outputRows[0].push_back(ManualModelBuilder::createPair(-2.3, 3));
    outputRows[0].push_back(ManualModelBuilder::createPair(0.13, 10));
    outputRows[0].push_back(ManualModelBuilder::createPair(1.2, 11));
    outputRows[2].push_back(ManualModelBuilder::createPair(2.3, 11));
    outputRows[11].push_back(ManualModelBuilder::createPair(4.13, 11));

    std::vector<Variable> outputVariables(12,
        Variable::createPlusTypeVariable(0, 0, 0.0));
    outputVariables[11] = testVariable1;

    std::vector<unsigned int> outputNonZerosInColumns(12);
    outputNonZerosInColumns[3] = 1;
    outputNonZerosInColumns[7] = 1;
    outputNonZerosInColumns[10] = 1;
    outputNonZerosInColumns[11] = 3;

    std::vector< std::list< ManualModelBuilder::IndexValuePair > > outputColumns(12);
    outputColumns[3].push_back(ManualModelBuilder::createPair(-2.3, 0));
    outputColumns[7].push_back(ManualModelBuilder::createPair(1.0, 0));
    outputColumns[10].push_back(ManualModelBuilder::createPair(0.13, 0));
    outputColumns[11].push_back(ManualModelBuilder::createPair(1.2, 0));
    outputColumns[11].push_back(ManualModelBuilder::createPair(2.3, 2));
    outputColumns[11].push_back(ManualModelBuilder::createPair(4.13, 11));

    TEST_ASSERT(builder.m_costVector == outputCostVector);
    TEST_ASSERT(builder.m_constraints == outputConstraints);
    TEST_ASSERT(builder.m_nonZerosInRows == outputNonZerosInRows);
    TEST_ASSERT(builder.m_rows == outputRows);
    TEST_ASSERT(builder.m_variables == outputVariables);
    TEST_ASSERT(builder.m_nonZerosInColumns == outputNonZerosInColumns);
    TEST_ASSERT(builder.m_columns == outputColumns);

}

void ManualModelBuilderTestSuite::buildRow()
{
    ManualModelBuilder builder;
    Constraint testConstraint1 = Constraint::createEqualityTypeConstraint("r1", 4.4);
    Vector testRow1(11);
    testRow1.set(7, 1.0);
    testRow1.set(3, -2.3);
    testRow1.set(10, 0.13);

    builder.addConstraint(testConstraint1, testRow1);

    Variable testVariable1 = Variable::createMinusTypeVariable("x1", 0.0, 10.0);
    Vector testColumn1(14);
    testColumn1.set(0, 1.2);
    testColumn1.set(2, 2.3);
    testColumn1.set(11, 4.13);
    const Numerical::Double testCostCoefficient1 = 4.2;

    builder.addVariable(testVariable1, testCostCoefficient1,
        testColumn1);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4.2]
     * m_constraints =    [ c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12]
     * m_nonZerosInRows : [ 4,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ] [  1.2 ; 11  ]
     * 1.:  [ ]
     * 2.:  [  2.3 ; 11 ]
     * 3.:  [ ]
     * 4.:  [ ]
     * 5.:  [ ]
     * 6.:  [ ]
     * 7.:  [ ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.:  [ ]
     * 11.:  [ 4.13; 11 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ]
     * m_nonZerosInColumns : [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,  3  ]
     * m_columns = 
     * 0.: 1.: 2.:  3.:        4.: 5.: 6.:  7.:       8.: 9.:   10.:      11.:
     * [ ] [ ] [ ] [ -2.3; 0 ] [ ] [ ] [ ] [ 1.0; 0 ] [ ] [ ] [ 0.13; 0 ] [ 1.2; 0]
     *                                                                    [ 2.3; 2]
     *                                                                    [ 4.13; 11]
     */
    unsigned int index;
    unsigned int rowIndex;
    Vector row;
    for (rowIndex = 0; rowIndex < 12; rowIndex++) {
        builder.buildRow(rowIndex, &row);
        TEST_ASSERT(row.length() == 12);

        switch (rowIndex) {
            case 0:
                for (index = 0; index < 12; index++) {
                    switch (index) {
                        case 3:
                            TEST_ASSERT(row.at(index) == -2.3);
                            break;
                        case 7:
                            TEST_ASSERT(row.at(index) == 1.0);
                            break;
                        case 10:
                            TEST_ASSERT(row.at(index) == 0.13);
                            break;
                        case 11:
                            TEST_ASSERT(row.at(index) == 1.2);
                            break;
                        default:
                            TEST_ASSERT(row.at(index) == 0.0);
                    }
                }
                break;
            case 2:
                for (index = 0; index < 12; index++) {
                    switch (index) {
                        case 11:
                            TEST_ASSERT(row.at(index) == 2.3);
                            break;
                        default:
                            TEST_ASSERT(row.at(index) == 0.0);
                    }
                }
                break;
            case 11:
                for (index = 0; index < 12; index++) {
                    switch (index) {
                        case 11:
                            TEST_ASSERT(row.at(index) == 4.13);
                            break;
                        default:
                            TEST_ASSERT(row.at(index) == 0.0);
                    }
                }
                break;
            default:
                for (index = 0; index < 12; index++) {
                    TEST_ASSERT(row.at(index) == 0.0);
                }
                break;
        }
    }
}

void ManualModelBuilderTestSuite::buildColumn()
{
    ManualModelBuilder builder;
    Constraint testConstraint1 = Constraint::createEqualityTypeConstraint("r1", 4.4);
    Vector testRow1(11);
    testRow1.set(7, 1.0);
    testRow1.set(3, -2.3);
    testRow1.set(10, 0.13);

    builder.addConstraint(testConstraint1, testRow1);

    Variable testVariable1 = Variable::createMinusTypeVariable("x1", 0.0, 10.0);
    Vector testColumn1(14);
    testColumn1.set(0, 1.2);
    testColumn1.set(2, 2.3);
    testColumn1.set(11, 4.13);
    const Numerical::Double testCostCoefficient1 = 4.2;

    builder.addVariable(testVariable1, testCostCoefficient1,
        testColumn1);

    /*
     * m_objecitveConstant : 0
     * m_name = ""
     * m_costVector : [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4.2]
     * m_constraints =    [ c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12]
     * m_nonZerosInRows : [ 4,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1 ]
     * m_rows = 
     * 0.:  [  1.0  ; 7  ] [ -2.3  ; 3  ] [  0.13 ; 10 ] [  1.2 ; 11  ]
     * 1.:  [ ]
     * 2.:  [  2.3 ; 11 ]
     * 3.:  [ ]
     * 4.:  [ ]
     * 5.:  [ ]
     * 6.:  [ ]
     * 7.:  [ ]
     * 8.:  [ ]
     * 9.:  [ ]
     * 10.:  [ ]
     * 11.:  [ 4.13; 11 ]
     * 
     * m_variables :         [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ]
     * m_nonZerosInColumns : [ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,  3  ]
     * m_columns = 
     * 0.: 1.: 2.:  3.:        4.: 5.: 6.:  7.:       8.: 9.:   10.:      11.:
     * [ ] [ ] [ ] [ -2.3; 0 ] [ ] [ ] [ ] [ 1.0; 0 ] [ ] [ ] [ 0.13; 0 ] [ 1.2; 0]
     *                                                                    [ 2.3; 2]
     *                                                                    [ 4.13; 11]
     */
    unsigned int index;
    unsigned int columnIndex;
    Vector column;
    for (columnIndex = 0; columnIndex < 12; columnIndex++) {
        builder.buildColumn(columnIndex, &column);
        TEST_ASSERT(column.length() == 12);

        switch (columnIndex) {
            case 11:
                for (index = 0; index < 12; index++) {
                    switch (index) {
                        case 0:
                            TEST_ASSERT(column.at(index) == 1.2);
                            break;
                        case 2:
                            TEST_ASSERT(column.at(index) == 2.3);
                            break;
                        case 11:
                            TEST_ASSERT(column.at(index) == 4.13);
                            break;
                        default:
                            TEST_ASSERT(column.at(index) == 0.0);
                    }
                }
                break;
            case 3:
                for (index = 0; index < 12; index++) {
                    switch (index) {
                        case 0:
                            TEST_ASSERT(column.at(index) == -2.3);
                            break;
                        default:
                            TEST_ASSERT(column.at(index) == 0.0);
                    }
                }
                break;
            case 7:
                for (index = 0; index < 12; index++) {
                    switch (index) {
                        case 0:
                            TEST_ASSERT(column.at(index) == 1.0);
                            break;
                        default:
                            TEST_ASSERT(column.at(index) == 0.0);
                    }
                }
                break;
            case 10:
                for (index = 0; index < 12; index++) {
                    switch (index) {
                        case 0:
                            TEST_ASSERT(column.at(index) == 0.13);
                            break;
                        default:
                            TEST_ASSERT(column.at(index) == 0.0);
                    }
                }
                break;
            default:
                for (index = 0; index < 12; index++) {
                    TEST_ASSERT(column.at(index) == 0.0);
                }
                break;
        }
    }
}

void ManualModelBuilderTestSuite::buildCostVector()
{
    ManualModelBuilder builder;
    Constraint testConstraint1 = Constraint::createEqualityTypeConstraint("r1", 4.4);
    Vector testRow1(11);
    testRow1.set(7, 1.0);
    testRow1.set(3, -2.3);
    testRow1.set(10, 0.13);

    builder.addConstraint(testConstraint1, testRow1);

    Variable testVariable1 = Variable::createMinusTypeVariable("x1", 0.0, 10.0);
    Vector testColumn1(14);
    testColumn1.set(0, 1.2);
    testColumn1.set(2, 2.3);
    testColumn1.set(11, 4.13);
    const Numerical::Double testCostCoefficient1 = 4.2;
    const unsigned int testIndex1 = 11;

    builder.addVariable(testVariable1, testCostCoefficient1,
        testColumn1);

    const Numerical::Double testCostCoefficient2 = 4.5;
    const unsigned int testIndex2 = 3;
    const Numerical::Double testCostCoefficient3 = -1.5;
    const unsigned int testIndex3 = 6;
    const Numerical::Double testCostCoefficient4 = 0.45;
    const unsigned int testIndex4 = 8;
    builder.setCostCoefficient(testIndex2, testCostCoefficient2);
    builder.setCostCoefficient(testIndex3, testCostCoefficient3);
    builder.setCostCoefficient(testIndex4, testCostCoefficient4);

    Vector costVector;
    builder.buildCostVector(&costVector);
    unsigned int index;
    TEST_ASSERT(costVector.length() == 12);
    for (index = 0; index < 12; index++) {
        switch (index) {
            case testIndex1:
                TEST_ASSERT(costVector.at(index) == testCostCoefficient1);
                break;
            case testIndex2:
                TEST_ASSERT(costVector.at(index) == testCostCoefficient2);
                break;
            case testIndex3:
                TEST_ASSERT(costVector.at(index) == testCostCoefficient3);
                break;
            case testIndex4:
                TEST_ASSERT(costVector.at(index) == testCostCoefficient4);
                break;
            default:
                TEST_ASSERT(costVector.at(index) == 0.0);
        }
    }
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