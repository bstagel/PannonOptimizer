#include <iostream>
#include <lp/model.h>
#include <linalg/matrix.h>
#include <simplex/dual_ratiotest.h>
#include <lp/mpsproblem.h>

using namespace std;

void initVar(Model& model,int i,Numerical::Double lb,Numerical::Double ub,Numerical::Double val){
    model.getVariable(i).setLowerBound(lb);
    model.getVariable(i).setUpperBound(ub);
    model.getVariable(i).setValue(val);

    if(lb==val){
        model.getVariable(i).setState(Variable::NONBASIC_AT_LB);
    }else
     if(ub==val){
        model.getVariable(i).setState(Variable::NONBASIC_AT_UB);
     }
}

void initializeStagelPh1(Model& model,Vector& alpha,Vector& redcost){
    int s=0;
    alpha.set(s,3);s++;
    alpha.set(s,7);s++;
    alpha.set(s,0);s++;
    alpha.set(s,1);s++;//4
    alpha.set(s,2);s++;
    alpha.set(s,-1);s++;
    alpha.set(s,-2);s++;
    alpha.set(s,-8);s++;//8
    alpha.set(s,3);s++;
    alpha.set(s,-2);s++;
    alpha.set(s,2);s++;
    alpha.set(s,4);s++;//12
    alpha.set(s,0);s++;
    alpha.set(s,0);s++;
    alpha.set(s,1);s++;
    alpha.set(s,1);//16
    s=0;
    redcost.set(s,-12);s++;
    redcost.set(s,14);s++;
    redcost.set(s,-6);s++;
    redcost.set(s,0);s++;//4
    redcost.set(s,-1);s++;
    redcost.set(s,1);s++;
    redcost.set(s,-4);s++;
    redcost.set(s,16);s++;//8
    redcost.set(s,0);s++;
    redcost.set(s,-5);s++;
    redcost.set(s,6);s++;
    redcost.set(s,-12);s++;//12
    redcost.set(s,0);s++;
    redcost.set(s,2);s++;
    redcost.set(s,3);s++;
    redcost.set(s,0);//16
    s=0;
    initVar(model,s,0,0,0);s++;
    initVar(model,s,0,5,0);s++;
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,0,infinity,0);s++;//4
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,-infinity,infinity,0);s++;//8
    initVar(model,s,-infinity,infinity,0);s++;
    initVar(model,s,-infinity,infinity,0);s++;
    initVar(model,s,-infinity,infinity,0);s++;
    initVar(model,s,-infinity,infinity,0);s++;//12
    initVar(model,s,-infinity,infinity,0);s++;
    initVar(model,s,-infinity,infinity,0);s++;
    initVar(model,s,-infinity,infinity,0);s++;
    initVar(model,s,0,infinity,5);//16
    model.getVariable(s).setState(Variable::BASIC);
}
void initializeMaros1Ph1(Model& model,Vector& alpha,Vector& redcost){
    int s=0;
    alpha.set(s,8);s++;
    alpha.set(s,4);s++;
    alpha.set(s,1);s++;
    alpha.set(s,4);s++;//4
    alpha.set(s,2);s++;
    alpha.set(s,-1);s++;
    alpha.set(s,-1);s++;
    alpha.set(s,1);s++;//8
    alpha.set(s,1);
    s=0;
    redcost.set(s,-24);s++;
    redcost.set(s,2);s++;
    redcost.set(s,0);s++;
    redcost.set(s,-8);s++;//4
    redcost.set(s,-1);s++;
    redcost.set(s,1);s++;
    redcost.set(s,0);s++;
    redcost.set(s,0);s++;//8
    redcost.set(s,0);
    s=0;
    initVar(model,s,-infinity,infinity,0);s++;
    initVar(model,s,-infinity,infinity,0);s++;
    initVar(model,s,-infinity,infinity,0);s++;
    initVar(model,s,0,infinity,0);s++;//4
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,0,infinity,0);s++;//8
    initVar(model,s,0,infinity,5);
    model.getVariable(s).setState(Variable::BASIC);
}
void initializeMaros2Ph1(Model& model,Vector& alpha,Vector& redcost){

    int s=0;
    alpha.set(s,-2);s++;
    alpha.set(s,-3);s++;
    alpha.set(s,-1);s++;
    alpha.set(s,2);s++;//4
    alpha.set(s,1);
    s=0;
    redcost.set(s,-4);s++;
    redcost.set(s,0);s++;
    redcost.set(s,-1);s++;
    redcost.set(s,6);s++;//4
    redcost.set(s,0);
    s=0;
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,-infinity,infinity,0);s++;//4
    initVar(model,s,0,infinity,4);
    model.getVariable(s).setState(Variable::BASIC);
}
void initilaizeStagelPh2(Model& model,Vector& alpha,Vector& redcost){
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
    redcost.set(s,-2);s++;
    redcost.set(s,2);s++;
    redcost.set(s,3);s++;
    redcost.set(s,-4);s++;
    redcost.set(s,-1);s++;
    redcost.set(s,0);s++;
    redcost.set(s,0);s++;
    redcost.set(s,3);s++;
    redcost.set(s,5);s++;
    redcost.set(s,6);s++;
    redcost.set(s,2);s++;
    redcost.set(s,0);
    s=0;
    initVar(model,s,0,0,0);s++;
    initVar(model,s,0,1,0);s++;
    initVar(model,s,0,2,0);s++;
    initVar(model,s,0,1,1);s++;
    initVar(model,s,0,2,2);s++;
    initVar(model,s,0,2,0);s++;
    initVar(model,s,0,3,3);s++;
    initVar(model,s,0,5,0);s++;
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,-infinity,infinity,0);s++;
    initVar(model,s,0,infinity,-20);
    model.getVariable(s).setState(Variable::BASIC);
}
void initializeMaros1Ph2(Model& model,Vector& alpha,Vector& redcost){
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
    redcost.set(s,-1);s++;
    redcost.set(s,2);s++;
    redcost.set(s,5);s++;
    redcost.set(s,-6);s++;
    redcost.set(s,-2);s++;
    redcost.set(s,0);s++;
    redcost.set(s,0);s++;
    redcost.set(s,0);s++;
    redcost.set(s,4);s++;
    redcost.set(s,10);s++;
    redcost.set(s,0);
    s=0;
    initVar(model,s,0,0,0);s++;
    initVar(model,s,0,1,0);s++;
    initVar(model,s,0,1,0);s++;
    initVar(model,s,0,1,1);s++;
    initVar(model,s,0,1,1);s++;
    initVar(model,s,0,1,0);s++;
    initVar(model,s,0,2,2);s++;
    initVar(model,s,0,1,1);s++;
    initVar(model,s,0,5,0);s++;
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,0,infinity,-11);
    model.getVariable(s).setState(Variable::BASIC);
}
void initializeMaros2Ph2(Model& model,Vector& alpha,Vector& redcost){
    int s=0;
    alpha.set(s,-2);s++;
    alpha.set(s,3);s++;
    alpha.set(s,-2);s++;
    alpha.set(s,-1);s++;
    alpha.set(s,1);s++;
    alpha.set(s,3);s++;
    alpha.set(s,1);
    s=0;
    redcost.set(s,2);s++;
    redcost.set(s,3);s++;
    redcost.set(s,0);s++;
    redcost.set(s,-2);s++;
    redcost.set(s,0);s++;
    redcost.set(s,9);s++;
    redcost.set(s,0);
    s=0;
    initVar(model,s,0,5,0);s++;
    initVar(model,s,0,1,0);s++;
    initVar(model,s,0,1,1);s++;
    initVar(model,s,0,2,2);s++;
    initVar(model,s,0,2,0);s++;
    initVar(model,s,0,infinity,0);s++;
    initVar(model,s,0,2,14);
    model.getVariable(s).setState(Variable::BASIC);
}

int main(){
//    MpsProblem problem;
//    const char* file="pelda.mps";
//    problem.loadFromFile(file);
//    problem.buildModel(&model);

//test Stagel phase 1
    Model modelSPH1;
    std::vector <Variable> & variablesSPH1= modelSPH1.getVariables();
    variablesSPH1.resize(16);
    Matrix & matrixSPH1 = modelSPH1.getMatrix();
    matrixSPH1.resize(1,16);
    Vector alphaSPH1(16),redcostSPH1(16);
    initializeStagelPh1(modelSPH1,alphaSPH1,redcostSPH1);
    Dual_Ratiotest dualtestSPH1(modelSPH1);
    cout<<endl<<"test Stagel phase 1"<<endl<<endl;
    dualtestSPH1.PerformRatioTestPhase1(15,alphaSPH1,redcostSPH1);

//test Maros1 phase 1
    Model modelM1PH1;
    std::vector<Variable> & variablesM1PH1 = modelM1PH1.getVariables();
    variablesM1PH1.resize(9);
    Matrix & matrixM1PH1 = modelM1PH1.getMatrix();
    matrixM1PH1.resize(1,9);
    Vector alphaM1PH1(9),redcostM1PH1(9);
    initializeMaros1Ph1(modelM1PH1,alphaM1PH1,redcostM1PH1);
    Dual_Ratiotest dualtestM1PH1(modelM1PH1);
    cout<<endl<<"test Maros1 phase 1"<<endl<<endl;
    dualtestM1PH1.PerformRatioTestPhase1(8,alphaM1PH1,redcostM1PH1);

//test Maros2 phase 1
      Model modelM2PH1;
      std::vector<Variable> & variablesM2PH1 = modelM2PH1.getVariables();
      variablesM2PH1.resize(5);
      Matrix & matrixM2PH1 = modelM2PH1.getMatrix();
      matrixM2PH1.resize(1,5);
      Vector alphaM2PH1(5),redcostM2PH1(5);
      initializeMaros2Ph1(modelM2PH1,alphaM2PH1,redcostM2PH1);
      Dual_Ratiotest dualtestM2PH1(modelM2PH1);
      cout<<endl<<"test Maros2 phase 1"<<endl<<endl;
      dualtestM2PH1.PerformRatioTestPhase1(4,alphaM2PH1,redcostM2PH1);

//test Stagel phase 2
        Model modelSPH2;
        std::vector<Variable>& variablesSPH2=modelSPH2.getVariables();
        variablesSPH2.resize(12);
        Matrix& matrixSPH2=modelSPH2.getMatrix();
        matrixSPH2.resize(1,12);
        Vector alphaSPH2(12),redcostSPH2(12);
        initilaizeStagelPh2(modelSPH2,alphaSPH2,redcostSPH2);
        Dual_Ratiotest dualtestSPH2(modelSPH2);
        cout<<endl<<"test Stagel phase 2"<<endl<<endl;
        dualtestSPH2.PerformRatiotestPhase2(11,alphaSPH2,redcostSPH2,5);

//test Maros1 phase 2
        Model modelM1PH2;
        std::vector<Variable>& variablesM1PH2=modelM1PH2.getVariables();
        variablesM1PH2.resize(11);
        Matrix& matrixM1PH2=modelM1PH2.getMatrix();
        matrixM1PH2.resize(1,11);
        Vector alphaM1PH2(11),redcostM1PH2(11);
        initializeMaros1Ph2(modelM1PH2,alphaM1PH2,redcostM1PH2);
        Dual_Ratiotest dualtestM1PH2(modelM1PH2);
        cout<<endl<<"test Maros 1 phase 2"<<endl<<endl;
        dualtestM1PH2.PerformRatiotestPhase2(10,alphaM1PH2,redcostM1PH2,1);

//test Maros2 phase 2
        Model modelM2PH2;
        std::vector<Variable>& variablesM2PH2=modelM2PH2.getVariables();
        variablesM2PH2.resize(7);
        Matrix& matrixM2PH2=modelM2PH2.getMatrix();
        matrixM2PH2.resize(1,7);
        Vector alphaM2PH2(7),redcostM2PH2(7);
        initializeMaros2Ph2(modelM2PH2,alphaM2PH2,redcostM2PH2);
        Dual_Ratiotest dualtestM2PH2(modelM2PH2);
        cout<<endl<<"test Maros2 phase 2"<<endl<<endl;
        dualtestM2PH2.PerformRatiotestPhase2(6,alphaM2PH2,redcostM2PH2,6);
}
