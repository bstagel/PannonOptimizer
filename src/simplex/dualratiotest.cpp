/**
 * @file dualratiotest.cpp
 */

#include <simplex/dualratiotest.h>
#include <simplex/simplexparameterhandler.h>
#include <lp/model.h>
#include <utility>
#include <simplex/simplex.h>
#include <globals.h>
#include <simplex/simplexparameterhandler.h>
#include <utils/thirdparty/prettyprint.h>

static const int nonlinearDualPhaseIFunction =
        SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_dual_phaseI_function");
static const int nonlinearDualPhaseIIFunction =
        SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_dual_phaseII_function");
static const int thresholdReportLevel =
        SimplexParameterHandler::getInstance().getIntegerParameterValue("threshold_report_level");
static const Numerical::Double optimalityTolerance =
        SimplexParameterHandler::getInstance().getDoubleParameterValue("e_optimality");

//static const int expandDualPhaseII =
//        SimplexParameterHandler::getInstance().getIntegerParameterValue("expand_dual_phaseII");

DualRatiotest::DualRatiotest(const SimplexModel & model,
                             const Vector& reducedCosts,
                             const IndexList<>& reducedCostFeasibilities,
                             const IndexList<const Numerical::Double*>& variableStates,
                             DualRatiotestUpdater &dualRatiotestUpdater) :
    m_model(model),
    m_reducedCosts(reducedCosts),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_variableStates(variableStates),
    m_tPositive(false),
    m_transform(false),
    m_variableAge(model.getColumnCount() + model.getRowCount(),1),
    m_ageStep(1),
    m_dualRatiotestUpdater(dualRatiotestUpdater),
    m_incomingVariableIndex(-1),
    m_dualSteplength(0),
    m_phaseIObjectiveValue(0),
    m_phaseIIObjectiveValue(0),
    m_pivotThreshold(SimplexParameterHandler::getInstance().getDoubleParameterValue("e_pivot"))
{
}

void DualRatiotest::shift(std::vector<BreakPoint>* breakpoints, unsigned int startId, unsigned int stopId) {
    unsigned int i=startId,  j=2*i+1;
    BreakPoint elementToShift = (*breakpoints)[startId];

//choosing smaller son
    if (j < stopId && (*breakpoints)[j+1].value < (*breakpoints)[j].value) j++;
//shifting
    while (j <= stopId && (*breakpoints)[j].value < elementToShift.value) {
        (*breakpoints)[i] = (*breakpoints)[j];
        i = j;
        j = 2*i + 1;
        if ( j < stopId && (*breakpoints)[j+1].value < (*breakpoints)[j].value ) j++;
    };
    (*breakpoints)[i] = elementToShift;
}

//getting smallest element considering the given length
void DualRatiotest::getNextElement(std::vector<BreakPoint>* breakpoints, unsigned int length) {
    BreakPoint temp;

//creating the heap
    if ( length >= 2) {
        for (int i = (length-2)/2; i>=0; i--) {
            shift(breakpoints,i,length-1);
        }
    }

    if(length<1){
        LPERROR("HEAP ERROR");
    }

    if(length != 1){
        temp = (*breakpoints)[0];
        (*breakpoints)[0] = (*breakpoints)[length-1];
        (*breakpoints)[length-1] = temp;
    }
}

//returns false if alpha is numerical good AND not fake feasible
bool DualRatiotest::numericalCheckPhase1(const Vector& alpha, unsigned int alphaId)
{
    unsigned variableId = m_breakpoints[alphaId].index;

    if ( Numerical::fabs(alpha.at(variableId)) > m_pivotThreshold)
    {
        return false;
//        if (expandDualPhaseI == 1)
//        {
//            return false;
//        }
//        //in F
//        if (m_reducedCostFeasibilities.where(variableId) == Simplex::FEASIBLE){
//            //F->P
//            if ( (m_model.getVariable(variableId).getType() == Variable::MINUS ||
//                m_model.getVariable(variableId).getType() == Variable::FREE) &&
//                ( (m_tPositive && alpha.at(variableId) < 0) || (!m_tPositive && alpha.at(variableId) > 0) ) )
//            {
//                if (m_reducedCosts.at(variableId) > 0) {
//                    LPERROR("variable "<<variableId<<" fake feasible d= "<<m_reducedCosts.at(variableId));
//                    return true;
//                }
//            }
//            //F->M
//            if ( (m_model.getVariable(variableId).getType() == Variable::PLUS ||
//                m_model.getVariable(variableId).getType() == Variable::FREE) &&
//                ( (m_tPositive && alpha.at(variableId) > 0) || (!m_tPositive && alpha.at(variableId) < 0) ) )
//            {
//                if (m_reducedCosts.at(variableId) < 0) {
//                    LPERROR("variable "<<variableId<<" fake feasible d= "<<m_reducedCosts.at(variableId));
//                    return true;
//                }
//            }
//        }
    }else{
        return true;
    }
    return false;
}

void DualRatiotest::generateBreakpointsPhase1(const Vector& alpha,
                                              Numerical::Double phaseIReducedCost){
    #ifndef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    m_dualRatiotestUpdater.m_updateVector.clear();
    m_dualRatiotestUpdater.m_updateVector.reserve(m_model.getMatrix().columnCount() + m_model.getMatrix().rowCount());

//determining t>=0 or t<=0 cases
    if (phaseIReducedCost > 0) {
        m_tPositive = false;
    } else{
        m_tPositive = true;
    }

//    LPINFO("PH1: "<<phaseIReducedCost);
//computing ratios
    m_breakpoints.clear();
    m_breakpoints.reserve(alpha.nonZeros()*2);
    BreakPoint currentRatio;
    currentRatio.functionValue = m_phaseIObjectiveValue;
    currentRatio.index = -1;
    currentRatio.value = 0;
    m_breakpoints.push_back(currentRatio);
    currentRatio.functionValue = 0;
    IndexList<>::Iterator it;
    IndexList<>::Iterator endit;
    unsigned int variableIndex = 0;
    Variable::VARIABLE_TYPE typeOfIthVariable;

//t>=0 case
    if (m_tPositive) {

    //computing ratios in M
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::MINUS);

        for (; it!=endit; it++){
            variableIndex = it.getData();
            if (alpha.at(variableIndex) < 0) {
                currentRatio.index = variableIndex;
                currentRatio.value = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                m_breakpoints.push_back(currentRatio);
                m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                    m_breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                }
            }
        }

    //computing ratios in P
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::PLUS);

        for (; it!=endit; it++){
            variableIndex = it.getData();
            if (alpha.at(variableIndex) > 0){
                currentRatio.index = variableIndex;
                currentRatio.value = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                m_breakpoints.push_back(currentRatio);
                m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                    m_breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                }
             }
        }

    //computing ratios in F
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);

        for (; it!=endit; it++){
            variableIndex = it.getData();
            typeOfIthVariable = m_model.getVariable(variableIndex).getType();
            if ( alpha.at(variableIndex) != 0 ) {
                currentRatio.index = variableIndex;
                currentRatio.value = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
            }

            if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) > 0) {
                m_breakpoints.push_back(currentRatio);
                m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
            } else
            if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) < 0) {
                m_breakpoints.push_back(currentRatio);
                m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
            }

            if (typeOfIthVariable == Variable::FREE && alpha.at(variableIndex) > 0) {
                m_breakpoints.push_back(currentRatio);
                m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
            } else
            if (typeOfIthVariable == Variable::FREE && alpha.at(variableIndex) < 0) {
                m_breakpoints.push_back(currentRatio);
                m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
            }
        }

//t<=0 case
    } else{

    //computing ratios in M
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::MINUS);

            for (; it!=endit; it++) {
                variableIndex = it.getData();
                if (alpha.at(variableIndex) > 0) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                    m_breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                    if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                        m_breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                    }
                }
            }

    //computing ratios in P
            m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::PLUS);

            for (; it!=endit; it++) {
                variableIndex = it.getData();
                if (alpha.at(variableIndex) < 0) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                    m_breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                    if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                        m_breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                    }
                }
            }

    //computing ratios in F
            m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);

            for (; it!=endit; it++) {
                variableIndex = it.getData();
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();
                if (alpha.at(variableIndex) != 0 ) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                }

                if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) < 0) {
                    m_breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                } else
                if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) > 0) {
                    m_breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                }

                if (typeOfIthVariable == Variable::FREE && alpha.at(variableIndex) < 0) {
                    m_breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                } else
                if (typeOfIthVariable == Variable::FREE && alpha.at(variableIndex) > 0) {
                    m_breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                }
            }
    }
}

void DualRatiotest::computeFunctionPhase1(const Vector& alpha,
                                          unsigned int& iterationCounter,
                                          Numerical::Double& functionSlope)
{
    unsigned int length = m_breakpoints.size();
    while (functionSlope >= 0 && iterationCounter < length-1) {
        iterationCounter++;
        getNextElement(&m_breakpoints,length-iterationCounter);

        m_phaseIObjectiveValue += functionSlope * (m_breakpoints.at(length-1-iterationCounter).value -
                m_breakpoints[length-iterationCounter].value);
        m_breakpoints.at(length-1-iterationCounter).functionValue = m_phaseIObjectiveValue;
        functionSlope -= Numerical::fabs(alpha.at(m_breakpoints.at(length-1-iterationCounter).index));
    }
    m_dualSteplength = m_tPositive ? m_breakpoints[length-iterationCounter-1].value :
            - m_breakpoints[length-iterationCounter-1].value;
    m_incomingVariableIndex = m_breakpoints.at(length-1-iterationCounter).index;
}

void DualRatiotest::useNumericalThresholdPhase1(unsigned int iterationCounter,
                                          const Vector& alpha,
                                          Numerical::Double& functionSlope)
{
    unsigned int length = m_breakpoints.size(),
    alphaId = length-1-iterationCounter, prevAlphaId = alphaId, nextAlphaId = alphaId,
    prevIterationCounter = 0, nextIterationCounter = 0;

    if (thresholdReportLevel > 0.0){
        if(m_breakpoints[alphaId].index != -1){
        LPWARNING("BAD NUMERICAL VALUE IN PHASE I,  ACTIVATING THRESHOLD for"
                                              "variable " << m_breakpoints[alphaId].index
                                              <<" with alpha" <<alpha.at(m_breakpoints[alphaId].index)
                                              <<" at f= "<<m_breakpoints[alphaId].functionValue);
        } else {
            LPWARNING("BAD NUMERICAL VALUE IN PHASE I for the INITIAL BREAKPOINT (-1)");
        }
    }
    Numerical::Double prevObjValue = m_phaseIObjectiveValue, nextObjValue = m_phaseIObjectiveValue;
    if (iterationCounter > 0) {
        prevAlphaId++;
        prevIterationCounter++;
        prevObjValue = m_breakpoints[prevAlphaId].functionValue;
        if (thresholdReportLevel > 1.0) LPWARNING("previous step f: "<<prevObjValue);
    }
    if (iterationCounter < length-1) {
        nextAlphaId--;
        nextIterationCounter++;
        getNextElement(&m_breakpoints, length - (iterationCounter+nextIterationCounter));
        nextObjValue += functionSlope * (m_breakpoints[length-1-(iterationCounter + nextIterationCounter)].value -
                m_breakpoints[length-(iterationCounter + nextIterationCounter)].value);
        m_breakpoints[length-1-(iterationCounter + nextIterationCounter)].functionValue = nextObjValue;
        functionSlope -= Numerical::fabs(alpha.at(m_breakpoints[nextAlphaId].index));
        if(nextObjValue < m_breakpoints[length-1].functionValue){
            nextObjValue = -Numerical::Infinity;
        }
        if (thresholdReportLevel > 1.0) LPWARNING("next step f: "<<nextObjValue);
    } else{
        nextObjValue = -Numerical::Infinity;
    }
    bool prevIsBetter = nextObjValue > prevObjValue ? false : true;
    bool step = true;

    while (step &&( prevIsBetter ? numericalCheckPhase1(alpha, prevAlphaId) : numericalCheckPhase1(alpha, nextAlphaId))) {
        step = false;
        if (prevIsBetter) {
            if (iterationCounter - prevIterationCounter > 0) {
                if (thresholdReportLevel > 1.0) LPWARNING("previous step f: "<<prevObjValue);
                prevAlphaId++;
                prevIterationCounter++;
                prevObjValue = m_breakpoints[prevAlphaId].functionValue;
                step = true;
            } else if ( prevObjValue != -Numerical::Infinity) {
                prevObjValue = -Numerical::Infinity;
                step = true;
            }
        } else {
            if (iterationCounter + nextIterationCounter < length -1 && nextObjValue != -Numerical::Infinity) {
                if (thresholdReportLevel > 1.0) LPWARNING("next step f: "<<nextObjValue);
                nextAlphaId--;
                nextIterationCounter++;
                getNextElement(&m_breakpoints, length - (iterationCounter+nextIterationCounter));
                nextObjValue += functionSlope * (m_breakpoints[length-1-(iterationCounter + nextIterationCounter)].value -
                        m_breakpoints[length-(iterationCounter + nextIterationCounter)].value);
                m_breakpoints[length-1-(iterationCounter + nextIterationCounter)].functionValue = nextObjValue;
                functionSlope -= Numerical::fabs(alpha.at(m_breakpoints[nextAlphaId].index));
                if (m_breakpoints[length-1].functionValue > nextObjValue) {
                    nextObjValue = -Numerical::Infinity;
                }
                step = true;
            } else if( nextObjValue != -Numerical::Infinity) {
                nextObjValue = -Numerical::Infinity;
                step = true;
            }
        }
        prevIsBetter = nextObjValue > prevObjValue? false : true;
    }

    if ((prevObjValue == - Numerical::Infinity) && (nextObjValue == - Numerical::Infinity)) {
        m_incomingVariableIndex = -1;
        m_dualSteplength = 0.0;
        m_phaseIObjectiveValue = m_breakpoints[length-1].functionValue;
        if (thresholdReportLevel > 0.0){
            LPERROR(" - Threshold Error - No breakpoint found! num of breakpoints: "<<m_breakpoints.size());
        }
    } else if (prevIsBetter) {
        m_phaseIObjectiveValue = m_breakpoints[prevAlphaId].functionValue;
        m_incomingVariableIndex = m_breakpoints[prevAlphaId].index;
        m_dualSteplength = m_breakpoints[prevAlphaId].value;
    } else {
        m_phaseIObjectiveValue = m_breakpoints[nextAlphaId].functionValue;
        m_incomingVariableIndex = m_breakpoints[nextAlphaId].index;
        m_dualSteplength = m_breakpoints[nextAlphaId].value;
    }

    if (thresholdReportLevel > 1.0)  {
        LPWARNING("steps(prev;next): " << prevIterationCounter<<
                                              " ; " << nextIterationCounter<<"\n");
        LPWARNING(m_breakpoints[alphaId+nextIterationCounter-prevIterationCounter]<<" chosen");
    }
//    LPWARNING("breakpoints\t\t|\talpha values");
//    for(unsigned i=0;i<m_breakpoints.size();i++){LPWARNING(m_breakpoints[i]<<
//                                                           "\t|\t"<<alpha.at(m_breakpoints[i].index));}
}

void DualRatiotest::performRatiotestPhase1(const Vector& alpha,
                                           Numerical::Double phaseIReducedCost,
                                           Numerical::Double phaseIObjectiveValue) {
    Numerical::Double functionSlope = Numerical::fabs(phaseIReducedCost);
    unsigned int iterationCounter = 0, length = 0;

    m_phaseIObjectiveValue = phaseIObjectiveValue;
    m_incomingVariableIndex = -1;
    m_dualSteplength = 0;

    generateBreakpointsPhase1(alpha,phaseIReducedCost);
    length = m_breakpoints.size();

    //Init the heap with the breakpoint at 0
    getNextElement(&m_breakpoints,length);

    if (m_breakpoints.size() > 1) {
        int num = nonlinearDualPhaseIFunction;
        //fake feasible variables
        while(m_breakpoints[length-1-iterationCounter].index != -1) {
            functionSlope -= Numerical::fabs(alpha.at(m_breakpoints[length-1-iterationCounter].index));

            iterationCounter++;
            getNextElement(&m_breakpoints,length-iterationCounter);

            if (functionSlope < 0) {
                if(thresholdReportLevel > 0){
                    LPERROR("functionSlope < 0 in the beginning (PHASE I) - incoming index: "<<m_incomingVariableIndex);
                }
                return;
            }
        }
        //using traditional one step method
        if (num == 0) {
            iterationCounter++;
            getNextElement(&m_breakpoints,length-1);

            m_phaseIObjectiveValue += functionSlope * (m_breakpoints[length-1-iterationCounter].value -
                    m_breakpoints[length-iterationCounter].value);
            m_breakpoints[length-1-iterationCounter].functionValue = m_phaseIObjectiveValue;
            m_incomingVariableIndex = m_breakpoints[length-1-iterationCounter].index;
            m_dualSteplength = m_breakpoints[length-1-iterationCounter].value;
        //using piecewise linear function
        }else{

            computeFunctionPhase1(alpha, iterationCounter, functionSlope);

            //using piecewise linear function with numerical threshold
            if(num == 2){
                if(m_breakpoints[length-1-iterationCounter].index != -1){
                    if( numericalCheckPhase1(alpha,length-1-iterationCounter)){
                        useNumericalThresholdPhase1(iterationCounter, alpha, functionSlope);
                    }
                }
            }
            //EXPAND
//            if(num == 3 && m_incomingVariableIndex != -1){
//                Numerical::Double t_min = expandStep / alpha.at(m_incomingVariableIndex);
//                if(t_min > m_dualSteplength){
//                    //how to handel this
//                }
//            }
        }
//        LPWARNING("breakpoints\t\t\t|\talpha values\t|\treduced costs");
//        for(unsigned i=0;i<m_breakpoints.size();i++){
//            if(m_breakpoints[i].index != -1){
//                LPWARNING(m_breakpoints[i]<<"     \t|\t"<<alpha.at(m_breakpoints[i].index)<<"    \t|\t"<<m_reducedCosts.at(m_breakpoints[i].index));
//            } else {
//                LPWARNING(m_breakpoints[i]<<"\t|\t"<<" - ");
//            }
//        }
//        LPINFO("1160 d_j: "<<m_reducedCosts.at(1160));
//        LPINFO("50 d_j: "<<m_reducedCosts.at(50));
//        LPINFO("inc d_j: "<<m_reducedCosts.at(m_incomingVariableIndex));
//        LPINFO("inc alpha: "<<alpha.at(m_incomingVariableIndex));

    } else{
        LPERROR(" - Ratiotest - No breakpoint found!");
    }
}

void DualRatiotest::generateBreakpointsPhase2(unsigned int outgoingVariableIndex,
                                              const Vector &alpha,
                                              Numerical::Double phaseIIObjectiveValue){
    #ifdef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    m_transform = false;
    m_tPositive = false;
    m_phaseIIObjectiveValue = phaseIIObjectiveValue;
    m_boundflips.clear();
    m_boundflips.reserve(alpha.nonZeros());
    m_breakpoints.clear();
    m_breakpoints.reserve(alpha.nonZeros());
    BreakPoint currentRatio;
    currentRatio.index = -1;
    currentRatio.value = 0;
    currentRatio.functionValue = phaseIIObjectiveValue;
    m_breakpoints.push_back(currentRatio);

    currentRatio.functionValue = 0;

    //determining t>0 or t<0 cases
    if ( *(m_variableStates.getAttachedData(outgoingVariableIndex)) <
         m_model.getVariable(outgoingVariableIndex).getLowerBound()) {
        m_tPositive = true;
    } else if (*(m_variableStates.getAttachedData(outgoingVariableIndex)) >
               m_model.getVariable(outgoingVariableIndex).getUpperBound()) {
        m_tPositive = false;
    }

//computing ratios
    Variable::VARIABLE_TYPE typeOfIthVariable;
    IndexList<const Numerical::Double*>::Iterator it;
    IndexList<const Numerical::Double*>::Iterator endit;
    m_variableStates.getIterators(&it,&endit,0,Simplex::FEASIBILITY_ENUM_LENGTH);
    unsigned int variableIndex = it.getData();

    //Threshold report
    unsigned int reported = 1;

//    LPINFO("m_tpositive "<<m_tPositive);
    //t>=0 case
    if (m_tPositive) {
        while (!m_transform && it != endit) {
            variableIndex = it.getData();
            if (m_variableStates.where(variableIndex) != Simplex::BASIC) {
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();

        //free variables
                if (typeOfIthVariable == Variable::FREE && alpha.at(variableIndex) != 0) {                    
                    m_transform = true;
                    m_incomingVariableIndex = variableIndex;
                 } else{
        //bounded variables
                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_LB &&
                            typeOfIthVariable == Variable::BOUNDED && alpha.at(variableIndex) < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                        m_breakpoints.push_back(currentRatio);
                    } else
                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_UB &&
                            typeOfIthVariable == Variable::BOUNDED && alpha.at(variableIndex) > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                        m_breakpoints.push_back(currentRatio);
                    } else

        //plus type variables
                    if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                        m_breakpoints.push_back(currentRatio);
                    } else

        //minus type variables
                    if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                        m_breakpoints.push_back(currentRatio);
                    }
                 }
                 if(thresholdReportLevel > 0){
                     if(m_breakpoints.size()>reported){
                         reported++;
                         if(currentRatio.value < 0){
//                             LPERROR("BREAKPOINT ERROR PHASE-II, FAKE BREAKPOINT "<<currentRatio.value
//                                     << "(index: "<<currentRatio.index<<")");
                         }
                     }
                 }
            }
            it++;
        }

    //t<=0 case
    } else{
        while (!m_transform && it != endit) {
            variableIndex = it.getData();
            if (m_variableStates.where(variableIndex) != Simplex::BASIC) {
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();

        //free variables
                 if (typeOfIthVariable == Variable::FREE && alpha.at(variableIndex) != 0) {
                    m_transform = true;
                    m_incomingVariableIndex = variableIndex;
                 } else{

        //bounded variables
                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_LB &&
                            typeOfIthVariable == Variable::BOUNDED && alpha.at(variableIndex) > 0) {
//                        if(variableIndex == 959){
//                            LPINFO("959 LB -> UB ratio ; "<<alpha.at(variableIndex));
//                        }
                        currentRatio.index = variableIndex;
                        currentRatio.value = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                        m_breakpoints.push_back(currentRatio);
                    } else
                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_UB &&
                            typeOfIthVariable == Variable::BOUNDED && alpha.at(variableIndex) < 0) {
//                        if(variableIndex == 959){
//                            LPINFO("959 UB -> LB ratio ; "<<alpha.at(variableIndex));
//                        }
                        currentRatio.index = variableIndex;
                        currentRatio.value = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                        m_breakpoints.push_back(currentRatio);
                    } else

        //plus type variables
                    if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                        m_breakpoints.push_back(currentRatio);
                    } else

        //minus type variables
                    if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                        m_breakpoints.push_back(currentRatio);
                    }
                 }
                 if(thresholdReportLevel > 0){
                     if(m_breakpoints.size()>reported){
                         reported++;
                         if(currentRatio.value < 0){
                             LPERROR("BREAKPOINT ERROR PHASE-II, FAKE BREAKPOINT "<<currentRatio.value
                                     << "(index: "<<currentRatio.index<<")");
                         }
                     }
                 }
            }
            it++;
        }
    }
//    LPINFO("num of bpts: "<<m_breakpoints.size());
}

void DualRatiotest::computeFunctionPhase2(const Vector &alpha,
                                          unsigned int &iterationCounter,
                                          Numerical::Double &functionSlope,
                                          Numerical::Double& primalSteplength)
{
    unsigned int length = m_breakpoints.size(), id = 0;
    Numerical::Double previousSlope = 0;
    while (!m_transform && functionSlope >= 0 && iterationCounter < length-1)  {
        iterationCounter++;
        getNextElement(&m_breakpoints,length-iterationCounter);
        id = length-1-iterationCounter;

        Numerical::Double diff = m_breakpoints[id].value - m_breakpoints[id+1].value;
        m_phaseIIObjectiveValue += functionSlope * diff;

        m_breakpoints[id].functionValue = m_phaseIIObjectiveValue;
        previousSlope = functionSlope;

        const Variable & variable = m_model.getVariable(m_breakpoints[id].index);
        functionSlope -= Numerical::fabs(alpha.at(m_breakpoints[id].index)) *
                Numerical::fabs(variable.getUpperBound() - variable.getLowerBound());

        if (m_tPositive) {
            primalSteplength =- previousSlope / alpha.at(m_breakpoints[id].index);
        } else{
            primalSteplength = previousSlope / alpha.at(m_breakpoints[id].index);
        }

//        LPERROR("primalsteplength "<< primalSteplength);

        if(variable.getType() == Variable::BOUNDED){
            if((variable.getUpperBound() - variable.getLowerBound()) < Numerical::fabs(primalSteplength) ) {
//                LPERROR("variable: "<<m_breakpoints[id].index);
//                LPERROR("variable.getUpperBound() - variable.getLowerBound()" <<variable.getUpperBound() - variable.getLowerBound());
                m_boundflips.push_back(m_breakpoints[id].index);
//                m_variableAge[m_breakpoints[id].index] = m_variableAge[m_breakpoints[id].index] + m_ageStep;
//                LPINFO("Slope AGE incremented: ("<<m_breakpoints[id].index<<") : "<<m_variableAge[m_breakpoints[id].index]);
            } else{
                m_transform = true;
            }
        }
        if (functionSlope < 0) {
            m_transform = true;
        }
    }
    //This forces basis change
    if(m_boundflips.size()>0 && (int)m_boundflips.back() == m_breakpoints.at(length-1-iterationCounter).index){
//        LPERROR("POP");
        m_boundflips.pop_back();
    }
    m_incomingVariableIndex = m_breakpoints.at(length-1-iterationCounter).index;
//    LPERROR("m_incomingVariableIndex" << m_incomingVariableIndex);
    m_dualSteplength = m_tPositive ? m_breakpoints.at(length-1-iterationCounter).value :
                                     - m_breakpoints.at(length-1-iterationCounter).value;
}

void DualRatiotest::useNumericalThresholdPhase2(unsigned int iterationCounter,
                                                const Vector &alpha)
{
    unsigned int length = m_breakpoints.size();
    int id, maxId = length-1-iterationCounter;
    Numerical::Double maxValue;

    if(maxId == -1){
        maxValue = 0;
    } else {
        maxValue = Numerical::fabs(alpha.at(m_breakpoints[maxId].index)) / m_variableAge[m_breakpoints[maxId].index];
    }

    while (iterationCounter < length-1)  {
        iterationCounter++;
        getNextElement(&m_breakpoints,length-iterationCounter);
        id = length-1-iterationCounter;
        Numerical::Double diff = m_breakpoints[id].value - m_breakpoints[id+1].value;
        if( diff == 0 ){
            if(thresholdReportLevel > 1) LPWARNING("Phase 2 Threshold activated 0 diff alpha: "<<maxValue);
            m_breakpoints[id].functionValue = m_phaseIIObjectiveValue;

            if(Numerical::fabs(alpha.at(m_breakpoints[id].index)) / m_variableAge[m_breakpoints[maxId].index] > maxValue){
                maxId = id;
                maxValue = Numerical::fabs(alpha.at(m_breakpoints[id].index)) / m_variableAge[m_breakpoints[maxId].index];
            }

        }else{
            break;
        }

    }

    if(maxValue < m_pivotThreshold){
        m_incomingVariableIndex = -1;
    }else{
        m_incomingVariableIndex = m_breakpoints[maxId].index;
    }

    if(thresholdReportLevel >= 1) LPWARNING("Phase 2 Threshold ended alpha: "<<maxValue);
}

void DualRatiotest::performRatiotestPhase2(unsigned int outgoingVariableIndex,
                                           const Vector& alpha,
                                           Numerical::Double phaseIIObjectiveValue) {

    generateBreakpointsPhase2(outgoingVariableIndex,alpha,phaseIIObjectiveValue);

    m_incomingVariableIndex = -1;
    m_dualSteplength = 0;

    Numerical::Double primalSteplength;
    unsigned int iterationCounter = 0,length = m_breakpoints.size();
    Numerical::Double functionSlope = 0;
    if(m_tPositive){
        functionSlope = - (*(m_variableStates.getAttachedData(outgoingVariableIndex)) -
                m_model.getVariable(outgoingVariableIndex).getLowerBound());
    }else{
        functionSlope = *(m_variableStates.getAttachedData(outgoingVariableIndex)) -
                m_model.getVariable(outgoingVariableIndex).getUpperBound();
    }
//    LPERROR("outgoingVariableIndex : " <<outgoingVariableIndex);
//    LPERROR("LB : " <<m_model.getVariable(outgoingVariableIndex).getLowerBound());
//    LPERROR("UB : " <<m_model.getVariable(outgoingVariableIndex).getUpperBound());
//    LPERROR("x_B : " <<(*(m_variableStates.getAttachedData(outgoingVariableIndex))));
//    LPERROR("functionSlope : " <<functionSlope);

    //Slope check should be enabled in debug mode
#ifndef NDEBUG
    if(functionSlope<0){
        LPERROR("SLOPE IS NEGATIVE ERROR - TERMINATING!!!");
        exit(-1);
    }
#endif


    //init function at 0
    getNextElement(&m_breakpoints,length);

    if (m_breakpoints.size() > 1) {
        int num = nonlinearDualPhaseIIFunction;
        if (num == 0) {
        //using traditional one step method
            if (!m_transform) {
                //Handle fake feasible breakpoints
                while(m_breakpoints[length-1-iterationCounter].index != -1) {
//                    LPWARNING("phase2 fakeFeas t_pos: " <<m_tPositive << " " << m_breakpoints[length-1-iterationCounter] << "\t alpha: \t" <<
//                                alpha.at(m_breakpoints[length-1-iterationCounter].index) << "\t reducedcost: \t" <<
//                                m_reducedCosts.at(m_breakpoints[length-1-iterationCounter].index)
//                                );

                    const Variable & variable = m_model.getVariable(m_breakpoints[length-1-iterationCounter].index);
                    functionSlope -= Numerical::fabs(alpha.at(m_breakpoints[length-1-iterationCounter].index)) *
                            (variable.getUpperBound() - variable.getLowerBound());

                    iterationCounter++;
                    getNextElement(&m_breakpoints,length-iterationCounter);

                    if (functionSlope < 0) {
                        if(thresholdReportLevel > 0){
                            LPERROR("functionSlope < 0 in the beginning using outgoing variable: "<<outgoingVariableIndex);
                        }
                        return;
                    }
                }
                iterationCounter++;
                getNextElement(&m_breakpoints,length-1);

                m_phaseIIObjectiveValue += functionSlope * (m_breakpoints[length-2].value -
                        m_breakpoints[length-1].value);
                m_breakpoints[length-2].functionValue = m_phaseIIObjectiveValue;
                m_incomingVariableIndex = m_breakpoints[length-2].index;
                if (m_tPositive) {
                    primalSteplength = - functionSlope / alpha.at(m_breakpoints[length-2].index);
                    m_dualSteplength = m_breakpoints[length-2].value;
                } else {
                    primalSteplength = functionSlope / alpha.at(m_breakpoints[length-2].index);
                    m_dualSteplength = m_breakpoints[length-2].value;
                }
            }
            //using piecewise linear function
    }else{
                if (!m_transform) {
                    //Handle fake feasible breakpoints
                    while(m_breakpoints[length-1-iterationCounter].index != -1) {
//                        LPWARNING("phase2 fakeFeas t_pos: " <<m_tPositive << " " << m_breakpoints[length-1-iterationCounter] << "\t d: \t" <<
//                                    alpha.at(m_breakpoints[length-1-iterationCounter].index) << "\t alpha: \t" <<
//                                    m_reducedCosts.at(m_breakpoints[length-1-iterationCounter].index));
                        const Variable & variable = m_model.getVariable(m_breakpoints[length-1-iterationCounter].index);
                        functionSlope -= Numerical::fabs(alpha.at(m_breakpoints[length-1-iterationCounter].index)) *
                                (variable.getUpperBound() - variable.getLowerBound());

                        iterationCounter++;
                        getNextElement(&m_breakpoints,length-iterationCounter);

                        if (functionSlope < 0) {
                            if(thresholdReportLevel > 0){
                                LPERROR("functionSlope < 0 in the beginning using outgoing variable: "<<outgoingVariableIndex);
                            }
                            return;
                        }
                    }

                    computeFunctionPhase2(alpha,iterationCounter,functionSlope,primalSteplength);

                    //numerical threshold
                    if(num == 2){
    //                    if(m_breakpoints[length-1-iterationCounter].index != -1){
                            useNumericalThresholdPhase2(iterationCounter, alpha);
    //                    }
                    }
                }
            }
        } else{
            LPERROR(" - Ratiotest - No breakpoint found!");
        }

        if(m_incomingVariableIndex != -1){
//            m_variableAge[m_incomingVariableIndex] = m_variableAge[m_incomingVariableIndex] + m_ageStep;
//            LPINFO("AGE incremented: ("<<m_incomingVariableIndex<<") : "<<m_variableAge[m_incomingVariableIndex]);
        }
}


DualRatiotest::~DualRatiotest() {
}

// Breakpoint printer debug code
//LPWARNING("breakpoints\t\t\t|\talpha values\t|\treduced costs");
//for(unsigned i=0;i<m_breakpoints.size();i++){
//    if(m_breakpoints[i].index != -1){
//        LPWARNING(m_breakpoints[i]<<"     \t|\t"<<alpha.at(m_breakpoints[i].index)<<"    \t|\t"<<m_reducedCosts.at(m_breakpoints[i].index));
//    } else {
//        LPWARNING(m_breakpoints[i]<<"\t|\t"<<" - ");
//    }
//}

