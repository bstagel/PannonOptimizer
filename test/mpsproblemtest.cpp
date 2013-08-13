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
    //problem.loadFromFile("/home/smidla/NetBeansProjects/PanOpt/input/MPSNETLB/AFIRO.MPS");
    //problem.loadFromFile("/home/smidla/NetBeansProjects/PanOpt/input/mps/osa-60.mps");
    problem.loadFromFile("/home/smidla/NetBeansProjects/PanOpt/input/mps/OHMIN.MPS");
    Model model;
    model.build(problem);

    cout << "*************************************" << endl;
    cout << model.getName() << endl;
    cout << model.constraintCount() << " constraints" << endl;
    cout << model.variableCount() << " variables" << endl;

    model.getCostVector().show();
    unsigned int index;
    for (index = 0; index < model.variableCount(); index++) {
        cout << "****************************" << endl;
        model.getVariable(index).getVector()->show();
    }
    cout << "Rows: " << endl;
    for (index = 0; index < model.constraintCount(); index++) {
        cout << "****************************" << endl;
        model.getConstraint(index).getVector()->show();
    }

}
