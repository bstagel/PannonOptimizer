///*
// * File:   mpsproblemtest.cpp
// * Author: smidla
// *
// * Created on 2013. augusztus 13., 14:01
// */

//#include <mpsproblemtest.h>
//#include <iostream>

//using namespace std;

//MpsProblemTestSuite::MpsProblemTestSuite(const char * name) : UnitTest(name)
//{
//    ADD_TEST(MpsProblemTestSuite::init);
//}

///*char * mygetenv(const char * name) {
//    return getenv(name);
//}*/

//void MpsProblemTestSuite::init()
//{

//    MpsModelBuilder problem;
//    //problem.loadFromFile("/home/smidla/NetBeansProjects/PanOpt/input/MPSNETLB/AFIRO.MPS");
//    problem.loadFromFile("/home/smidla/NetBeansProjects/PanOpt/input/mps/osa-60.mps");
//    //problem.loadFromFile("/home/smidla/NetBeansProjects/PanOpt/input/mps/OHMIN.MPS");
//    Model model;
//    model.build(problem);
//    return;
//    cout << "*************************************" << endl;
//    cout << model.getName() << endl;
//    cout << model.constraintCount() << " constraints" << endl;
//    cout << model.variableCount() << " variables" << endl;

//    model.getCostVector().show();
//    cout << "constant: " << model.getCostConstant() << endl;
//    unsigned int index;
//    for (index = 0; index < model.variableCount(); index++) {
//        cout << "****************************" << endl;
//        model.getVariable(index).getVector()->show();
//        cout << "\t" << model.getVariable(index).getLowerBound() << " <= " <<
//            model.getVariable(index).getName() << " <= " <<
//            model.getVariable(index).getUpperBound() << endl;

//    }
//    cout << "Rows: " << endl;
//    for (index = 0; index < model.constraintCount(); index++) {
//        cout << "****************************" << endl;
//        model.getConstraint(index).getVector()->show();
//        cout << "\t" << model.getConstraint(index).getLowerBound() << " <= " <<
//            model.getConstraint(index).getName() << " <= " <<
//            model.getConstraint(index).getUpperBound() << endl;
//    }

//}
