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

DualRatiotest::DualRatiotest(const SimplexModel & model,
                             const Vector& reducedCosts,
                             const IndexList<>& reducedCostFeasibilities,
                             const IndexList<const Numerical::Double*>& variableStates) :
    m_model(model),
    m_reducedCosts(reducedCosts),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_variableStates(variableStates),
    m_tPositive(false),
    m_variableAge(model.getColumnCount() + model.getRowCount(),1),
    m_incomingVariableIndex(-1),
    m_dualSteplength(0),
    m_phaseIObjectiveValue(0),
    m_phaseIIObjectiveValue(0),
    m_stablePivotActivationPhase1(0),
    m_stablePivotBackwardStepsPhase1(0),
    m_stablePivotForwardStepsPhase1(0),
    m_fakeFeasibilityActivationPhase1(0),
    m_fakeFeasibilityCounterPhase1(0),
    m_stablePivotNotFoundPhase2(0),
    m_fakeFeasibilityActivationPhase2(0),
    m_fakeFeasibilityCounterPhase2(0),
    m_nonlinearDualPhaseIFunction(static_cast<DUAL_RATIOTEST_METHOD>
                                  (SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_dual_phaseI_function"))),
    m_nonlinearDualPhaseIIFunction(static_cast<DUAL_RATIOTEST_METHOD>
                                   (SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_dual_phaseII_function"))),
    m_optimalityTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("e_optimality")),
    m_pivotTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("e_pivot")),
    m_enableFakeFeasibility(SimplexParameterHandler::getInstance().getIntegerParameterValue("enable_fake_feasibility")),
    m_expandDualPhase1(SimplexParameterHandler::getInstance().getIntegerParameterValue("expand_dual_phaseI")),
    m_expandDualPhase2(SimplexParameterHandler::getInstance().getIntegerParameterValue("expand_dual_phaseII"))
{

}

void DualRatiotest::generateSignedBreakpointsPhase1(const Vector& alpha){
    #ifndef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

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
            if ( alpha.at(variableIndex) != 0 ) {
                valueOfVariable = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();

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
            if (alpha.at(variableIndex) != 0 ) {
                valueOfVariable = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();

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

void DualRatiotest::generateExpandedBreakpointsPhase1(const Vector &alpha,
                                                      Numerical::Double workingTolerance,
                                                      bool secondTurn)
{
    #ifndef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    //in theory only positive breakpoints can be defined if not, fake feasibility is present
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
                valueOfVariable = (m_reducedCosts.at(variableIndex) + workingTolerance) / alpha.at(variableIndex);
                if(secondTurn){
                    if(valueOfVariable < m_dualSteplength){
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                }else{
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                }
                if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                    valueOfVariable = (m_reducedCosts.at(variableIndex) - workingTolerance) / alpha.at(variableIndex);
                    if(secondTurn){
                        if(valueOfVariable < m_dualSteplength){
                            m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                        }
                    }else{
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                }
            }
        }

    //computing ratios in P
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::PLUS);

        for (; it!=endit; it++){
            variableIndex = it.getData();
            if (alpha.at(variableIndex) > 0){
                valueOfVariable = (m_reducedCosts.at(variableIndex) - workingTolerance) / alpha.at(variableIndex);
                if(secondTurn){
                    if(valueOfVariable < m_dualSteplength){
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                }else{
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                }
                if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                    valueOfVariable = (m_reducedCosts.at(variableIndex) + workingTolerance) / alpha.at(variableIndex);
                    if(secondTurn){
                        if(valueOfVariable < m_dualSteplength){
                            m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                        }
                    }else{
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                }
             }
        }

    //computing ratios in F
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);

        for (; it!=endit; it++){
            variableIndex = it.getData();
            if ( alpha.at(variableIndex) != 0 ) {
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();

                if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) > 0) {
                    valueOfVariable = (m_reducedCosts.at(variableIndex) - workingTolerance) / alpha.at(variableIndex);
                    if(secondTurn){
                        if(valueOfVariable < m_dualSteplength){
                            m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                        }
                    }else{
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                } else
                if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) < 0) {
                    valueOfVariable = (m_reducedCosts.at(variableIndex) + workingTolerance) / alpha.at(variableIndex);
                    if(secondTurn){
                        if(valueOfVariable < m_dualSteplength){
                            m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                        }
                    }else{
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                } else
                if (typeOfIthVariable == Variable::FREE ) {
                    valueOfVariable = alpha.at(variableIndex) < 0 ?
                                ( m_reducedCosts.at(variableIndex) + workingTolerance) / alpha.at(variableIndex) :
                        (m_reducedCosts.at(variableIndex) - workingTolerance) / alpha.at(variableIndex);
                    if(secondTurn){
                        if(valueOfVariable < m_dualSteplength){
                            m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                        }
                    }else{
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
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
                valueOfVariable = - (m_reducedCosts.at(variableIndex) + workingTolerance) / alpha.at(variableIndex);
                if(secondTurn){
                    if(valueOfVariable < m_dualSteplength){
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                }else{
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                }
                if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                    valueOfVariable = -(m_reducedCosts.at(variableIndex) - workingTolerance) / alpha.at(variableIndex);
                    if(secondTurn){
                        if(valueOfVariable < m_dualSteplength){
                            m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                        }
                    }else{
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                }
            }
        }

    //computing ratios in P
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::PLUS);

        for (; it!=endit; it++) {
            variableIndex = it.getData();
            if (alpha.at(variableIndex) < 0) {
                valueOfVariable = - (m_reducedCosts.at(variableIndex) - workingTolerance) / alpha.at(variableIndex);
                if(secondTurn){
                    if(valueOfVariable < m_dualSteplength){
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                }else{
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                }
                if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                    valueOfVariable = -(m_reducedCosts.at(variableIndex) + workingTolerance) / alpha.at(variableIndex);
                    if(secondTurn){
                        if(valueOfVariable < m_dualSteplength){
                            m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                        }
                    }else{
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                }
            }
        }

    //computing ratios in F
        m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);

        for (; it!=endit; it++) {
            variableIndex = it.getData();
            if (alpha.at(variableIndex) != 0 ) {
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();

                if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) < 0) {
                    valueOfVariable = -(m_reducedCosts.at(variableIndex) - workingTolerance) / alpha.at(variableIndex);
                    if(secondTurn){
                        if(valueOfVariable < m_dualSteplength){
                            m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                        }
                    }else{
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                } else
                if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) > 0) {
                    valueOfVariable = -(m_reducedCosts.at(variableIndex) + workingTolerance) / alpha.at(variableIndex);
                    if(secondTurn){
                        if(valueOfVariable < m_dualSteplength){
                            m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                        }
                    }else{
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                } else
                if (typeOfIthVariable == Variable::FREE ) {
                    valueOfVariable = alpha.at(variableIndex) < 0 ? -(m_reducedCosts.at(variableIndex) + workingTolerance) / alpha.at(variableIndex) :
                            -(m_reducedCosts.at(variableIndex) - workingTolerance) / alpha.at(variableIndex);
                    if(secondTurn){
                        if(valueOfVariable < m_dualSteplength){
                            m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                        }
                    }else{
                        m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                    }
                }
            }
        }
    }
}

void DualRatiotest::computeFunctionPhase1(const Vector& alpha,
                                          unsigned int& iterationCounter,
                                          Numerical::Double& functionSlope)
{
    unsigned int numberOfBreakpoints = m_breakpointHandler.getNumberOfBreakpoints();
    Numerical::Double t_prev = 0;
    Numerical::Double t_actual = 0;
    const BreakpointHandler::BreakPoint * actualBreakpoint = NULL;

    while (functionSlope >= 0 && iterationCounter < numberOfBreakpoints) {
        actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
        t_actual = actualBreakpoint->value;

        m_phaseIObjectiveValue += functionSlope * (t_actual - t_prev);
        actualBreakpoint->functionValue = m_phaseIObjectiveValue;

        functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));

        iterationCounter++;
        t_prev = t_actual;
    }
    if (actualBreakpoint != NULL){
        m_incomingVariableIndex = actualBreakpoint->variableIndex;
        m_dualSteplength = m_tPositive ? actualBreakpoint->value : - actualBreakpoint->value;
    } else{
        LPERROR("In phase 1 NO function defined, num of bpts: "<<m_breakpointHandler.getNumberOfBreakpoints());
    }
}

void DualRatiotest::useNumericalThresholdPhase1(unsigned int iterationCounter,
                                          const Vector& alpha,
                                          Numerical::Double& functionSlope)
{
    m_stablePivotActivationPhase1++;
    LPINFO("Stable pivot activated in phase 1");

    unsigned int numberOfBreakpoints = m_breakpointHandler.getNumberOfBreakpoints();
    unsigned int prevBreakpointId = iterationCounter;
    unsigned int nextBreakpointId = iterationCounter;
    Numerical::Double prevObjValue = - Numerical::Infinity;
    Numerical::Double nextObjValue = - Numerical::Infinity;

    //If there is any previous breakpoint, initalize the first one
    if (prevBreakpointId > 0) {
        prevBreakpointId--;
        prevObjValue = m_breakpointHandler.getBreakpoint(prevBreakpointId)->functionValue;
    }
    //If there is any next breakpoint, initalize the first one
    if (iterationCounter < numberOfBreakpoints-1) {
        Numerical::Double t_prev = m_breakpointHandler.getBreakpoint(iterationCounter)->value;
        nextBreakpointId++;

        const BreakpointHandler::BreakPoint * actualBreakpoint = m_breakpointHandler.getBreakpoint(nextBreakpointId);
        Numerical::Double t_actual = actualBreakpoint->value;

        nextObjValue = m_phaseIObjectiveValue + functionSlope * (t_actual - t_prev);
        actualBreakpoint->functionValue = nextObjValue;

        functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));
        if(nextObjValue < m_initialPhaseIObjectiveValue){
            nextObjValue = -Numerical::Infinity;
        }
    }

    //Prefer the selection o the previous breakpoint hence it is already available
    bool prevIsBetter = (prevObjValue >= nextObjValue ? true : false);

    //Search while there is any potential positive step
    while (prevObjValue > - Numerical::Infinity || nextObjValue > - Numerical::Infinity) {
        //If the better objective value has a good pivot, stop searching
        if(prevIsBetter){
            if(Numerical::fabs(alpha.at(prevBreakpointId)) < m_pivotTolerance){
                break;
            }
        } else {
            if(Numerical::fabs(alpha.at(nextBreakpointId)) < m_pivotTolerance){
                break;
            }
        }
        //One step backward
        if (prevIsBetter) {
            m_stablePivotBackwardStepsPhase1++;
            if (prevBreakpointId > 0) {
                prevBreakpointId--;
                prevObjValue = m_breakpointHandler.getBreakpoint(prevBreakpointId)->functionValue;
            } else {
                prevObjValue = -Numerical::Infinity;
            }
        //One step forward
        } else {
            m_stablePivotForwardStepsPhase1++;
            if (nextBreakpointId < numberOfBreakpoints - 1) {
                Numerical::Double t_prev = m_breakpointHandler.getBreakpoint(nextBreakpointId)->value;
                nextBreakpointId++;

                const BreakpointHandler::BreakPoint * actualBreakpoint = m_breakpointHandler.getBreakpoint(nextBreakpointId);
                Numerical::Double t_actual = actualBreakpoint->value;

                nextObjValue += functionSlope * (t_actual - t_prev);
                actualBreakpoint->functionValue = nextObjValue;

                functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));

                if (nextObjValue < m_initialPhaseIObjectiveValue) {
                    nextObjValue = -Numerical::Infinity;
                }
            } else {
                nextObjValue = -Numerical::Infinity;
            }
        }

        //Prefer the selection o the previous breakpoint hence it is already available
        prevIsBetter = (prevObjValue >= nextObjValue ? true : false);
    }

    //No acceptable pivot found with positive step
    if ((prevObjValue == - Numerical::Infinity) && (nextObjValue == - Numerical::Infinity)) {
        LPWARNING("No stable pivot found in phase 1!");
        m_incomingVariableIndex = -1;
        m_dualSteplength = 0.0;
        m_phaseIObjectiveValue = m_initialPhaseIObjectiveValue;
    //A good pivot is found backward
    } else if (prevIsBetter) {
        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(prevBreakpointId);
        m_incomingVariableIndex = breakpoint->variableIndex;
        m_dualSteplength = m_tPositive ? breakpoint->value : - breakpoint->value;
        m_phaseIObjectiveValue = breakpoint->functionValue;
    //A good pivot is found forward
    } else {
        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(nextBreakpointId);
        m_incomingVariableIndex = breakpoint->variableIndex;
        m_dualSteplength = m_tPositive ? breakpoint->value : - breakpoint->value;
        m_phaseIObjectiveValue = breakpoint->functionValue;
    }
}

void DualRatiotest::performRatiotestPhase1(const Vector& alpha,
                                           Numerical::Double phaseIReducedCost,
                                           Numerical::Double phaseIObjectiveValue,
                                           Numerical::Double workingTolerance)
{
    m_breakpointHandler.init(2*alpha.nonZeros());
    Numerical::Double functionSlope = Numerical::fabs(phaseIReducedCost);
    unsigned iterationCounter = 0;

    m_initialPhaseIObjectiveValue = phaseIObjectiveValue;
    m_phaseIObjectiveValue = phaseIObjectiveValue;
    m_incomingVariableIndex = -1;
    m_dualSteplength = 0;

    //determining t>=0 or t<=0 cases
    if (phaseIReducedCost > 0) {
        m_tPositive = false;
    } else{
        m_tPositive = true;
    }

    //Generate the breakpoints of the piecewise linear concave function
    if (m_expandDualPhase1 > 0){
        generateExpandedBreakpointsPhase1(alpha,workingTolerance);
    } else{
        generateSignedBreakpointsPhase1(alpha);
    }

    //If is there any breakpoint
    if (m_breakpointHandler.getNumberOfBreakpoints() > 0) {
        //Initialize the handler to provide sorted breakpoints
        m_breakpointHandler.selectMethod(m_nonlinearDualPhaseIFunction);
        //Handle fake feasible variables if enables
        if (m_enableFakeFeasibility) {
            const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
            int fakeFeasibilityCounter = 0;

            //If the first breakpoint is fake feasible filter them
            while (breakpoint->value < 0 && functionSlope >=0 ) {
                LPINFO("Fake feasibility detected: " << breakpoint->value);
                fakeFeasibilityCounter++;

                functionSlope -= Numerical::fabs(alpha.at(breakpoint->variableIndex));
                iterationCounter++;

                //Check the next breakpoint for fake feasibility
                if(iterationCounter < m_breakpointHandler.getNumberOfBreakpoints()){
                    breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
                } else {
                    //If all breakpoints were fake feasible return
                    return;
                }
            }

            //Update the fake feasibility report
            if(fakeFeasibilityCounter > 0){
                m_fakeFeasibilityActivationPhase1++;
                m_fakeFeasibilityCounterPhase1+=fakeFeasibilityCounter;
            }

            //If the slope gone to negative return
            if( functionSlope < 0){
                return;
            }
        }

        //IterationCounter points to the first non-fake feasible breakpoint if there is any (otherwise returned)
        switch (m_nonlinearDualPhaseIFunction){
            case ONE_STEP:{
                const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);

                m_phaseIObjectiveValue += functionSlope * breakpoint->value;
                breakpoint->functionValue = m_phaseIObjectiveValue;

                m_incomingVariableIndex = breakpoint->variableIndex;
                m_dualSteplength = m_tPositive ? breakpoint->value : - breakpoint->value;
                break;
            }

            case PIECEWISE_LINEAR_FUNCTION:{
                computeFunctionPhase1(alpha, iterationCounter, functionSlope);
                break;
            }

            case STABLE_PIVOT:{
                computeFunctionPhase1(alpha, iterationCounter, functionSlope);

                if( Numerical::fabs(alpha.at(m_incomingVariableIndex)) < m_pivotTolerance ) {
                    useNumericalThresholdPhase1(iterationCounter, alpha, functionSlope);
                }
                break;
            }
        }
        //Harris, expand second turn with exact bounds
//        generateExpandedBreakpointsPhase1(alpha,0,true);

//        m_breakpointHandler.selectMethod(m_nonlinearDualPhaseIFunction);

    } else{
        LPWARNING(" - Ratiotest - No breakpoint found!");
    }

//    if(m_incomingVariableIndex != -1 && Numerical::fabs(alpha.at(m_incomingVariableIndex)) < 1e-6){
//        LPWARNING("small pivot in phase1: "<<alpha.at(m_incomingVariableIndex));
//    }
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
//            LPINFO("Free variable selected to enter basis");
            Numerical::Double steplength = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
            //A free variable is selected only if it defines a dual steplength with proper sign
            if (m_tPositive && steplength > 0) {
                m_incomingVariableIndex = variableIndex;
                m_dualSteplength = steplength;
                break;
            } else if(steplength < 0){
                m_incomingVariableIndex = variableIndex;
                m_dualSteplength = steplength;
                break;
            }
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
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                } else
                if (it.getPartitionIndex() == Simplex::NONBASIC_AT_UB && alpha.at(variableIndex) < 0) {
                    valueOfVariable = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
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
                    m_breakpointHandler.insertBreakpoint(variableIndex,valueOfVariable);
                } else
                if (it.getPartitionIndex() == Simplex::NONBASIC_AT_UB && alpha.at(variableIndex) > 0) {
                    valueOfVariable = - m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
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

    if(iterationCounter < length){
        breakpoint = m_breakpointHandler.getBreakpoint(maxAlphaId);
        maxAlphaAbs = Numerical::fabs(alpha.at(breakpoint->variableIndex)) / m_variableAge[breakpoint->variableIndex];
        t_actual = breakpoint->value;
    }

    unsigned int nextIterationCounter = iterationCounter;

    //Step forward if there are more breakpoints at the same point
    while (nextIterationCounter < length-1)  {
        nextIterationCounter++;

        breakpoint = m_breakpointHandler.getBreakpoint(nextIterationCounter);
        Numerical::Double t_next = breakpoint->value;
        if ( t_actual == t_next ) {
            breakpoint->functionValue = m_phaseIIObjectiveValue;

            if (Numerical::fabs(alpha.at(breakpoint->variableIndex)) / m_variableAge[breakpoint->variableIndex] > maxAlphaAbs){
                maxAlphaId = nextIterationCounter;
                maxAlphaAbs = Numerical::fabs(alpha.at(breakpoint->variableIndex)) / m_variableAge[breakpoint->variableIndex];
            }
        }else{
            break;
        }
    }

    if (maxAlphaAbs > m_pivotTolerance){
        m_incomingVariableIndex = m_breakpointHandler.getBreakpoint(maxAlphaId)->variableIndex;
        m_dualSteplength = m_tPositive ? m_breakpointHandler.getBreakpoint(maxAlphaId)->value : - m_breakpointHandler.getBreakpoint(maxAlphaId)->value;
        return;
    }

    //Step back if there is any boundflip selected
    if (m_boundflips.size()>0) {
        int prevIterationCounter = iterationCounter;
        while (prevIterationCounter > 0) {
            prevIterationCounter--;
            m_boundflips.pop_back();
            const BreakpointHandler::BreakPoint * prevBreakpoint = m_breakpointHandler.getBreakpoint(prevIterationCounter);
            int prevBreakpointId = prevBreakpoint->variableIndex;
            if (Numerical::fabs(alpha.at(prevBreakpointId)) > m_pivotTolerance) {
                LPINFO("Stable pivot chosen");
                m_incomingVariableIndex = prevBreakpointId;
                m_dualSteplength = m_tPositive ? prevBreakpoint->value : - prevBreakpoint->value;
                return;
            }
        }
        if (prevIterationCounter == 0){
            m_incomingVariableIndex = -1;
            m_dualSteplength = 0;
            m_stablePivotNotFoundPhase2++;
            LPWARNING("No stable pivot fbound in phase 2");
        }
    } else {
        m_incomingVariableIndex = -1;
        m_dualSteplength = 0;
        m_stablePivotNotFoundPhase2++;
        LPWARNING("No stable pivot found in phase 2");
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
            m_breakpointHandler.selectMethod(m_nonlinearDualPhaseIIFunction);
            //Handle fake feasible breakpoints
            if (m_enableFakeFeasibility) {
                const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
                int fakeFeasibilityCounter = 0;

                while ( breakpoint->value < 0) {
                    if (Numerical::fabs(m_reducedCosts.at(breakpoint->variableIndex)) > m_optimalityTolerance){
                        throw FallbackException("Infeasible variable in phase 2");
                    }
                    fakeFeasibilityCounter++;

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

                if(fakeFeasibilityCounter > 0){
                    m_fakeFeasibilityActivationPhase2++;
                    m_fakeFeasibilityCounterPhase2+=fakeFeasibilityCounter;
                }

                if( functionSlope < 0 || iterationCounter == m_breakpointHandler.getNumberOfBreakpoints()){
                    LPINFO("Fake feasible slope: "<<functionSlope);
                    m_incomingVariableIndex = -1;
                    m_dualSteplength = 0;
                    return;
                }
            }

            if(iterationCounter < m_breakpointHandler.getNumberOfBreakpoints()){
                switch (m_nonlinearDualPhaseIIFunction) {
                    case ONE_STEP:{
                        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);

                        m_phaseIIObjectiveValue += functionSlope * breakpoint->value;
                        breakpoint->functionValue = m_phaseIIObjectiveValue;
                        m_incomingVariableIndex = breakpoint->variableIndex;
                        m_dualSteplength = m_tPositive ? breakpoint->value : - breakpoint->value;
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
//        if(m_incomingVariableIndex != -1 && Numerical::fabs(alpha.at(m_incomingVariableIndex)) < 1e-6){
//            LPWARNING("small pivot in phase2: "<<alpha.at(m_incomingVariableIndex));
//        }
    }
}


DualRatiotest::~DualRatiotest() {

}
