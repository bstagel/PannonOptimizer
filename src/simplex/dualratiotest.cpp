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
#include <prettyprint.h>

DualRatiotest::DualRatiotest(const SimplexModel & model,
                             const Vector& reducedCosts,
                             const IndexList<>& reducedCostFeasibilities,
                             const IndexList<const Numerical::Double*>& variableStates) :
    m_model(model),
    m_reducedCosts(reducedCosts),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_variableStates(variableStates),
    m_sigma(1),
    m_incomingVariableIndex(-1),
    m_variableAge(model.getColumnCount() + model.getRowCount(),1),
    m_dualSteplength(0),
    m_phaseIObjectiveValue(0),
    m_stablePivotActivationPhase1(0),
    m_stablePivotBackwardStepsPhase1(0),
    m_stablePivotForwardStepsPhase1(0),
    m_fakeFeasibilityActivationPhase1(0),
    m_fakeFeasibilityCounterPhase1(0),
    m_stablePivotNotFoundPhase2(0),
    m_fakeFeasibilityActivationPhase2(0),
    m_fakeFeasibilityCounterPhase2(0),
    m_ePivotGeneration(SimplexParameterHandler::getInstance().getBoolParameterValue("Ratiotest.e_pivot_generation")),
    m_nonlinearDualPhaseIFunction(getDualRatiotestMethod(
                                      SimplexParameterHandler::getInstance().getStringParameterValue("Ratiotest.nonlinear_dual_phaseI_function"))),
    m_nonlinearDualPhaseIIFunction(getDualRatiotestMethod(
                                       SimplexParameterHandler::getInstance().getStringParameterValue("Ratiotest.nonlinear_dual_phaseII_function"))),
    m_optimalityTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_optimality")),
    m_pivotTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_pivot")),
    m_enableFakeFeasibility(SimplexParameterHandler::getInstance().getBoolParameterValue("Ratiotest.enable_fake_feasibility")),
    m_expand(SimplexParameterHandler::getInstance().getStringParameterValue("Ratiotest.Expand.type")),
    m_toleranceStep(m_optimalityTolerance * (1 - SimplexParameterHandler::getInstance().getDoubleParameterValue("Ratiotest.Expand.multiplier")) /
                    SimplexParameterHandler::getInstance().getIntegerParameterValue("Ratiotest.Expand.divider")),
    m_degenerate(false)
{
    m_ratioDirections.resize(m_model.getColumnCount() + m_model.getRowCount());
}

void DualRatiotest::generateSignedBreakpointsPhase1(const Vector& alpha){
    #ifndef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

//computing ratios
    IndexList<>::PartitionIterator it;
    IndexList<>::PartitionIterator endit;
    Variable::VARIABLE_TYPE typeOfIthVariable;
    Numerical::Double epsilon = 0;
    if(m_ePivotGeneration){
        epsilon = m_pivotTolerance;
    }

    //computing ratios in M
    m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::MINUS);
    for (; it != endit; ++it){
        int variableIndex = it.getData();
        Numerical::Double signedAlpha = m_sigma * alpha.at(variableIndex);
        if (signedAlpha < -epsilon) {
            m_ratioDirections[variableIndex] = 1;
            m_breakpointHandler.insertBreakpoint(variableIndex,
                                                 m_reducedCosts.at(variableIndex) / signedAlpha);
            if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                m_breakpointHandler.insertBreakpoint(variableIndex,
                                                     m_reducedCosts.at(variableIndex) / signedAlpha);
            }
        }
    }

    //computing ratios in P
    m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::PLUS);
    for (; it!=endit; ++it){
        int variableIndex = it.getData();
        Numerical::Double signedAlpha = m_sigma * alpha.at(variableIndex);
        if (signedAlpha > epsilon){
            m_ratioDirections[variableIndex] = 0;
            m_breakpointHandler.insertBreakpoint(variableIndex,
                                                 m_reducedCosts.at(variableIndex) / signedAlpha);
            if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                m_breakpointHandler.insertBreakpoint(variableIndex,
                                                     m_reducedCosts.at(variableIndex) / signedAlpha);
            }
         }
    }

    //computing ratios in F
    m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);
    for (; it!=endit; ++it){
        int variableIndex = it.getData();
        Numerical::Double signedAlpha = m_sigma * alpha.at(variableIndex);
        if ( Numerical::fabs(alpha.at(variableIndex)) > epsilon ) {
            typeOfIthVariable = m_model.getVariable(variableIndex).getType();

            if (typeOfIthVariable == Variable::PLUS && signedAlpha > epsilon) {
                m_ratioDirections[variableIndex] = 0;
                m_breakpointHandler.insertBreakpoint(variableIndex,
                                                     m_reducedCosts.at(variableIndex) / signedAlpha);
            } else
            if (typeOfIthVariable == Variable::MINUS && signedAlpha < -epsilon) {
                m_ratioDirections[variableIndex] = 1;
                m_breakpointHandler.insertBreakpoint(variableIndex,
                                                     m_reducedCosts.at(variableIndex) / signedAlpha);
            } else
            if (typeOfIthVariable == Variable::FREE) {
                m_breakpointHandler.insertBreakpoint(variableIndex,
                                                     m_reducedCosts.at(variableIndex) / signedAlpha);
                if(signedAlpha > epsilon){
                    m_ratioDirections[variableIndex] = 0;
                }else{
                    m_ratioDirections[variableIndex] = 1;
                }
            }
        }
    }

    m_breakpointHandler.finalizeBreakpoints();
}

void DualRatiotest::computeFunctionPhase1(const Vector& alpha,
                                          unsigned int& iterationCounter,
                                          Numerical::Double& functionSlope)
{

    unsigned int numberOfBreakpoints = m_breakpointHandler.getNumberOfBreakpoints();
    Numerical::Double t_prev = 0;
    Numerical::Double t_actual = 0;
    const BreakpointHandler::BreakPoint * actualBreakpoint = NULL;

    bool original = true;
    if(!original){
        Numerical::Double t_next = 0;

        std::vector<std::pair<int,Numerical::Double> > ratios;

        int firstCandidate = -1;
        int lastCandidate = -1;

        while (iterationCounter < numberOfBreakpoints) {
            actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
            t_actual = actualBreakpoint->value;
            ratios.push_back(std::pair<int,Numerical::Double>(actualBreakpoint->variableIndex,t_actual));

            m_phaseIObjectiveValue += functionSlope * (t_actual - t_prev);
            actualBreakpoint->additionalValue = m_phaseIObjectiveValue;

            functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));

            if(functionSlope <= 0){
                if(firstCandidate == -1){
                    firstCandidate = iterationCounter;
                    //First candidate values can occur before slope turns not positive
                    Numerical::Double firstCandidateValue = t_actual;
                    for(int i=iterationCounter-1; i>=0; i--){
                        if(m_breakpointHandler.getBreakpoint(i)->value == firstCandidateValue){
                            firstCandidate = i;
                        } else {
                            break;
                        }
                    }
                }
                if(iterationCounter+1 < numberOfBreakpoints){
                    t_next = m_breakpointHandler.getBreakpoint(iterationCounter+1)->value;
                    if(t_next != t_actual && functionSlope < 0){
                        lastCandidate = iterationCounter;
                        break;
                    }
                } else {
                    lastCandidate = numberOfBreakpoints-1;
                    break;
                }
            }

            ++iterationCounter;
            t_prev = t_actual;
        }
    //    LPINFO("firstCandidate "<<firstCandidate);
    //    LPINFO("lastCandidate "<<lastCandidate);
        //Search the best incoming alternative
        if(firstCandidate != -1){
            for(int i=firstCandidate; i<=lastCandidate; i++){
                actualBreakpoint = m_breakpointHandler.getBreakpoint(i);
                if(m_reducedCostFeasibilities.where(actualBreakpoint->variableIndex) != Simplex::FEASIBLE){
                    break;
                }
            }
            actualBreakpoint = m_breakpointHandler.getBreakpoint(firstCandidate);
        }
    //    LPINFO("m_incomingVariableIndex "<<m_incomingVariableIndex);
        if (actualBreakpoint != NULL){
            m_incomingVariableIndex = actualBreakpoint->variableIndex;
            m_dualSteplength = m_sigma * actualBreakpoint->value;
            //adding variables to the update vector if their feasibility states are changed
            int variableIndex = -1;
            for(unsigned i = 0; i < ratios.size(); i++){
                variableIndex = ratios[i].first;
                if(variableIndex < firstCandidate){
                    m_updateFeasibilitySets.push_back(std::pair<int,char>(variableIndex,m_ratioDirections[variableIndex]));
                }else{
                    if(variableIndex != m_incomingVariableIndex){
                        m_becomesFeasible.push_back(variableIndex);
                    }
                }
            }
        } else{
            LPERROR("In phase 1 NO function defined, num of bpts: "<<m_breakpointHandler.getNumberOfBreakpoints());
        }
//        m_breakpointHandler.printBreakpoints();
//        LPINFO(m_updateFeasibilitySets);
//        LPINFO(m_becomesFeasible);
    } else {
    //Original version

        while(functionSlope > 0 && iterationCounter < numberOfBreakpoints){
            actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
            t_actual = actualBreakpoint->value;

            m_phaseIObjectiveValue += functionSlope * (t_actual - t_prev);
            actualBreakpoint->additionalValue = m_phaseIObjectiveValue;

            functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));

            iterationCounter++;
            t_prev = t_actual;
        }

        if (actualBreakpoint != NULL){
          m_incomingVariableIndex = actualBreakpoint->variableIndex;
          m_dualSteplength = m_sigma * actualBreakpoint->value;
        } else{
          LPERROR("In phase 1 NO function defined, num of bpts: "<<m_breakpointHandler.getNumberOfBreakpoints());
        }
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
        prevObjValue = m_breakpointHandler.getBreakpoint(prevBreakpointId)->additionalValue;
    }
    //If there is any next breakpoint, initalize the first one
    if (iterationCounter < numberOfBreakpoints-1) {
        Numerical::Double t_prev = m_breakpointHandler.getBreakpoint(iterationCounter)->value;
        nextBreakpointId++;

        const BreakpointHandler::BreakPoint * actualBreakpoint = m_breakpointHandler.getBreakpoint(nextBreakpointId);
        Numerical::Double t_actual = actualBreakpoint->value;

        nextObjValue = m_phaseIObjectiveValue + functionSlope * (t_actual - t_prev);
        actualBreakpoint->additionalValue = nextObjValue;

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
                prevObjValue = -Numerical::Infinity;
                break;
            }
        } else {
            if(Numerical::fabs(alpha.at(nextBreakpointId)) < m_pivotTolerance){
                nextObjValue = -Numerical::Infinity;
                break;
            }
        }
        //One step backward
        if (prevIsBetter) {
            m_stablePivotBackwardStepsPhase1++;
            if (prevBreakpointId > 0) {
                prevBreakpointId--;
                prevObjValue = m_breakpointHandler.getBreakpoint(prevBreakpointId)->additionalValue;
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
                actualBreakpoint->additionalValue = nextObjValue;

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
        m_dualSteplength = m_sigma * breakpoint->value;
        m_phaseIObjectiveValue = breakpoint->additionalValue;
    //A good pivot is found forward
    } else {
        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(nextBreakpointId);
        m_incomingVariableIndex = breakpoint->variableIndex;
        m_dualSteplength = m_sigma * breakpoint->value;
        m_phaseIObjectiveValue = breakpoint->additionalValue;
    }
}

void DualRatiotest::performRatiotestPhase1(const Vector& alpha,
                                           Numerical::Double phaseIReducedCost,
                                           Numerical::Double phaseIObjectiveValue)
{
    m_becomesFeasible.clear();
    m_updateFeasibilitySets.clear();
    m_updateFeasibilitySets.reserve(2*alpha.nonZeros());
    m_breakpointHandler.init(2*alpha.nonZeros());
    Numerical::Double functionSlope = Numerical::fabs(phaseIReducedCost);
    unsigned iterationCounter = 0;

    m_initialPhaseIObjectiveValue = phaseIObjectiveValue;
    m_phaseIObjectiveValue = phaseIObjectiveValue;
    m_incomingVariableIndex = -1;
    m_dualSteplength = 0;

    //determining t>=0 or t<=0 cases
    if (phaseIReducedCost > 0) {
        m_sigma = -1;
    } else{
        m_sigma = 1;
    }

    //Generate the breakpoints of the piecewise linear concave function
    generateSignedBreakpointsPhase1(alpha);

    //If is there any breakpoint
    if (m_breakpointHandler.getNumberOfBreakpoints() > 0) {
        //Initialize the handler to provide sorted breakpoints
        m_breakpointHandler.initSorting();
        //Handle fake feasible variables if enables
        if (m_enableFakeFeasibility) {
            const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
            int fakeFeasibilityCounter = 0;

            //If the first breakpoint is fake feasible filter them
            while (breakpoint->value < 0 && functionSlope >=0 ) {
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
                breakpoint->additionalValue = m_phaseIObjectiveValue;

                m_incomingVariableIndex = breakpoint->variableIndex;
                m_dualSteplength = m_sigma * breakpoint->value;
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
        if(m_dualSteplength != 0){
            m_degenerate = false;
        }else{
            m_degenerate = true;
        }

    } else{
        LPWARNING(" - Ratiotest - No breakpoint found!");
    }
    if(m_incomingVariableIndex != -1 && Numerical::fabs(alpha.at(m_incomingVariableIndex)) < m_pivotTolerance){
        m_incomingVariableIndex = -1;
        m_dualSteplength = 0;
    }
}

void DualRatiotest::generateSignedBreakpointsPhase2(const Vector &alpha)
{
    #ifndef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

//computing ratios
    IndexList<const Numerical::Double*>::PartitionIterator it;
    IndexList<const Numerical::Double*>::PartitionIterator endit;
    Numerical::Double epsilon = 0;
    if(m_ePivotGeneration){
        epsilon = m_pivotTolerance;
    }

    //free variables always enter the basis
    m_variableStates.getIterators(&it,&endit,Simplex::NONBASIC_FREE);
    while (it != endit) {
        unsigned variableIndex = it.getData();
        if (Numerical::fabs(alpha.at(variableIndex)) > epsilon) {
            Numerical::Double steplength = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
            //A free variable is selected only if it defines a dual steplength with proper sign
            if (m_sigma * steplength >= 0) {
                m_incomingVariableIndex = variableIndex;
                m_dualSteplength = steplength;
                break;
            }
        }
        ++it;
    }

    if(m_incomingVariableIndex == -1){
        //NONBASIC_FIXED variables are ignored
        m_variableStates.getIterators(&it,&endit,Simplex::NONBASIC_AT_LB);
        while (it != endit) {
            int variableIndex = it.getData();
            Numerical::Double signedAlpha = m_sigma * alpha.at(variableIndex);
            if (signedAlpha > epsilon) {
                m_breakpointHandler.insertBreakpoint(variableIndex,
                                                     m_reducedCosts.at(variableIndex) / signedAlpha);
            }
            ++it;
        }

        m_variableStates.getIterators(&it,&endit,Simplex::NONBASIC_AT_UB);
        while (it != endit) {
            int variableIndex = it.getData();
            Numerical::Double signedAlpha = m_sigma * alpha.at(variableIndex);
            if (signedAlpha < -epsilon) {
                m_breakpointHandler.insertBreakpoint(variableIndex,
                                                     m_reducedCosts.at(variableIndex) / signedAlpha);
            }
            ++it;
        }
    }

    m_breakpointHandler.finalizeBreakpoints();
}

void DualRatiotest::generateExpandedBreakpointsPhase2(const Vector &alpha,
                                                      Numerical::Double workingTolerance)
{
    #ifndef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    //computing ratios
    IndexList<const Numerical::Double*>::PartitionIterator it;
    IndexList<const Numerical::Double*>::PartitionIterator endit;

    //free variables always enter the basis
    m_variableStates.getIterators(&it,&endit,Simplex::NONBASIC_FREE);
    while (it != endit) {
        int variableIndex = it.getData();
        if (Numerical::fabs(alpha.at(variableIndex)) > m_pivotTolerance) {
            Numerical::Double steplength = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
            //A free variable is selected only if it defines a dual steplength with proper sign
            if (m_sigma * steplength >= 0) {
                m_incomingVariableIndex = variableIndex;
                m_dualSteplength = steplength;
                break;
            }
        }
        ++it;
    }

    if(m_incomingVariableIndex == -1){
        //NONBASIC_FIXED variables are ignored
        m_variableStates.getIterators(&it,&endit,Simplex::NONBASIC_AT_LB);
        while (it != endit) {
            int variableIndex = it.getData();
            Numerical::Double signedAlpha = m_sigma * alpha.at(variableIndex);
            if ( signedAlpha > m_pivotTolerance) {
//                Numerical::Double value = m_reducedCosts.at(variableIndex) / signedAlpha;
//                m_breakpointHandler.insertBreakpoint(variableIndex,
//                                                     value ,
//                                                     value + (workingTolerance / signedAlpha ));
                const Numerical::Double& reducedCost = m_reducedCosts.at(variableIndex);
                m_breakpointHandler.insertBreakpoint(variableIndex,
                                                     reducedCost / signedAlpha,
                                                     (reducedCost + workingTolerance) / signedAlpha);
//                m_breakpointHandler.insertBreakpoint(variableIndex,
//                                                     m_reducedCosts.at(variableIndex) / signedAlpha,
//                                                     (m_reducedCosts.at(variableIndex) + workingTolerance) / signedAlpha);
            }
            ++it;
        }

        m_variableStates.getIterators(&it,&endit,Simplex::NONBASIC_AT_UB);
        while (it != endit) {
            int variableIndex = it.getData();
            Numerical::Double signedAlpha = m_sigma * alpha.at(variableIndex);
            if (signedAlpha < - m_pivotTolerance) {
                const Numerical::Double& reducedCost = m_reducedCosts.at(variableIndex);
                m_breakpointHandler.insertBreakpoint(variableIndex,
                                                     reducedCost / signedAlpha,
                                                     (reducedCost - workingTolerance) / signedAlpha);
            }
            ++it;
        }
    }

    m_breakpointHandler.finalizeBreakpoints();
}

void DualRatiotest::computeFunctionPhase2(const Vector &alpha,
                                          unsigned int &iterationCounter,
                                          Numerical::Double &functionSlope,
                                          Numerical::Double workingTolerance)
{
    unsigned int length = m_breakpointHandler.getNumberOfBreakpoints();
    Numerical::Double t_actual = 0;
    Numerical::Double absPrimalSteplength = 0;

    const BreakpointHandler::BreakPoint * actualBreakpoint = NULL;
    while (iterationCounter < length) {

        actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
        t_actual = actualBreakpoint->value;

        absPrimalSteplength = functionSlope / Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));

        const Variable & variable = m_model.getVariable(actualBreakpoint->variableIndex);
        functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex)) *
                (variable.getUpperBound() - variable.getLowerBound());

        if (functionSlope <= 0) {
            if(t_actual < workingTolerance){
                m_boundflips.clear();
                actualBreakpoint = m_breakpointHandler.getBreakpoint(0);
            }
            m_incomingVariableIndex = actualBreakpoint->variableIndex;
            m_dualSteplength = m_sigma * t_actual;
            break;
        }

        //at this point only bounded type variables
        if((variable.getUpperBound() - variable.getLowerBound()) < absPrimalSteplength ) {
            m_boundflips.push_back(actualBreakpoint->variableIndex);
        } else{
            if(t_actual < workingTolerance){
                m_boundflips.clear();
                actualBreakpoint = m_breakpointHandler.getBreakpoint(0);
            }
            m_incomingVariableIndex = actualBreakpoint->variableIndex;
            m_dualSteplength = m_sigma * t_actual;
            break;
        }
        ++iterationCounter;
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
            if (Numerical::fabs(alpha.at(breakpoint->variableIndex))> maxAlphaAbs){
                maxAlphaId = nextIterationCounter;
                maxAlphaAbs = Numerical::fabs(alpha.at(breakpoint->variableIndex));
            }
        }else{
            break;
        }
    }

    if (maxAlphaAbs > m_pivotTolerance){
        m_incomingVariableIndex = m_breakpointHandler.getBreakpoint(maxAlphaId)->variableIndex;
        m_dualSteplength = m_sigma * m_breakpointHandler.getBreakpoint(maxAlphaId)->value;
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
                m_dualSteplength = m_sigma * prevBreakpoint->value;
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
                                           Numerical::Double workingTolerance)
{
    m_boundflips.clear();
    m_boundflips.reserve(alpha.nonZeros());
    m_breakpointHandler.init(alpha.nonZeros());

    m_incomingVariableIndex = -1;
    m_dualSteplength = 0;
    Numerical::Double functionSlope = 0;

    unsigned int iterationCounter = 0;

//determining t>0 or t<0 cases
    const Variable & outgoingVariable = m_model.getVariable(outgoingVariableIndex);
    Numerical::Double outgoingVariableValue = *(m_variableStates.getAttachedData(outgoingVariableIndex));

    if ( outgoingVariableValue < outgoingVariable.getLowerBound()) {
        functionSlope = -(outgoingVariableValue - outgoingVariable.getLowerBound());
        m_sigma = -1;
    } else {
        functionSlope = outgoingVariableValue - outgoingVariable.getUpperBound();
        m_sigma = 1;
    }

    if(m_expand != "INACTIVE"){
        generateExpandedBreakpointsPhase2(alpha,workingTolerance);
    }else{
        generateSignedBreakpointsPhase2(alpha);
    }

    //Slope check should be enabled in debug mode
#ifndef NDEBUG
    if (functionSlope < 0) {
        LPERROR("SLOPE IS NEGATIVE ERROR - TERMINATING!!! s: "<<functionSlope);
    }
#endif

    //free variables always enter the basis
    if (m_incomingVariableIndex == -1) {
        if (m_breakpointHandler.getNumberOfBreakpoints() > 0) {
            m_breakpointHandler.initSorting();
            //Handle fake feasible breakpoints
            if(m_expand == "INACTIVE" && m_enableFakeFeasibility){
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

                        m_incomingVariableIndex = breakpoint->variableIndex;
                        m_dualSteplength = m_sigma * breakpoint->value;
                        break;
                    }

                    case PIECEWISE_LINEAR_FUNCTION:{
                        computeFunctionPhase2(alpha,iterationCounter,functionSlope,workingTolerance);
                        break;
                    }

                    case STABLE_PIVOT:{
                        computeFunctionPhase2(alpha,iterationCounter,functionSlope,workingTolerance);
                        if( Numerical::fabs(alpha.at(m_incomingVariableIndex)) < m_pivotTolerance ) {
                            useNumericalThresholdPhase2(iterationCounter, alpha);
                        }
                        break;
                    }
                }
                //Harris, expand
                if(m_expand != "INACTIVE"){
                    const std::vector<const BreakpointHandler::BreakPoint*>& secondPassRatios = m_breakpointHandler.getExpandSecondPass();
#ifndef NDEBUG
                    if(secondPassRatios.size() == 0){
                        LPWARNING("No second pass ratios found!!");
                        exit(-1);
                    }
#endif
                    int maxBreakpointId = 0;
                    int maxAlphaId = secondPassRatios[0]->variableIndex;
                    int variableIndex = -1;

                    //choosing best pivot candidate
                    for(unsigned i=1; i < secondPassRatios.size(); i++){
                        variableIndex = secondPassRatios[i]->variableIndex;
                        if(Numerical::fabs(alpha.at(variableIndex)) > Numerical::fabs(alpha.at(maxAlphaId))){
                            maxAlphaId = variableIndex;
                            maxBreakpointId = i;
                        }
                    }
                    Numerical::Double thetaMin = 0;
                    if(m_expand == "EXPANDING"){
                        thetaMin = m_toleranceStep / Numerical::fabs(alpha.at(maxAlphaId));
                    }
                    //maxAlphaId holds the biggest |alpha| index
                    //HARRIS: if no positive ratio is present, maxAlphaId is the incoming variable
                    //EXPAND: if no bigger than theta_min is present, maxAlphaId is the incoming variable
                    bool original = true;
                    if(!original){
                        int candidateAlphaId = secondPassRatios[0]->variableIndex;
                        int candidateBreakpointId = -1;
                        double limit = (m_expand == "HARRIS") ? 0 : thetaMin;
                        //choosing best pivot candidate with positive ratio value
                        for(unsigned i=1; i < secondPassRatios.size(); i++){
                            variableIndex = secondPassRatios[i]->variableIndex;
                            if(Numerical::fabs(alpha.at(variableIndex)) > Numerical::fabs(alpha.at(candidateAlphaId)) &&
                                    (secondPassRatios[i]->value > limit)){
                                candidateAlphaId = variableIndex;
                                candidateBreakpointId = i;
                            }
                        }
                        if(candidateBreakpointId != -1){
//                            LPINFO("TAKEOVER");
                            maxBreakpointId = candidateBreakpointId;
                            maxAlphaId = candidateAlphaId;
                        }
                    }
                    //theta remains zero if the choosen breakpoint value is smaller than thetaMin
                    //Harris, expand
                    if(secondPassRatios[maxBreakpointId]->value <= thetaMin){
//                        LPINFO("Harris ratiotest theta is zero!");
                        m_degenerate = true;
                        m_dualSteplength = m_sigma * thetaMin;
                    } else {
                        m_degenerate = false;
                        m_dualSteplength = m_sigma * secondPassRatios[maxBreakpointId]->value;
                    }
                    m_incomingVariableIndex = maxAlphaId;
                } else if(m_dualSteplength != 0){
                    m_degenerate = false;
                }else{
                    m_degenerate = true;
                }
            }
        } else {
            LPWARNING(" - Ratiotest - No breakpoint found!");
            m_incomingVariableIndex = -1;
        }
    }
    if(m_incomingVariableIndex != -1 && Numerical::fabs(alpha.at(m_incomingVariableIndex)) < m_pivotTolerance){
        m_incomingVariableIndex = -1;
        m_dualSteplength = 0;
    }
}
