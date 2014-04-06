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

static const DualRatiotest::DUAL_RATIOTEST_METHOD nonlinearDualPhaseIFunction =
        (DualRatiotest::DUAL_RATIOTEST_METHOD)SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_dual_phaseI_function");
static const DualRatiotest::DUAL_RATIOTEST_METHOD nonlinearDualPhaseIIFunction =
        (DualRatiotest::DUAL_RATIOTEST_METHOD)SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_dual_phaseII_function");
static const Numerical::Double optimalityTolerance =
        SimplexParameterHandler::getInstance().getDoubleParameterValue("e_optimality");

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
    m_variableAge(model.getColumnCount() + model.getRowCount(),1),
    m_ageStep(1),
    m_dualRatiotestUpdater(dualRatiotestUpdater),
    m_incomingVariableIndex(-1),
    m_dualSteplength(0),
    m_phaseIObjectiveValue(0),
    m_phaseIIObjectiveValue(0),
    m_pivotThreshold(SimplexParameterHandler::getInstance().getDoubleParameterValue("e_pivot")),
    m_enableFakeFeasibility(SimplexParameterHandler::getInstance().getIntegerParameterValue("enable_fake_feasibility")),
    m_stablePivotActivationPhase1(0),
    m_stablePivotBackwardStepsPhase1(0),
    m_stablePivotForwardStepsPhase1(0),
    m_fakeFeasibilityActivationPhase1(0),
    m_fakeFeasibilityCounterPhase1(0),
    m_stablePivotNotFoundPhase2(0),
    m_fakeFeasibilityActivationPhase2(0),
    m_fakeFeasibilityCounterPhase2(0)
{}

//returns false if alpha is numerical good
bool DualRatiotest::numericalCheck(const Vector& alpha, unsigned int alphaId)
{
    if ( Numerical::fabs(alpha.at(alphaId)) > m_pivotThreshold)
    {
        return false;
    }else{
        return true;
    }
}

void DualRatiotest::generateSignedBreakpointsPhase1(const Vector& alpha){
    #ifndef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    m_dualRatiotestUpdater.m_updateVector.clear();
    m_dualRatiotestUpdater.m_updateVector.reserve(m_model.getMatrix().columnCount() + m_model.getMatrix().rowCount());


//computing ratios
    IndexList<>::Iterator it;
    IndexList<>::Iterator endit;
    unsigned int variableIndex = 0;
    Numerical::Double valueOfVariable = 0;
    Variable::VARIABLE_TYPE typeOfIthVariable;

//t>=0 case
    if (m_tPositive) {

    //computing ratios in M
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::MINUS);

        for (; it!=endit; it++){
            variableIndex = it.getData();
            if (alpha.at(variableIndex) < 0) {
                valueOfVariable = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                }
            }
        }

    //computing ratios in P
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::PLUS);

        for (; it!=endit; it++){
            variableIndex = it.getData();
            if (alpha.at(variableIndex) > 0){
                valueOfVariable = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                }
             }
        }

    //computing ratios in F
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);

        for (; it!=endit; it++){
            variableIndex = it.getData();
            typeOfIthVariable = m_model.getVariable(variableIndex).getType();
            if ( alpha.at(variableIndex) != 0 ) {
                valueOfVariable = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);

                if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) > 0) {
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                } else
                if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) < 0) {
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                } else
                if (typeOfIthVariable == Variable::FREE ) {
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                }
            }
        }

//t<=0 case
    } else{

    //computing ratios in M
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::MINUS);

        for (; it!=endit; it++) {
            variableIndex = it.getData();
            if (alpha.at(variableIndex) > 0) {
                valueOfVariable = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                }
            }
        }

    //computing ratios in P
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::PLUS);

        for (; it!=endit; it++) {
            variableIndex = it.getData();
            if (alpha.at(variableIndex) < 0) {
                valueOfVariable = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                }
            }
        }

    //computing ratios in F
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);

        for (; it!=endit; it++) {
            variableIndex = it.getData();
            typeOfIthVariable = m_model.getVariable(variableIndex).getType();
            if (alpha.at(variableIndex) != 0 ) {
                valueOfVariable = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);

                if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) < 0) {
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                } else
                if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) > 0) {
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                } else
                if (typeOfIthVariable == Variable::FREE ) {
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                }
            }
        }
    }
}

void DualRatiotest::computeFunctionPhase1(const Vector& alpha,
                                          unsigned int& iterationCounter,
                                          Numerical::Double& functionSlope)
{
    unsigned int length = m_breakpointHandler.getNumberOfBreakpoints();
    Numerical::Double t_prev = 0;
    Numerical::Double t_actual = 0;
    const BreakpointHandler::BreakPoint * actualBreakpoint = NULL;

    while (functionSlope >= 0 && iterationCounter < length) {
        actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
        t_actual = actualBreakpoint->value;

        m_phaseIObjectiveValue += functionSlope * (t_actual - t_prev);
        actualBreakpoint->functionValue = m_phaseIObjectiveValue;
        functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));

        iterationCounter++;
        t_prev = t_actual;
    }
    if (actualBreakpoint != NULL){
        m_dualSteplength = m_tPositive ? actualBreakpoint->value : - actualBreakpoint->value;
        m_incomingVariableIndex = actualBreakpoint->variableIndex;
    } else{
//        m_breakpointHandler.printBreakpoints();
        LPERROR("In phase 1 NO function defined, num of bpts: "<<m_breakpointHandler.getNumberOfBreakpoints());
    }
}

void DualRatiotest::useNumericalThresholdPhase1(unsigned int iterationCounter,
                                          const Vector& alpha,
                                          Numerical::Double& functionSlope)
{
    m_stablePivotActivationPhase1++;

    unsigned int length = m_breakpointHandler.getNumberOfBreakpoints(),
    breakpointId = iterationCounter, prevBreakpointId = breakpointId, nextBreakpointId = breakpointId,
    prevIterationCounter = 0, nextIterationCounter = 0;
#ifndef NDEBUG
        LPWARNING("BAD NUMERICAL VALUE IN PHASE I,  ACTIVATING THRESHOLD for variable " <<
                  m_breakpointHandler.getBreakpoint(breakpointId)->variableIndex <<
                  " with alpha" <<alpha.at(m_breakpointHandler.getBreakpoint(breakpointId)->variableIndex)<<
                  " at f= "<<m_breakpointHandler.getBreakpoint(breakpointId)->functionValue);
#endif
    Numerical::Double prevObjValue = m_phaseIObjectiveValue, nextObjValue = m_phaseIObjectiveValue;
    if (iterationCounter > 0) {
        prevBreakpointId--;
        prevIterationCounter++;
        prevObjValue = m_breakpointHandler.getBreakpoint(prevBreakpointId)->functionValue;
    }
    if (iterationCounter < length-1) {
        nextBreakpointId++;
        Numerical::Double t_prev = m_breakpointHandler.getBreakpoint(iterationCounter+nextIterationCounter)->value;
        nextIterationCounter++;

        const BreakpointHandler::BreakPoint * actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter+nextIterationCounter);
        Numerical::Double t_actual = actualBreakpoint->value;
        nextObjValue += functionSlope * (t_actual - t_prev);
        actualBreakpoint->functionValue = nextObjValue;
        functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));
        if(nextObjValue < m_initialPhaseIObjectiveValue){
            nextObjValue = -Numerical::Infinity;
        }
    } else{
        nextObjValue = -Numerical::Infinity;
    }
    bool prevIsBetter = nextObjValue > prevObjValue ? false : true;
    bool step = true;

    while (step &&( prevIsBetter ? Numerical::fabs(alpha.at(prevBreakpointId)) < m_pivotThreshold :
                    Numerical::fabs(alpha.at(nextBreakpointId)) < m_pivotThreshold) ) {
        step = false;
        if (prevIsBetter) {
            m_stablePivotBackwardStepsPhase1++;
            if (iterationCounter - prevIterationCounter > 0) {
                prevBreakpointId--;
                prevIterationCounter++;
                prevObjValue = m_breakpointHandler.getBreakpoint(prevBreakpointId)->functionValue;
                step = true;
            } else if ( prevObjValue != -Numerical::Infinity) {
                prevObjValue = -Numerical::Infinity;
                step = true;
            }
        } else {
            m_stablePivotForwardStepsPhase1++;
            if (iterationCounter + nextIterationCounter < length && nextObjValue != -Numerical::Infinity) {
                nextBreakpointId++;
                Numerical::Double t_prev = m_breakpointHandler.getBreakpoint(iterationCounter+nextIterationCounter)->value;
                nextIterationCounter++;
                const BreakpointHandler::BreakPoint * actualBreakpoint =
                        m_breakpointHandler.getBreakpoint(iterationCounter+nextIterationCounter);
                Numerical::Double t_actual = actualBreakpoint->value;
                nextObjValue += functionSlope * (t_actual - t_prev);
                actualBreakpoint->functionValue = nextObjValue;
                functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));
                if (nextObjValue < m_initialPhaseIObjectiveValue) {
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
        LPWARNING("No stable pivot found in phase 1!");
        m_incomingVariableIndex = -1;
        m_dualSteplength = 0.0;
        m_phaseIObjectiveValue = m_initialPhaseIObjectiveValue;
#ifndef NDEBUG
        LPERROR(" - Threshold Error - No breakpoint found! num of breakpoints: "<<
                m_breakpointHandler.getNumberOfBreakpoints());
#endif
    } else if (prevIsBetter) {
        m_phaseIObjectiveValue = m_breakpointHandler.getBreakpoint(prevBreakpointId)->functionValue;
        m_incomingVariableIndex = m_breakpointHandler.getBreakpoint(prevBreakpointId)->variableIndex;
        m_dualSteplength = m_breakpointHandler.getBreakpoint(prevBreakpointId)->value;
    } else {
        m_phaseIObjectiveValue = m_breakpointHandler.getBreakpoint(nextBreakpointId)->functionValue;
        m_incomingVariableIndex = m_breakpointHandler.getBreakpoint(nextBreakpointId)->variableIndex;
        m_dualSteplength = m_breakpointHandler.getBreakpoint(nextBreakpointId)->value;
    }
#ifndef NDEBUG
    LPWARNING("steps(prev;next): " << prevIterationCounter<<
                                          " ; " << nextIterationCounter<<"\n");
#endif
}

void DualRatiotest::performRatiotestPhase1(const Vector& alpha,
                                           Numerical::Double phaseIReducedCost,
                                           Numerical::Double phaseIObjectiveValue)
{
    m_breakpointHandler.init(2*alpha.nonZeros());
    Numerical::Double functionSlope = Numerical::fabs(phaseIReducedCost);
    unsigned iterationCounter = 0;

    m_phaseIObjectiveValue = phaseIObjectiveValue;
    m_initialPhaseIObjectiveValue = phaseIObjectiveValue;
    m_incomingVariableIndex = -1;
    m_dualSteplength = 0;

    //determining t>=0 or t<=0 cases
    if (phaseIReducedCost > 0) {
        m_tPositive = false;
    } else{
        m_tPositive = true;
    }

    generateSignedBreakpointsPhase1(alpha);


    if (m_breakpointHandler.getNumberOfBreakpoints() > 0) {
        m_breakpointHandler.selectMethod();
        //fake feasible variables
        if (m_enableFakeFeasibility) {
            const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
            int fakeFesibilityCounter = 0;

            while (breakpoint->value < 0 ) {
                fakeFesibilityCounter++;

                functionSlope -= Numerical::fabs(alpha.at(breakpoint->variableIndex));
                iterationCounter++;

                if(iterationCounter < m_breakpointHandler.getNumberOfBreakpoints()){
                    breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
                } else {
                    break;
                }
            }

            if(fakeFesibilityCounter > 0){
                m_fakeFeasibilityActivationPhase1++;
                m_fakeFeasibilityCounterPhase1+=fakeFesibilityCounter;
            }

            if( functionSlope < 0 || iterationCounter == m_breakpointHandler.getNumberOfBreakpoints()){
                m_incomingVariableIndex = -1;
                return;
            }
        }


        if(iterationCounter < m_breakpointHandler.getNumberOfBreakpoints()){
            switch (nonlinearDualPhaseIFunction){
                case ONE_STEP:{
                    const BreakpointHandler::BreakPoint* breakpoint = m_breakpointHandler.getBreakpoint(0);

                    m_phaseIObjectiveValue += functionSlope * breakpoint->value;
                    breakpoint->functionValue = m_phaseIObjectiveValue;
                    m_incomingVariableIndex = breakpoint->variableIndex;
                    m_dualSteplength = breakpoint->value;
                    break;
                }

                case PIECEWISE_LINEAR_FUNCTION:{
                    computeFunctionPhase1(alpha, iterationCounter, functionSlope);
                    break;
                }

                case STABLE_PIVOT:{
                    computeFunctionPhase1(alpha, iterationCounter, functionSlope);

                    if( Numerical::fabs(alpha.at(m_incomingVariableIndex)) < m_pivotThreshold ) {
                        useNumericalThresholdPhase1(iterationCounter, alpha, functionSlope);
                    }
                    break;
                }
            }
        }
    } else{
        LPWARNING(" - Ratiotest - No breakpoint found!");
    }

    if(m_incomingVariableIndex != -1 && Numerical::fabs(alpha.at(m_incomingVariableIndex)) < 1e-6){
        LPWARNING("small pivot in phase1: "<<alpha.at(m_incomingVariableIndex));
    }
}

void DualRatiotest::generateSignedBreakpointsPhase2(const Vector &alpha)
{
    #ifndef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    m_breakpointHandler.init(alpha.nonZeros());

//computing ratios
    IndexList<const Numerical::Double*>::Iterator it;
    IndexList<const Numerical::Double*>::Iterator endit;
    unsigned int variableIndex = -1;
    Numerical::Double valueOfVariable = 0;

    //free variables always enter the basis
    m_variableStates.getIterators(&it,&endit,Simplex::NONBASIC_FREE,1);
    while (it != endit) {
        variableIndex = it.getData();
        if (alpha.at(variableIndex) != 0) {
            LPINFO("Free variable selected to enter basis");
            m_incomingVariableIndex = variableIndex;
            break;
        }
        it++;
    }

    if(m_incomingVariableIndex == -1){
        //NONBASIC_FIXED variables are ignored
        m_variableStates.getIterators(&it,&endit,Simplex::NONBASIC_AT_LB,2);

        //t>=0 case
        if (m_tPositive) {
            while (it != endit) {
                variableIndex = it.getData();

                if (it.getPartitionIndex() == Simplex::NONBASIC_AT_LB && alpha.at(variableIndex) > 0) {
                    valueOfVariable = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
//                    if (valueOfVariable < 0) LPWARNING("t>0, negative value: "<<valueOfVariable);
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                } else
                if (it.getPartitionIndex() == Simplex::NONBASIC_AT_UB && alpha.at(variableIndex) < 0) {
                    valueOfVariable = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
//                    if (valueOfVariable < 0) LPWARNING("t<0, negative value: "<<valueOfVariable);
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                }
                it++;
            }

        //t<=0 case
        } else{
            while (it != endit) {
                variableIndex = it.getData();

                if (it.getPartitionIndex() == Simplex::NONBASIC_AT_LB && alpha.at(variableIndex) < 0) {
                    valueOfVariable = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
//                    if (valueOfVariable < 0) LPWARNING("t<0,negative value: "<<valueOfVariable);
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                } else
                if (it.getPartitionIndex() == Simplex::NONBASIC_AT_UB && alpha.at(variableIndex) > 0) {
                    valueOfVariable = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
//                    if (valueOfVariable < 0) LPWARNING("t<0, negative value: "<<valueOfVariable);
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                }
                it++;
            }
        }
    }
}

void DualRatiotest::computeFunctionPhase2(const Vector &alpha,
                                          unsigned int &iterationCounter,
                                          Numerical::Double &functionSlope)
{
    unsigned int length = m_breakpointHandler.getNumberOfBreakpoints();
    Numerical::Double t_prev = 0;
    Numerical::Double t_actual = 0;
    Numerical::Double primalSteplength = 0;

    const BreakpointHandler::BreakPoint * actualBreakpoint = NULL;

    while (iterationCounter < length) {

        actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
        t_actual = actualBreakpoint->value;

        m_phaseIIObjectiveValue += functionSlope * (t_actual - t_prev);
        actualBreakpoint->functionValue = m_phaseIIObjectiveValue;

        if (m_tPositive) {
            primalSteplength = functionSlope / alpha.at(actualBreakpoint->variableIndex);
        } else{
            primalSteplength = - functionSlope / alpha.at(actualBreakpoint->variableIndex);
        }

        const Variable & variable = m_model.getVariable(actualBreakpoint->variableIndex);
        functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex)) *
                Numerical::fabs(variable.getUpperBound() - variable.getLowerBound());

        if (functionSlope < 0) {
            m_incomingVariableIndex = actualBreakpoint->variableIndex;
            m_dualSteplength = m_tPositive ? actualBreakpoint->value : - actualBreakpoint->value;
            break;
        }

        if(variable.getType() == Variable::BOUNDED){
            if((variable.getUpperBound() - variable.getLowerBound()) < Numerical::fabs(primalSteplength) ) {
                m_boundflips.push_back(actualBreakpoint->variableIndex);
            } else{
                m_incomingVariableIndex = actualBreakpoint->variableIndex;
                m_dualSteplength = m_tPositive ? actualBreakpoint->value : - actualBreakpoint->value;
                break;
            }
        }

        t_prev = t_actual;
        iterationCounter++;
    }
}

void DualRatiotest::useNumericalThresholdPhase2(unsigned int iterationCounter,
                                                const Vector &alpha)
{
    unsigned int length = m_breakpointHandler.getNumberOfBreakpoints();
    int maxAlphaId = iterationCounter;
    Numerical::Double maxAlphaAbs = 0;
    Numerical::Double t_actual = 0;
    const BreakpointHandler::BreakPoint * breakpoint = NULL;

    if(iterationCounter != length){
        breakpoint = m_breakpointHandler.getBreakpoint(maxAlphaId);
        maxAlphaAbs = Numerical::fabs(alpha.at(breakpoint->variableIndex)) / m_variableAge[breakpoint->variableIndex];
        t_actual = breakpoint->value;
    }

    while (iterationCounter < length-1)  {
        iterationCounter++;

        breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
        Numerical::Double t_next = breakpoint->value;
        if ( t_actual == t_next ) {
            breakpoint->functionValue = m_phaseIIObjectiveValue;

            if (Numerical::fabs(alpha.at(breakpoint->variableIndex)) / m_variableAge[breakpoint->variableIndex] > maxAlphaAbs){
                maxAlphaId = iterationCounter;
                maxAlphaAbs = Numerical::fabs(alpha.at(breakpoint->variableIndex)) / m_variableAge[breakpoint->variableIndex];
            }
        }else{
            break;
        }
    }

    if (maxAlphaAbs < m_pivotThreshold) {
//        m_stablePivotNotFoundPhase2++;
//        m_incomingVariableIndex = -1;
        //step back
        int prevIterationCounter = iterationCounter--;
        m_boundflips.pop_back();
        while (prevIterationCounter >= 0) {
            const BreakpointHandler::BreakPoint * prevBreakpoint = m_breakpointHandler.getBreakpoint(prevIterationCounter);
            int prevBreakpointId = prevBreakpoint->variableIndex;
            if (Numerical::fabs(alpha.at(prevBreakpointId)) <= m_pivotThreshold) {
                prevIterationCounter--;
                m_boundflips.pop_back();
            }else{
                LPINFO("Stable pivot chosen");
                m_incomingVariableIndex = prevBreakpointId;
                m_dualSteplength = prevBreakpoint->value;
                break;
            }
        }
        if (prevIterationCounter < 0){
            m_incomingVariableIndex = -1;
            m_stablePivotNotFoundPhase2++;
            LPWARNING("No stable pivot found in phase 2");
        }


    }else{
        m_incomingVariableIndex = m_breakpointHandler.getBreakpoint(maxAlphaId)->variableIndex;
    }
}

void DualRatiotest::performRatiotestPhase2(unsigned int outgoingVariableIndex,
                                           const Vector& alpha,
                                           Numerical::Double phaseIIObjectiveValue)
{
    m_boundflips.clear();
    m_boundflips.reserve(alpha.nonZeros());
    m_tPositive = false;
    m_phaseIIObjectiveValue = phaseIIObjectiveValue;

    m_incomingVariableIndex = -1;
    m_dualSteplength = 0;
    Numerical::Double functionSlope = 0;

    unsigned int iterationCounter = 0;

//determining t>0 or t<0 cases
    const Variable & outgoingVariable = m_model.getVariable(outgoingVariableIndex);
    Numerical::Double outgoingVariableValue = *(m_variableStates.getAttachedData(outgoingVariableIndex));

    if ( outgoingVariableValue < outgoingVariable.getLowerBound()) {
        functionSlope = -(outgoingVariableValue - outgoingVariable.getLowerBound());
        m_tPositive = false;

    } else if (outgoingVariableValue > outgoingVariable.getUpperBound()) {
        functionSlope = outgoingVariableValue - outgoingVariable.getUpperBound();
        m_tPositive = true;
    }

    generateSignedBreakpointsPhase2(alpha);

    //Slope check should be enabled in debug mode
#ifndef NDEBUG
    if (functionSlope < 0) {
        LPERROR("SLOPE IS NEGATIVE ERROR - TERMINATING!!! s: "<<functionSlope);
    }
#endif

    //free variables always enter the basis
    if (m_incomingVariableIndex == -1) {
        if (m_breakpointHandler.getNumberOfBreakpoints() > 0) {
            m_breakpointHandler.selectMethod();
            //Handle fake feasible breakpoints
            if (m_enableFakeFeasibility) {
                const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
                int fakeFesibilityCounter = 0;

                while ( breakpoint->value < 0) {
                    if (Numerical::fabs(m_reducedCosts.at(breakpoint->variableIndex)) > optimalityTolerance){
                        throw FallbackException("Infeasible variable in phase 2");
                    }
                    fakeFesibilityCounter++;

                    const Variable & variable = m_model.getVariable(breakpoint->variableIndex);

                    functionSlope -= Numerical::fabs(alpha.at(breakpoint->variableIndex)) *
                            (variable.getUpperBound() - variable.getLowerBound());
                    iterationCounter++;

                    if(iterationCounter < m_breakpointHandler.getNumberOfBreakpoints()){
                        breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
                    } else {
                        break;
                    }
                }

                if(fakeFesibilityCounter > 0){
                    m_fakeFeasibilityActivationPhase2++;
                    m_fakeFeasibilityCounterPhase2+=fakeFesibilityCounter;
                }

                if( functionSlope < 0 || iterationCounter == m_breakpointHandler.getNumberOfBreakpoints()){
                    LPINFO("Fake feasible slope: "<<functionSlope);
                    m_incomingVariableIndex = -1;
                    return;
                }
            }

            if(iterationCounter < m_breakpointHandler.getNumberOfBreakpoints()){
                switch (nonlinearDualPhaseIIFunction) {
                    case ONE_STEP:{
                        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);

                        m_phaseIIObjectiveValue += functionSlope * breakpoint->value;
                        breakpoint->functionValue = m_phaseIIObjectiveValue;
                        m_incomingVariableIndex = breakpoint->variableIndex;
                        m_dualSteplength = breakpoint->value;
                        break;
                    }

                    case PIECEWISE_LINEAR_FUNCTION:{
                        computeFunctionPhase2(alpha,iterationCounter,functionSlope);
                        break;
                    }

                    case STABLE_PIVOT:{
                        computeFunctionPhase2(alpha,iterationCounter,functionSlope);
                        useNumericalThresholdPhase2(iterationCounter, alpha);
                        break;
                    }
                }
            }
        } else{
            LPWARNING(" - Ratiotest - No breakpoint found!");
            throw DualUnboundedException("No breakpoint found!");
        }
        if(m_incomingVariableIndex != -1 && Numerical::fabs(alpha.at(m_incomingVariableIndex)) < 1e-6){
            LPWARNING("small pivot in phase2: "<<alpha.at(m_incomingVariableIndex));
        }
    }
}


DualRatiotest::~DualRatiotest() {

}
