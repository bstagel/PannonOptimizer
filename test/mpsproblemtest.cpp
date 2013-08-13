/* 
 * File:   mpsproblemtest.cpp
 * Author: smidla
 * 
 * Created on 2013. augusztus 13., 14:01
 */

#include "mpsproblemtest.h"
#include <iostream>

using namespace std;

MpsProblemTestSuite::MpsProblemTestSuite(const char * name) : UnitTest(name)
{
    ADD_TEST(MpsProblemTestSuite::init);
}

void MpsProblemTestSuite::init()
{
    MpsProblem problem;
    problem.loadFromFile("/home/smidla/NetBeansProjects/PanOpt/input/MPSNETLB/AFIRO.MPS");
    Model model;
    model.build(problem);

    cout << "*************************************" << endl;
    cout << model.getName() << endl;
    cout << model.constraintCount() << " constraints" << endl;
    cout << model.variableCount() << " variables" << endl;
}