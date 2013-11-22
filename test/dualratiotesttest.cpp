#include <test/dualratiotesttest.h>
#include <indexlisttest.h>
#include <linalg/matrix.h>
#include <simplex/simplex.h>

#include <iostream>
using namespace std;

DualRatiotestTestSuite::DualRatiotestTestSuite(const char * name):UnitTest(name)
{
    ADD_TEST(DualRatiotestTestSuite::testStagelPh1);
    ADD_TEST(DualRatiotestTestSuite::testMaros1Ph1);
    ADD_TEST(DualRatiotestTestSuite::testMaros2Ph1);
    ADD_TEST(DualRatiotestTestSuite::testStagelCf3Ph1);
//    ADD_TEST(DualRatiotestTestSuite::testStagelPh2);
//    ADD_TEST(DualRatiotestTestSuite::testMaros1Ph2);
//    ADD_TEST(DualRatiotestTestSuite::testMaros2Ph2);

    initTestDefines();
}

void DualRatiotestTestSuite::initVariable(Model& model,
                                          Numerical::Double lb,
                                          Numerical::Double ub){
    Variable var;
    var.setLowerBound(lb);
    var.setUpperBound(ub);

    model.m_variables.push_back(var);
}

void DualRatiotestTestSuite::initTestDefines()
{
    #define _M_ Simplex::MINUS
    #define _P_ Simplex::PLUS
    #define _F_ Simplex::FEASIBLE
    #define _NLB_ Simplex::NONBASIC_AT_LB
    #define _NUB_ Simplex::NONBASIC_AT_UB
    #define _NF_ Simplex::NONBASIC_FREE
    #define _B_ Simplex::BASIC
    #define _INF_ Numerical::Infinity
}

void DualRatiotestTestSuite::testStagelPh1()
{
//init
    Model model;
    Vector alpha(16),reducedCosts(16);
    Numerical::Double alphas[] = { 3, 7, 0, 1, 2,-1,-2,-8, 3,-2,
                                   2, 4, 0, 0, 1, 1};
    for(int i = 0; i <16 ;i++){
        alpha.set(i, alphas[i]);
    }
    Numerical::Double d[] = { -12, 14, -6, 0, -1, 1, -4, 16, 0, -5,
                                  6, -12, 0, 2, 3, 0};

    Simplex::FEASIBILITY d_feasibilities[] = {_M_, _F_, _M_, _F_,_M_,
                                              _P_, _F_, _M_, _P_, _M_,
                                              _F_, _P_, _P_};
    IndexList<> reducedCostFeasibilities;
    reducedCostFeasibilities.init(16,Simplex::FEASIBILITY_ENUM_LENGTH);
    for(int i=0;i<13;i++){
        reducedCostFeasibilities.insert(d_feasibilities[i],i+2);
    }

    for(int i=0;i <16;i++){
        reducedCosts.set(i, d[i]);
    }
    Numerical::Double lowerbounds[] = { 0, 0, 0, 0, 0, 0, 0,-_INF_,
                                     -_INF_,-_INF_,
                                     -_INF_,-_INF_,
                                     -_INF_,-_INF_,
                                     -_INF_};
    Numerical::Double upperbounds[] = { 0, 5, _INF_, _INF_,
                                    _INF_,_INF_,_INF_,
                                    _INF_,_INF_,_INF_,
                                    _INF_,_INF_,_INF_,
                                    _INF_,_INF_};
    Simplex::VARIABLE_STATE varstates[] ={ _NLB_, _NLB_, _NLB_,
                                           _NLB_,_NLB_, _NLB_,
                                           _NLB_, _NF_, _NF_,
                                           _NF_, _NF_, _NF_,
                                           _NF_,_NF_, _NF_};
    IndexList<const Numerical::Double*> variableStates;
    variableStates.init(16,Simplex::VARIABLE_STATE_ENUM_LENGTH);

    for(int i=0; i < 15 ;i++){
        initVariable(model,lowerbounds[i],upperbounds[i]);
        variableStates.insert(varstates[i],i);
    }
    double var = 16.0;
    initVariable(model,0,_INF_);
    variableStates.insert(_B_,15,&var);

    Constraint con;
    model.addConstraint(con,alpha);
    SimplexModel mo(model);
    DualRatiotestUpdater updater(&reducedCostFeasibilities);

//performing
    DualRatiotest dualtest(mo,reducedCosts,reducedCostFeasibilities,variableStates,updater);

    dualtest.generateBreakpointsPhase1(alpha,7,-55);
    Numerical::Double breakpts[] = { 3, 3, 0.5, 2, 2, 0, 1};

    for(int i = 0 ; i < dualtest.m_breakpoints.size()-1; i++){
//        cout<<"bpts: "<<dualtest.m_breakpoints[i+1]<<endl;
        TEST_ASSERT(dualtest.m_breakpoints[i+1].value == breakpts[i]);
    }

    dualtest.performRatiotestPhase1(15,alpha,7,-55);

//asserting
//    cout<<"t_d: "<<dualtest.getDualSteplength()<<endl;
//    cout<<"t_p: "<<dualtest.getPrimalSteplength()<<endl;
//    cout<<"inc: "<<dualtest.getIncomingVariableIndex()<<endl;
//    cout<<"f1: "<<dualtest.getObjectiveFunctionPhase1()<<endl;

    TEST_ASSERT(dualtest.m_tPositive == false);
    TEST_ASSERT(dualtest.getDualSteplength() == -2);
    TEST_ASSERT(dualtest.getPrimalSteplength() == -2);
    TEST_ASSERT(dualtest.getIncomingVariableIndex() == 7);
    TEST_ASSERT(dualtest.getObjectiveFunctionPhase1() == -51);
}

void DualRatiotestTestSuite::testMaros1Ph1()
{
    Model model;
    Vector alpha(9),reducedCosts(9);

    Numerical::Double alphas[] = { 8, 4, 1, 4, 2,
                                   -1, -1, 1, 1};
    for(int i=0; i<9; i++){
        alpha.set(i,alphas[i]);
    }

    Numerical::Double d[] ={ -24, 2, 0, -8, -1,
                             1, 0, 0, 0};
    for(int i=0; i<9; i++){
        reducedCosts.set(i,d[i]);
    }

    Simplex::FEASIBILITY d_feasibilities[] = {_M_, _P_, _F_,
                                            _M_, _M_, _F_,
                                            _F_, _F_};
    IndexList<> reducedCostFeasibilities;
    reducedCostFeasibilities.init(9,Simplex::FEASIBILITY_ENUM_LENGTH);
    for(int i=0;i<8; i++){
        reducedCostFeasibilities.insert(d_feasibilities[i],i);
    }

    Numerical::Double lowerbounds[] = {-_INF_, -_INF_,
                                       -_INF_, 0, 0, 0, 0, 0};
    Numerical::Double upperbounds[] = {_INF_, _INF_,
                                      _INF_, _INF_,
                                      _INF_, _INF_,
                                      _INF_, _INF_};
    Simplex::VARIABLE_STATE states[] = {_NF_, _NF_,
                                       _NF_, _NLB_,
                                       _NLB_, _NLB_,
                                       _NLB_, _NLB_};

    IndexList<const Numerical::Double*> variableStates;
    variableStates.init(9,Simplex::VARIABLE_STATE_ENUM_LENGTH);
    for(int i=0; i<8; i++){
        initVariable(model,lowerbounds[i],upperbounds[i]);
        variableStates.insert(states[i],i);
    }
    initVariable(model,0,_INF_);
    double val = 16.0;
    variableStates.insert(_B_,8,&val);


    Constraint con;
    model.addConstraint(con,alpha);
    SimplexModel mo(model);
    DualRatiotestUpdater updater(&reducedCostFeasibilities);

//performing
    DualRatiotest dualtest(model,reducedCosts,reducedCostFeasibilities,variableStates,updater);

//generate & test breakpoints
    dualtest.generateBreakpointsPhase1(alpha,10,-35);
    Numerical::Double breakpts[] ={ 0.5, 2, 3, 3, 0, 1, 0};

    for(int i=0;i<dualtest.m_breakpoints.size()-1;i++){
//        cout<<dualtest.m_breakpoints[i].value<<endl;
        TEST_ASSERT(dualtest.m_breakpoints[i+1].value == breakpts[i]);
    }

    dualtest.performRatiotestPhase1(8,alpha,10,-35);

//asserting
//    cout<<"t_d: "<<dualtest.getDualSteplength()<<endl;
//    cout<<"t_p: "<<dualtest.getPrimalSteplength()<<endl;
//    cout<<"inc: "<<dualtest.getIncomingVariableIndex()<<endl;
//    cout<<"f1: "<<dualtest.getObjectiveFunctionPhase1()<<endl;

    TEST_ASSERT(dualtest.m_tPositive == false);
    TEST_ASSERT(dualtest.getDualSteplength() == -3);
    TEST_ASSERT(dualtest.getPrimalSteplength() == 2);
    TEST_ASSERT(dualtest.getIncomingVariableIndex() == 0);
    TEST_ASSERT(dualtest.getObjectiveFunctionPhase1() == -22);
}

void DualRatiotestTestSuite::testMaros2Ph1()
{
    Model model;
    Vector alpha(5),reducedCosts(5);

    Numerical::Double alphas[] = { -3 ,-2, -1, 2, 1};
    for(int i=0; i<5; i++){
        alpha.set(i,alphas[i]);
    }

    Numerical::Double d[] ={ 0, -4, -1, 6, 0};
    for(int i=0; i<5; i++){
        reducedCosts.set(i,d[i]);
    }

    Simplex::FEASIBILITY d_feasibilities[] = {_M_, _M_, _P_};
    IndexList<> reducedCostFeasibilities;
    reducedCostFeasibilities.init(5,Simplex::FEASIBILITY_ENUM_LENGTH);
    for(int i=0;i<3; i++){
        reducedCostFeasibilities.insert(d_feasibilities[i],i+1);
    }

    Numerical::Double lowerbounds[] = {0, 0, 0, -_INF_};
    Numerical::Double upperbounds[] = {_INF_, _INF_,
                                      _INF_, _INF_};
    Simplex::VARIABLE_STATE states[] = {_NLB_, _NLB_,
                                        _NLB_, _NF_};

    IndexList<const Numerical::Double*> variableStates;
    variableStates.init(5,Simplex::VARIABLE_STATE_ENUM_LENGTH);
    for(int i=0; i<4; i++){
        initVariable(model,lowerbounds[i],upperbounds[i]);
        variableStates.insert(states[i],i);
    }
    initVariable(model,0,_INF_);
    double val = 2.0;
    variableStates.insert(_B_,4,&val);

    Constraint con;
    model.addConstraint(con,alpha);
    SimplexModel mo(model);
    DualRatiotestUpdater updater(&reducedCostFeasibilities);

//performing
    DualRatiotest dualtest(model,reducedCosts,reducedCostFeasibilities,variableStates,updater);

//generate & test breakpoints
    dualtest.generateBreakpointsPhase1(alpha,-5,-11);
    Numerical::Double breakpts[] ={ 1, 2, 3, 3};

    for(int i=0;i<dualtest.m_breakpoints.size()-1;i++){
//        cout<<"bpt: "<<dualtest.m_breakpoints[i].value<<endl;
        TEST_ASSERT(dualtest.m_breakpoints[i+1].value == breakpts[i]);
    }

    dualtest.performRatiotestPhase1(4,alpha,-5,-11);

//asserting
//    cout<<"t_d: "<<dualtest.getDualSteplength()<<endl;
//    cout<<"t_p: "<<dualtest.getPrimalSteplength()<<endl;
//    cout<<"inc: "<<dualtest.getIncomingVariableIndex()<<endl;
//    cout<<"f1: "<<dualtest.getObjectiveFunctionPhase1()<<endl;

    TEST_ASSERT(dualtest.m_tPositive == true);
    TEST_ASSERT(dualtest.getDualSteplength() == 3);
    TEST_ASSERT(dualtest.getPrimalSteplength() == 1);
    TEST_ASSERT(dualtest.getIncomingVariableIndex() == 3);
    TEST_ASSERT(dualtest.getObjectiveFunctionPhase1() == 0);
}

void DualRatiotestTestSuite::testStagelCf3Ph1()
{
    Model model;
    Vector alpha(10),reducedCosts(10);

    Numerical::Double alphas[] = { -2, 0, 7, 6, -10,
                                   -2, 0, 4, 5, 1};
    for(int i=0; i<10; i++){
        alpha.set(i,alphas[i]);
    }

    Numerical::Double d[] ={ -8, -1, 0, 18, -10,
                              4, 0, 8, 0, 0};
    for(int i=0; i<10; i++){
        reducedCosts.set(i,d[i]);
    }

    Simplex::FEASIBILITY d_feasibilities[] = {_F_, _M_, _F_,  _P_, _M_,
                                              _P_, _F_, _P_, _F_};
    IndexList<> reducedCostFeasibilities;
    reducedCostFeasibilities.init(10,Simplex::FEASIBILITY_ENUM_LENGTH);
    for(int i=0;i<9; i++){
        reducedCostFeasibilities.insert(d_feasibilities[i],i);
    }

    Numerical::Double lowerbounds[] = {-_INF_, 0, 0, -_INF_, -_INF_,
                                       -_INF_, -_INF_, -_INF_, -_INF_};
    Numerical::Double upperbounds[] = { 0, _INF_, _INF_, _INF_, _INF_,
                                        0, _INF_, _INF_, 0};
    Simplex::VARIABLE_STATE states[] = {_NUB_, _NLB_, _NLB_, _NF_, _NF_,
                                        _NUB_, _NF_, _NF_, _NUB_};

    IndexList<const Numerical::Double*> variableStates;
    variableStates.init(10,Simplex::VARIABLE_STATE_ENUM_LENGTH);
    for(int i=0; i<9; i++){
        initVariable(model,lowerbounds[i],upperbounds[i]);
        variableStates.insert(states[i],i);
    }
    initVariable(model,-_INF_,0 );
    double val = -5.0;
    variableStates.insert(_B_,9,&val);

    Constraint con;
    model.addConstraint(con,alpha);
    SimplexModel mo(model);
    DualRatiotestUpdater updater(&reducedCostFeasibilities);

//performing
    DualRatiotest dualtest(model,reducedCosts,reducedCostFeasibilities,variableStates,updater);

//generate & test breakpoints
    dualtest.generateBreakpointsPhase1(alpha,-18,-41);
    Numerical::Double breakpts[] ={ 1, 1, 2, 2, 3, 3, 0, 4};

    for(int i=0;i<dualtest.m_breakpoints.size()-1;i++){
//        cout<<"bpt: "<<dualtest.m_breakpoints[i].value<<endl;
        TEST_ASSERT(dualtest.m_breakpoints[i+1].value == breakpts[i]);
    }

    dualtest.performRatiotestPhase1(9,alpha,-18,-41);

//asserting
//    cout<<"t_d: "<<dualtest.getDualSteplength()<<endl;
//    cout<<"t_p: "<<dualtest.getPrimalSteplength()<<endl;
//    cout<<"inc: "<<dualtest.getIncomingVariableIndex()<<endl;
//    cout<<"f1: "<<dualtest.getObjectiveFunctionPhase1()<<endl;

    TEST_ASSERT(dualtest.m_tPositive == true);
    TEST_ASSERT(dualtest.getDualSteplength() == 1);
    TEST_ASSERT(dualtest.getPrimalSteplength() == 0.5);
    TEST_ASSERT(dualtest.getIncomingVariableIndex() == 4);
    TEST_ASSERT(dualtest.getObjectiveFunctionPhase1() == -30);
}

void DualRatiotestTestSuite::testStagelPh2(){

}

void DualRatiotestTestSuite::testMaros1Ph2(){

}

void DualRatiotestTestSuite::testMaros2Ph2(){

}
void DualRatiotestTestSuite::testStagelCf3Ph2(){

}

/*
void initilaizeStagelPh2(Model& model,Vector& alpha,Vector& reducedCosts){
    int s=0;
    alpha.set(s,4);s++;
    alpha.set(s,-2);s++;
    alpha.set(s,1);s++;
    alpha.set(s,2);s++;
    alpha.set(s,0);s++;
    alpha.set(s,-4);s++;
    alpha.set(s,2);s++;
    alpha.set(s,-1);s++;
    alpha.set(s,-1);s++;
    alpha.set(s,2);s++;
    alpha.set(s,0);s++;
    alpha.set(s,1);
    s=0;
    reducedCosts.set(s,-2);s++;
    reducedCosts.set(s,2);s++;
    reducedCosts.set(s,3);s++;
    reducedCosts.set(s,-4);s++;
    reducedCosts.set(s,-1);s++;
    reducedCosts.set(s,0);s++;
    reducedCosts.set(s,0);s++;
    reducedCosts.set(s,3);s++;
    reducedCosts.set(s,5);s++;
    reducedCosts.set(s,6);s++;
    reducedCosts.set(s,2);s++;
    reducedCosts.set(s,0);
    s=0;
    initVariable(model,s,0,0,0);s++;
    initVariable(model,s,0,1,0);s++;
    initVariable(model,s,0,2,0);s++;
    initVariable(model,s,0,1,1);s++;
    initVariable(model,s,0,2,2);s++;
    initVariable(model,s,0,2,0);s++;
    initVariable(model,s,0,3,3);s++;
    initVariable(model,s,0,5,0);s++;
    initVariable(model,s,0,Numerical::Infinity,0);s++;
    initVariable(model,s,0,Numerical::Infinity,0);s++;
    initVariable(model,s,-Numerical::Infinity,Numerical::Infinity,0);s++;
    initVariable(model,s,0,Numerical::Infinity,-20);
    model.getVariable(s).setState(Variable::BASIC);
}
void initializeMaros1Ph2(Model& model,Vector& alpha,Vector& reducedCosts){
    int s=0;
    alpha.set(s,2);s++;
    alpha.set(s,-2);s++;
    alpha.set(s,1);s++;
    alpha.set(s,3);s++;
    alpha.set(s,-4);s++;
    alpha.set(s,-1);s++;
    alpha.set(s,1);s++;
    alpha.set(s,-2);s++;
    alpha.set(s,-1);s++;
    alpha.set(s,-2);s++;
    alpha.set(s,1);
    s=0;
    reducedCosts.set(s,-1);s++;
    reducedCosts.set(s,2);s++;
    reducedCosts.set(s,5);s++;
    reducedCosts.set(s,-6);s++;
    reducedCosts.set(s,-2);s++;
    reducedCosts.set(s,0);s++;
    reducedCosts.set(s,0);s++;
    reducedCosts.set(s,0);s++;
    reducedCosts.set(s,4);s++;
    reducedCosts.set(s,10);s++;
    reducedCosts.set(s,0);
    s=0;
    initVariable(model,s,0,0,0);s++;
    initVariable(model,s,0,1,0);s++;
    initVariable(model,s,0,1,0);s++;
    initVariable(model,s,0,1,1);s++;
    initVariable(model,s,0,1,1);s++;
    initVariable(model,s,0,1,0);s++;
    initVariable(model,s,0,2,2);s++;
    initVariable(model,s,0,1,1);s++;
    initVariable(model,s,0,5,0);s++;
    initVariable(model,s,0,Numerical::Infinity,0);s++;
    initVariable(model,s,0,Numerical::Infinity,-11);
    model.getVariable(s).setState(Variable::BASIC);
}
void initializeMaros2Ph2(Model& model,Vector& alpha,Vector& reducedCosts){
    int s=0;
    alpha.set(s,-2);s++;
    alpha.set(s,3);s++;
    alpha.set(s,-2);s++;
    alpha.set(s,-1);s++;
    alpha.set(s,1);s++;
    alpha.set(s,3);s++;
    alpha.set(s,1);
    s=0;
    reducedCosts.set(s,2);s++;
    reducedCosts.set(s,3);s++;
    reducedCosts.set(s,0);s++;
    reducedCosts.set(s,-2);s++;
    reducedCosts.set(s,0);s++;
    reducedCosts.set(s,9);s++;
    reducedCosts.set(s,0);
    s=0;
    initVariable(model,s,0,5,0);s++;
    initVariable(model,s,0,1,0);s++;
    initVariable(model,s,0,1,1);s++;
    initVariable(model,s,0,2,2);s++;
    initVariable(model,s,0,2,0);s++;
    initVariable(model,s,0,Numerical::Infinity,0);s++;
    initVariable(model,s,0,2,14);
    model.getVariable(s).setState(Variable::BASIC);
}
int main(){

//test Maros1 phase 1
    Model modelM1PH1;
    std::vector<Variable> & variablesM1PH1 = modelM1PH1.getVariables();
    variablesM1PH1.resize(9);
    Matrix & matrixM1PH1 = modelM1PH1.getMatrix();
    matrixM1PH1.resize(1,9);
    Vector alphaM1PH1(9),reducedCostsM1PH1(9);
    initializeMaros1Ph1(modelM1PH1,alphaM1PH1,reducedCostsM1PH1);
    Dual_Ratiotest dualtestM1PH1(modelM1PH1);
    cout<<endl<<"test Maros1 phase 1"<<endl<<endl;
    dualtestM1PH1.PerformRatioTestPhase1(8,alphaM1PH1,reducedCostsM1PH1);

//test Maros2 phase 1
      Model modelM2PH1;
      std::vector<Variable> & variablesM2PH1 = modelM2PH1.getVariables();
      variablesM2PH1.resize(5);
      Matrix & matrixM2PH1 = modelM2PH1.getMatrix();
      matrixM2PH1.resize(1,5);
      Vector alphaM2PH1(5),reducedCostsM2PH1(5);
      initializeMaros2Ph1(modelM2PH1,alphaM2PH1,reducedCostsM2PH1);
      Dual_Ratiotest dualtestM2PH1(modelM2PH1);
      cout<<endl<<"test Maros2 phase 1"<<endl<<endl;
      dualtestM2PH1.PerformRatioTestPhase1(4,alphaM2PH1,reducedCostsM2PH1);

//test Stagel phase 2
        Model modelSPH2;
        std::vector<Variable>& variablesSPH2=modelSPH2.getVariables();
        variablesSPH2.resize(12);
        Matrix& matrixSPH2=modelSPH2.getMatrix();
        matrixSPH2.resize(1,12);
        Vector alphaSPH2(12),reducedCostsSPH2(12);
        initilaizeStagelPh2(modelSPH2,alphaSPH2,reducedCostsSPH2);
        Dual_Ratiotest dualtestSPH2(modelSPH2);
        cout<<endl<<"test Stagel phase 2"<<endl<<endl;
        dualtestSPH2.PerformRatiotestPhase2(11,alphaSPH2,reducedCostsSPH2,5);

//test Maros1 phase 2
        Model modelM1PH2;
        std::vector<Variable>& variablesM1PH2=modelM1PH2.getVariables();
        variablesM1PH2.resize(11);
        Matrix& matrixM1PH2=modelM1PH2.getMatrix();
        matrixM1PH2.resize(1,11);
        Vector alphaM1PH2(11),reducedCostsM1PH2(11);
        initializeMaros1Ph2(modelM1PH2,alphaM1PH2,reducedCostsM1PH2);
        Dual_Ratiotest dualtestM1PH2(modelM1PH2);
        cout<<endl<<"test Maros 1 phase 2"<<endl<<endl;
        dualtestM1PH2.PerformRatiotestPhase2(10,alphaM1PH2,reducedCostsM1PH2,1);

//test Maros2 phase 2
        Model modelM2PH2;
        std::vector<Variable>& variablesM2PH2=modelM2PH2.getVariables();
        variablesM2PH2.resize(7);
        Matrix& matrixM2PH2=modelM2PH2.getMatrix();
        matrixM2PH2.resize(1,7);
        Vector alphaM2PH2(7),reducedCostsM2PH2(7);
        initializeMaros2Ph2(modelM2PH2,alphaM2PH2,reducedCostsM2PH2);
        Dual_Ratiotest dualtestM2PH2(modelM2PH2);
        cout<<endl<<"test Maros2 phase 2"<<endl<<endl;
        dualtestM2PH2.PerformRatiotestPhase2(6,alphaM2PH2,reducedCostsM2PH2,6);
}
*/
