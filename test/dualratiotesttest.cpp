#include <test/dualratiotesttest.h>
#include <indexlisttest.h>
#include <linalg/matrix.h>
#include <simplex/simplex.h>

#include <iostream>
using namespace std;

DualRatiotestTestSuite::DualRatiotestTestSuite(const char * name):UnitTest(name)
{
    ADD_TEST(DualRatiotestTestSuite::testStagelPh1);
//    ADD_TEST(DualRatiotestTestSuite::testMaros1Ph1);
//    ADD_TEST(DualRatiotestTestSuite::testMaros2Ph1);
//    ADD_TEST(DualRatiotestTestSuite::testStagelPh2);
//    ADD_TEST(DualRatiotestTestSuite::testMaros1Ph2);
//    ADD_TEST(DualRatiotestTestSuite::testMaros2Ph2);
}

void DualRatiotestTestSuite::initVariable(Model& model,
                                          Numerical::Double lb,
                                          Numerical::Double ub){
    Variable var;
    var.setLowerBound(lb);
    var.setUpperBound(ub);

    model.m_variables.push_back(var);
}

void DualRatiotestTestSuite::testStagelPh1()
{
//init
    Model model;
    Vector alpha(16),reducedCosts(16);
    int s=0;
    Numerical::Double alphas[] = { 3, 7, 0, 1, 2,-1,-2,-8, 3,-2,
                                   2, 4, 0, 0, 1, 1};
    for(int i = 0; i < (sizeof(alphas)/sizeof(Numerical::Double)) ;i++){
        alpha.set(i, alphas[i]);
    }
    Numerical::Double d[] = { -12, 14, -6, 0, -1, 1, -4, 16, 0, -5,
                                  6, -12, 0, 2, 3, 0};

    Simplex::FEASIBILITY d_feasibilities[] = {Simplex::MINUS, Simplex::FEASIBLE, Simplex::MINUS,
                                            Simplex::FEASIBLE, Simplex::MINUS, Simplex::PLUS,
                                            Simplex::FEASIBLE, Simplex::MINUS, Simplex::PLUS,
                                            Simplex::MINUS, Simplex::FEASIBLE, Simplex::PLUS,
                                            Simplex::PLUS};
    IndexList<> reducedCostFeasibilities;
    reducedCostFeasibilities.init(16,Simplex::FEASIBILITY_ENUM_LENGTH);
    for(int i=0;i<13;i++){
        reducedCostFeasibilities.insert(d_feasibilities[i],i+2);
    }

    for(int i=0;i < (sizeof(d)/sizeof(Numerical::Double));i++){
        reducedCosts.set(i, d[i]);
    }
    Numerical::Double lowerbounds[] = { 0, 0, 0, 0, 0, 0, 0,-Numerical::Infinity,
                                     -Numerical::Infinity,-Numerical::Infinity,
                                     -Numerical::Infinity,-Numerical::Infinity,
                                     -Numerical::Infinity,-Numerical::Infinity,
                                     -Numerical::Infinity};
    Numerical::Double upperbounds[] = { 0, 5, Numerical::Infinity, Numerical::Infinity,
                                    Numerical::Infinity,Numerical::Infinity,Numerical::Infinity,
                                    Numerical::Infinity,Numerical::Infinity,Numerical::Infinity,
                                    Numerical::Infinity,Numerical::Infinity,Numerical::Infinity,
                                    Numerical::Infinity,Numerical::Infinity};
    Simplex::VARIABLE_STATE varstates[] ={ Simplex::NONBASIC_AT_LB, Simplex::NONBASIC_AT_LB, Simplex::NONBASIC_AT_LB,
                                           Simplex::NONBASIC_AT_LB,Simplex::NONBASIC_AT_LB, Simplex::NONBASIC_AT_LB,
                                           Simplex::NONBASIC_AT_LB, Simplex::NONBASIC_FREE, Simplex::NONBASIC_FREE,
                                           Simplex::NONBASIC_FREE, Simplex::NONBASIC_FREE, Simplex::NONBASIC_FREE,
                                           Simplex::NONBASIC_FREE,Simplex::NONBASIC_FREE, Simplex::NONBASIC_FREE,
                                         };
    IndexList<const Numerical::Double*> variableStates;
    variableStates.init(16,Simplex::VARIABLE_STATE_ENUM_LENGTH);

    for(int i=0; i < (sizeof(varstates)/sizeof(Simplex::VARIABLE_STATE)) ;i++){
        initVariable(model,lowerbounds[i],upperbounds[i]);
        variableStates.insert(varstates[i],i);
    }
    double var = 5.0;
    initVariable(model,0,Numerical::Infinity);
    variableStates.insert(Simplex::BASIC,15,&var);

    Constraint con;
    model.addConstraint(con,alpha);
    SimplexModel mo(model);
    DualRatiotestUpdater updater(&reducedCostFeasibilities);

//performing
    DualRatiotest dualtest(mo,reducedCosts,reducedCostFeasibilities,variableStates,updater);

    dualtest.generateBreakpointsPhase1(alpha,7,-55);
    Numerical::Double breakpts[] = { 0.5, 1, 2, 2, 0, 3, 3};
    for(int i = 1 ; i < (sizeof(breakpts)/sizeof(Numerical::Double)); i++){
        cout<<"BreakPoints["<<i<<"]: "<<dualtest.m_breakpoints[i]<<endl;
//        TEST_ASSERT(dualtest.m_breakpoints[i] == breakpts[i] );
    }

    dualtest.performRatiotestPhase1(15,alpha,7,-55);
//asserting
    cout<<"t_d: "<<dualtest.getDualSteplength()<<endl;
    TEST_ASSERT(dualtest.getDualSteplength() == -2);
    cout<<"t_p: "<<dualtest.getPrimalSteplength()<<endl;
    TEST_ASSERT(dualtest.getPrimalSteplength() == -2.5);
    cout<<"inc: "<<dualtest.getIncomingVariableIndex()<<endl;
    TEST_ASSERT(dualtest.getIncomingVariableIndex() == 7);
    cout<<"f1: "<<dualtest.getObjectiveFunctionPhase1()<<endl;
    TEST_ASSERT(dualtest.getObjectiveFunctionPhase1() == -47.5);
}

void DualRatiotestTestSuite::testMaros1Ph1()
{
    Model model;
    Vector alpha(9),reducedCosts(9);

    Numerical::Double alphas[] = { 8, 4, 1, 4, 2, -1, -1, 1, 1};
    for(int i=0; i<(sizeof(alphas)/sizeof(Numerical::Double)); i++){
        alpha.set(i,alphas[i]);
    }

    Numerical::Double d[] ={ -24, 2, 0, -8, -1, 1, 0, 0, 0};
    for(int i=0; i<(sizeof(d)/sizeof(Numerical::Double)); i++){
        reducedCosts.set(i,d[i]);
    }

    Numerical::Double lowerbounds[] = {-Numerical::Infinity, -Numerical::Infinity,
                                       -Numerical::Infinity, 0, 0, 0, 0, 0, 0};
    Numerical::Double upperbounds[] = {Numerical::Infinity, Numerical::Infinity,
                                      Numerical::Infinity, Numerical::Infinity,
                                      Numerical::Infinity, Numerical::Infinity,
                                      Numerical::Infinity, Numerical::Infinity,
                                      Numerical::Infinity};
    Simplex::VARIABLE_STATE states[] = {Simplex::NONBASIC_FREE, Simplex::NONBASIC_FREE,
                                       Simplex::NONBASIC_FREE, Simplex::NONBASIC_AT_LB,
                                       Simplex::NONBASIC_AT_LB, Simplex::NONBASIC_AT_LB,
                                       Simplex::NONBASIC_AT_LB, Simplex::NONBASIC_AT_LB,
                                       Simplex::NONBASIC_AT_LB};

    IndexList<const Numerical::Double*> variableStates;
    variableStates.init(9,Simplex::VARIABLE_STATE_ENUM_LENGTH);
    for(int i=0; i<(sizeof(states)/sizeof(Simplex::VARIABLE_STATE)); i++){
        initVariable(model,lowerbounds[i],upperbounds[i]);
    }
    initVariable(model,0,Numerical::Infinity);
    double val = 16.0;
    variableStates.insert(Simplex::BASIC,9,&val);

    Simplex::FEASIBILITY d_feasibilities[] = {Simplex::MINUS, Simplex::PLUS, Simplex::FEASIBLE,
                                            Simplex::MINUS, Simplex::MINUS, Simplex::FEASIBLE,
                                            Simplex::FEASIBLE, Simplex::FEASIBLE};
    IndexList<> reducedCostFeasibilities;
    reducedCostFeasibilities.init(9,Simplex::FEASIBILITY_ENUM_LENGTH);
    for(int i=0;i<(sizeof(d_feasibilities)/sizeof(Simplex::FEASIBILITY)); i++){
        reducedCostFeasibilities.insert(d_feasibilities[i],i);
    }

    Constraint con;
    model.addConstraint(con,alpha);
    SimplexModel mo(model);
    DualRatiotestUpdater updater(&reducedCostFeasibilities);

//performing
    DualRatiotest dualtest(model,reducedCosts,reducedCostFeasibilities,variableStates,updater);
    dualtest.performRatiotestPhase1(9,alpha,10,-35);

//asserting
    cout<<"t_d: "<<dualtest.getDualSteplength()<<endl;
    TEST_ASSERT(dualtest.getDualSteplength() == -3);
    cout<<"t_p: "<<dualtest.getPrimalSteplength()<<endl;
    TEST_ASSERT(dualtest.getPrimalSteplength() == 2);
    cout<<"inc: "<<dualtest.getIncomingVariableIndex()<<endl;
    TEST_ASSERT(dualtest.getIncomingVariableIndex() == 0);
    cout<<"f1: "<<dualtest.getObjectiveFunctionPhase1()<<endl;
    TEST_ASSERT(dualtest.getObjectiveFunctionPhase1() == -22);

}

void DualRatiotestTestSuite::testMaros2Ph1()
{

}

void DualRatiotestTestSuite::testStagelPh2()
{

}

void DualRatiotestTestSuite::testMaros1Ph2()
{

}

void DualRatiotestTestSuite::testMaros2Ph2()
{

}
/*
void initializeMaros1Ph1(Model& model,Vector& alpha,Vector& reducedCosts){
}
void initializeMaros2Ph1(Model& model,Vector& alpha,Vector& reducedCosts){

    int s=0;
    alpha.set(s,-2);s++;
    alpha.set(s,-3);s++;
    alpha.set(s,-1);s++;
    alpha.set(s,2);s++;//4
    alpha.set(s,1);
    s=0;
    reducedCosts.set(s,-4);s++;
    reducedCosts.set(s,0);s++;
    reducedCosts.set(s,-1);s++;
    reducedCosts.set(s,6);s++;//4
    reducedCosts.set(s,0);
    s=0;
    initVariable(model,s,0,Numerical::Infinity,0);s++;
    initVariable(model,s,0,Numerical::Infinity,0);s++;
    initVariable(model,s,0,Numerical::Infinity,0);s++;
    initVariable(model,s,-Numerical::Infinity,Numerical::Infinity,0);s++;//4
    initVariable(model,s,0,Numerical::Infinity,4);
    model.getVariable(s).setState(Variable::BASIC);
}
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
