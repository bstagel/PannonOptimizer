/*
 * File:   presolvertest.cpp
 * Author: borocz
 *
 * Created on 2014. március 27., 12:57
 */

#include <presolvertest.h>
#include <lp/model.h>
#include <lp/manualmodelbuilder.h>

using namespace std;

PresolverTestSuite::PresolverTestSuite(const char * name) : UnitTest(name)
{
    //ADD_TEST(PresolverTestSuite::init);
    ADD_TEST(PresolverTestSuite::singletonRows);
}

void PresolverTestSuite::init() {
    Model model;
    Presolver presolver(&model);
    TEST_ASSERT(1 == 1);
}

void PresolverTestSuite::singletonRows() {

    /*min.  3 + 4 + 2
     *st 1 <2 + 0 + 0 < 5
     *   4 <0 + 0 + 4 < 8
     *   4 <0 + 2 + 5 < 5
     *   0 <0 - 1 + 0 < 2
     *
     *   0 < x1 < 4
     *   0 < x2 < 4
     *   -2< x3 < 8
     */
    ManualModelBuilder builder;
    builder.setObjectiveType(MINIMIZE);
    builder.setObjectiveFunctionConstant(0);
    builder.setName("test1");
    Constraint c1 = Constraint::createRangeTypeConstraint("c1", 1, 5);
    Constraint c2 = Constraint::createRangeTypeConstraint("c2", 4, 8);
    Constraint c3 = Constraint::createRangeTypeConstraint("c3", 4, 5);
    Constraint c4 = Constraint::createRangeTypeConstraint("c4", 0, 2);
    Vector cvect1(3); cvect1.set(0, 2);
    Vector cvect2(3); cvect2.set(2, 4);
    Vector cvect3(3); cvect3.set(1, 2); cvect3.set(2,5);
    Vector cvect4(3); cvect4.set(1, -1);

    builder.addConstraint(c1, cvect1);
    builder.addConstraint(c2, cvect2);
    builder.addConstraint(c3, cvect3);
    builder.addConstraint(c4, cvect4);

    builder.setCostCoefficient(0, 3);
    builder.setCostCoefficient(1, 4);
    builder.setCostCoefficient(2, 2);

    Model model;
    model.build(builder);

    Presolver presolver(&model);
    presolver.presolve();
    int eR = 0;
    int eC = 0;
    for(unsigned int i = 0; i < presolver.m_modules.size(); i++) {
        eR += presolver.m_modules.at(i)->getRemovedConstraintCount();
        eC += presolver.m_modules.at(i)->getRemovedVariableCount();
    }
    std::cout << "PRESOLVER ELIM " << eR << " AND " << eC << std::endl;

    TEST_ASSERT(1 == 1);


}
