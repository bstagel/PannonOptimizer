//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

/**
 * @file dualratiotest.cpp
 */

#include <simplex/dualratiotest.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/simplex.h>
#include <random>
#define LPINOF LPINFO

DualRatiotest::DualRatiotest(const SimplexModel & model,
                             DenseVector &reducedCosts,
                             const IndexList<>& reducedCostFeasibilities,
                             const IndexList<const Numerical::Double *> &variableStates) :
    m_model(model),
    m_reducedCosts(reducedCosts),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_variableStates(variableStates),
    m_sigma(0),
    m_wolfeActive(false),
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
    m_avoidThetaMin(SimplexParameterHandler::getInstance().getBoolParameterValue("Ratiotest.Expand.avoidthetamin")),
    m_wolfe(SimplexParameterHandler::getInstance().getBoolParameterValue("Ratiotest.enable_wolfe_adhoc")),
    m_toleranceStep(m_optimalityTolerance * (1 - SimplexParameterHandler::getInstance().getDoubleParameterValue("Ratiotest.Expand.multiplier")) /
                    SimplexParameterHandler::getInstance().getIntegerParameterValue("Ratiotest.Expand.divider")),
    m_degenerate(false)
{
    int dimension = m_model.getColumnCount() + m_model.getRowCount();
    m_ratioDirections.resize(dimension);
    m_updateFeasibilitySets.reserve(2*dimension);
    m_breakpointHandler.init(2*dimension);
    m_boundflips.reserve(dimension);
    if (m_wolfe) {
        m_degenerateAtLB.init(dimension, dimension);
        m_degenerateAtUB.init(dimension, dimension);
    }
}

void DualRatiotest::generateSignedBreakpointsPhase1(const DenseVector& alpha)
{
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
            m_breakpointHandler.insertBreakpoint(variableIndex, m_reducedCosts.at(variableIndex) / signedAlpha);
            if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                m_breakpointHandler.insertBreakpoint(variableIndex, m_reducedCosts.at(variableIndex) / signedAlpha);
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
            m_breakpointHandler.insertBreakpoint(variableIndex, m_reducedCosts.at(variableIndex) / signedAlpha);
            if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                m_breakpointHandler.insertBreakpoint(variableIndex, m_reducedCosts.at(variableIndex) / signedAlpha);
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
                m_breakpointHandler.insertBreakpoint(variableIndex, m_reducedCosts.at(variableIndex) / signedAlpha);
            } else
                if (typeOfIthVariable == Variable::MINUS && signedAlpha < -epsilon) {
                    m_ratioDirections[variableIndex] = 1;
                    m_breakpointHandler.insertBreakpoint(variableIndex, m_reducedCosts.at(variableIndex) / signedAlpha);
                } else
                    if (typeOfIthVariable == Variable::FREE) {
                        m_breakpointHandler.insertBreakpoint(variableIndex, m_reducedCosts.at(variableIndex) / signedAlpha);
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

void DualRatiotest::computeFunctionPhase1(const DenseVector& alpha,
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
#ifndef NDEBUG
            LPERROR("In phase 1 NO function defined, num of bpts: "<<m_breakpointHandler.getNumberOfBreakpoints());
#endif
        }
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
#ifndef NDEBUG
            LPERROR("In phase 1 NO function defined, num of bpts: "<<m_breakpointHandler.getNumberOfBreakpoints());
#endif
        }
    }
}

void DualRatiotest::useNumericalThresholdPhase1(unsigned int iterationCounter,
                                                const DenseVector& alpha,
                                                Numerical::Double& functionSlope)
{
    m_stablePivotActivationPhase1++;
#ifndef NDEBUG
    LPINFO("Stable pivot activated in phase 1");
#endif

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

    //Prefer the selection of the previous breakpoint hence it is already available
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
#ifndef NDEBUG
        LPWARNING("No stable pivot found in phase 1!");
#endif
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

void DualRatiotest::performRatiotestPhase1(const DenseVector& alpha,
                                           Numerical::Double phaseIReducedCost,
                                           Numerical::Double phaseIObjectiveValue)
{
    //VERY IMPORTANT to have here this boundflip clear (if any)
    //If a fallback occurs when the boundflips is not empty than it will lead to undefined behaviour
    //In parallel case this can cause serious errors (because of the basis resetting)
    m_boundflips.clear();

    m_becomesFeasible.clear();
    m_updateFeasibilitySets.clear();
    m_breakpointHandler.clear();

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
#ifndef NDEBUG
        LPWARNING(" - Ratiotest - No breakpoint found!");
#endif
    }
    //Ask for another row
    if(m_incomingVariableIndex != -1 && Numerical::fabs(alpha.at(m_incomingVariableIndex)) < m_pivotTolerance){
        m_incomingVariableIndex = -1;
        m_dualSteplength = 0;
    }
}

void DualRatiotest::generateSignedBreakpointsPhase2(const DenseVector &alpha)
{
    //computing ratios
    auto it = m_variableStates.getPartitionIterator();
    auto endit = m_variableStates.getPartitionIterator();
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
                m_breakpointHandler.insertBreakpoint(variableIndex, m_reducedCosts.at(variableIndex) / signedAlpha);
            }
            ++it;
        }

        m_variableStates.getIterators(&it,&endit,Simplex::NONBASIC_AT_UB);
        while (it != endit) {
            int variableIndex = it.getData();
            Numerical::Double signedAlpha = m_sigma * alpha.at(variableIndex);
            if (signedAlpha < -epsilon) {
                m_breakpointHandler.insertBreakpoint(variableIndex, m_reducedCosts.at(variableIndex) / signedAlpha);
            }
            ++it;
        }
    }

    m_breakpointHandler.finalizeBreakpoints();
}

void DualRatiotest::generateExpandedBreakpointsPhase2(const DenseVector &alpha,
                                                      Numerical::Double workingTolerance)
{
    //computing ratios
    auto it = m_variableStates.getPartitionIterator();
    auto endit = m_variableStates.getPartitionIterator();

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
                const Numerical::Double& reducedCost = m_reducedCosts.at(variableIndex);
                m_breakpointHandler.insertBreakpoint(variableIndex,
                                                     reducedCost / signedAlpha,
                                                     (reducedCost + workingTolerance) / signedAlpha);
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

void DualRatiotest::computeFunctionPhase2(const DenseVector &alpha,
                                          unsigned int &iterationCounter,
                                          Numerical::Double &functionSlope,
                                          Numerical::Double workingTolerance)
{
    unsigned int numberOfBreakpoints = m_breakpointHandler.getNumberOfBreakpoints();
    Numerical::Double t_actual = 0;
    Numerical::Double absPrimalSteplength = 0;

    const BreakpointHandler::BreakPoint * actualBreakpoint = NULL;
    while (iterationCounter < numberOfBreakpoints) {
        actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
        t_actual = actualBreakpoint->value;

        absPrimalSteplength = functionSlope / Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));

        const Variable & variable = m_model.getVariable(actualBreakpoint->variableIndex);

        /*if (std::isnan((Numerical::fabs(alpha.at(actualBreakpoint->variableIndex)) *
                       (variable.getUpperBound() - variable.getLowerBound())).getDouble())) {
            double a = Numerical::fabs(alpha.at(actualBreakpoint->variableIndex)).getDouble();
            double b = (variable.getUpperBound() - variable.getLowerBound()).getDouble();
            LPERROR(a << " * " << b << " = " << a * b);
            exit(1);
        }*/
        Numerical::Double a = Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));
        Numerical::Double b = (variable.getUpperBound() - variable.getLowerBound());

        Numerical::Double c = a * b;
        functionSlope -= c;
        //functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex)) *
        //        (variable.getUpperBound() - variable.getLowerBound());


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
                                                const DenseVector &alpha)
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
#ifndef NDEBUG
                LPINFO("Stable pivot chosen");
#endif
                m_incomingVariableIndex = prevBreakpointId;
                m_dualSteplength = m_sigma * prevBreakpoint->value;
                return;
            }
        }
        if (prevIterationCounter == 0){
            m_incomingVariableIndex = -1;
            m_dualSteplength = 0;
            m_stablePivotNotFoundPhase2++;
#ifndef NDEBUG
            LPWARNING("No stable pivot fbound in phase 2");
#endif
        }
    } else {
        m_incomingVariableIndex = -1;
        m_dualSteplength = 0;
        m_stablePivotNotFoundPhase2++;
#ifndef NDEBUG
        LPWARNING("No stable pivot found in phase 2");
#endif
    }
}

void DualRatiotest::performRatiotestPhase2(unsigned int outgoingVariableIndex,
                                           const DenseVector& alpha,
                                           Numerical::Double workingTolerance)
{
    m_boundflips.clear();
    m_breakpointHandler.clear();

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

    if (m_wolfeActive) {
        wolfeAdHocMethod(outgoingVariableIndex,alpha,workingTolerance);
    } else {
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
                        ++iterationCounter;

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
#ifndef NDEBUG
                        LPINFO("Fake feasible slope: "<<functionSlope);
#endif
                        m_incomingVariableIndex = -1;
                        m_dualSteplength = 0;
                        return;
                    }
                }else if(m_expand != "INACTIVE" && m_enableFakeFeasibility){
                    const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
                    //original slope isn't updated, we investigate if there are too many fake feasible breakpoints
                    auto slope = functionSlope;
                    unsigned iteration = iterationCounter;
                    while (breakpoint->value < 0){
                        const Variable & variable = m_model.getVariable(breakpoint->variableIndex);
                        slope -= Numerical::fabs(alpha.at(breakpoint->variableIndex)) *
                                (variable.getUpperBound() - variable.getLowerBound());
                        iteration++;
                        if(iterationCounter < m_breakpointHandler.getNumberOfBreakpoints()){
                            breakpoint = m_breakpointHandler.getBreakpoint(iteration);
                        } else {
                            break;
                        }
                    }
                    //if slope gone negative, fake feasible variable with biggest |alpha| is incoming
                    if(slope < 0 || iteration == m_breakpointHandler.getNumberOfBreakpoints()){
                        int maxAlphaId = m_breakpointHandler.getBreakpoint(0)->variableIndex;
                        for(unsigned i=1; i < (iteration-iterationCounter); i++){
                            int variableIndex = m_breakpointHandler.getBreakpoint(i)->variableIndex;
                            if(Numerical::fabs(alpha.at(variableIndex)) > Numerical::fabs(alpha.at(maxAlphaId))){
                                maxAlphaId = variableIndex;
                            }
                        }
#ifndef NDEBUG
                        LPINFO("Fake feasible variable");
#endif
                        m_dualSteplength = m_sigma * m_reducedCosts.at(maxAlphaId) / Numerical::fabs(alpha.at(maxAlphaId));
                        m_incomingVariableIndex = maxAlphaId;
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
                        //Harris, expand
                        if(secondPassRatios[maxBreakpointId]->value <= thetaMin){
                            if(m_avoidThetaMin){
                                int candidateBreakpointId = -1;
                                Numerical::Double maxAlpha = 0;
                                Numerical::Double limit = (m_expand == "HARRIS") ? (Numerical::Double)0.0 : thetaMin;
                                //choosing best pivot candidate with ratio value > thetamin
                                for(unsigned i=0; i < secondPassRatios.size(); i++){
                                    variableIndex = secondPassRatios[i]->variableIndex;
                                    if((secondPassRatios[i]->value > limit) &&
                                            Numerical::fabs(alpha.at(variableIndex)) > Numerical::fabs(maxAlpha)){
                                        maxAlpha = alpha.at(variableIndex);
                                        candidateBreakpointId = i;
                                    }
                                }
                                if(maxAlpha != 0){
                                    //                                LPINFO("Alternative maximal pivot element!");
                                    //                                LPINFO("t: "<<secondPassRatios[candidateBreakpointId]->value<<" alpha: "<<alpha.at(candidateAlphaId)<<
                                    //                                       " vs "<<secondPassRatios[maxBreakpointId]->value<<" alpha: "<<alpha.at(maxAlphaId)<<
                                    //                                       " thetamin: "<<thetaMin);
                                    m_degenerate = false;
                                    m_dualSteplength = m_sigma * secondPassRatios[candidateBreakpointId]->value;
                                    m_incomingVariableIndex = secondPassRatios[candidateBreakpointId]->variableIndex;
                                    return;
                                }
                            }
                            //                       LPINFO("ThetaMin selected: "<<m_thetaMin);
                            m_degenerate = true;
                            m_dualSteplength = m_sigma * thetaMin;
                        } else {
                            m_degenerate = false;
                            m_dualSteplength = m_sigma * secondPassRatios[maxBreakpointId]->value;
                        }
                        m_incomingVariableIndex = maxAlphaId;
                    } else if(m_dualSteplength != 0){
                        m_degenerate = false;
                    } else{
                        m_degenerate = true;
//                        LPINFO("Degeneracy at: "<<m_outgoingVariableIndex);
                        if (m_wolfe) {
                            wolfeAdHocMethod(outgoingVariableIndex,alpha,workingTolerance);
                        }
                    }
                }
            } else {
#ifndef NDEBUG
                LPWARNING(" - Ratiotest - No breakpoint found!");
#endif
                m_incomingVariableIndex = -1;
            }
        }
        //Ask for another row
        if(m_incomingVariableIndex != -1 && Numerical::fabs(alpha.at(m_incomingVariableIndex)) < m_pivotTolerance){
            m_incomingVariableIndex = -1;
            m_dualSteplength = 0;
        }
    }
}

bool DualRatiotest::performWolfeRatiotest(const DenseVector &alpha)
{
    m_breakpointHandler.clear();
    Numerical::Double epsilon = 0;
    if(m_ePivotGeneration){
        epsilon = m_pivotTolerance;
    }

    IndexList<>::PartitionIterator it;
    IndexList<>::PartitionIterator endit;
    //variables in this set have finite LB
    m_degenerateAtLB.getIterators(&it,&endit,m_degenDepth);
    for (; it != endit; ++it) {
        unsigned int variableIndex = it.getData();
        Numerical::Double signedAlpha = m_sigma * alpha.at(variableIndex);

#ifndef NDEBUG
        if (0 > m_reducedCosts.at(variableIndex) ) {
            LPINFO("m_reducedCosts.at(variableIndex) < 0: "<<m_reducedCosts.at(variableIndex));
            exit(-1);
        }
#endif
        if ( signedAlpha > epsilon) {
            m_breakpointHandler.insertBreakpoint(variableIndex, m_reducedCosts.at(variableIndex) / signedAlpha);
        }
    }

    //variables in this set have finite UB
    m_degenerateAtUB.getIterators(&it,&endit,m_degenDepth);
    for (; it != endit; ++it) {
        unsigned int variableIndex = it.getData();
        Numerical::Double signedAlpha = m_sigma * alpha.at(variableIndex);

#ifndef NDEBUG
        if (0 < m_reducedCosts.at(variableIndex) ) {
            LPERROR("m_reducedCosts.at(variableIndex) > 0: "<<m_reducedCosts.at(variableIndex));
            exit(-1);
        }
#endif
        if ( signedAlpha < -epsilon) {
            m_breakpointHandler.insertBreakpoint(variableIndex, m_reducedCosts.at(variableIndex) / signedAlpha);
        }
    }

    m_breakpointHandler.finalizeBreakpoints();

    if ( m_breakpointHandler.getNumberOfBreakpoints() > 0 ) {
        m_breakpointHandler.initSorting();
        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(0);

        m_incomingVariableIndex = breakpoint->variableIndex;
        m_dualSteplength = m_sigma * breakpoint->value;
#ifndef NDEBUG
        if (m_dualSteplength >= 10E-4 || Numerical::fabs(alpha.at(m_incomingVariableIndex)) < m_pivotTolerance) {
            LPINFO("m_dualSteplength "<<m_dualSteplength);
            LPINFO("alpha: "<<alpha.at(m_incomingVariableIndex));
            LPINFO("d_j: "<<m_reducedCosts.at(m_incomingVariableIndex));
        }
#endif
        return true;
    }
    return false;
}

void DualRatiotest::wolfeAdHocMethod(int outgoingVariableIndex, const DenseVector &alpha, Numerical::Double workingTolerance)
{
//        LPINFO("wolfeadhocmethod called");
        //Wolfe's 'ad hoc' method, small pivot candidates are excluded in the ratiotest
        Numerical::Double degeneracyTolerance = m_optimalityTolerance;
        //step 0: init Wolfe, compute degeneracy sets
        if (!m_wolfeActive) {
            for (unsigned variableIndex = 0; variableIndex < m_reducedCosts.length(); ++variableIndex) {
//                LPINFO("varindex: "<<variableIndex);
                Numerical::Double dj = m_reducedCosts.at(variableIndex);
                if (Numerical::equal(dj, 0, degeneracyTolerance) ) {
                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_LB) {
                        m_degenerateAtLB.insert(0,variableIndex);
                    } else if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_UB) {
                        m_degenerateAtUB.insert(0,variableIndex);
                    }
                } else if (dj + m_optimalityTolerance < 0 && m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_LB) {
                    m_wolfeActive = false;
//                    LPINFO("fallback at d_j: "<<m_reducedCosts.at(variableIndex));
//                    LPINFO("m_state: "<<m_variableStates.where(variableIndex));
//                    exit(-1);
                    throw FallbackException("Infeasible variable in phase 2");
                } else if (dj - m_optimalityTolerance > 0 && m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_UB) {
                    m_wolfeActive = false;
//                    LPINFO("fallback at d_j: "<<m_reducedCosts.at(variableIndex));
//                    LPINFO("m_state: "<<m_variableStates.where(variableIndex));
//                    exit(-1);
                    throw FallbackException("Infeasible variable in phase 2");
                }
            }
            m_degenDepth = 0;
            m_wolfeActive = true;
        }

        //step 1: vitual perturbation from interval (e_opt, 2*e_opt)
        //TODO: addVector a set helyett?
        std::uniform_real_distribution<double> distribution(m_optimalityTolerance, 2*m_optimalityTolerance);
        std::default_random_engine engine;
        Numerical::Double delta = distribution(engine);
        //perturbation with the same delta value for a given depth
        bool sameForDepth = false;
        IndexList<>::PartitionIterator it;
        IndexList<>::PartitionIterator endit;
        bool increaseDepth = false;

        //D_Lb
//         LPINFO("m_degenDepth: "<<m_degenDepth);
        m_degenerateAtLB.getIterators(&it,&endit,m_degenDepth);
        std::vector<unsigned> positionsToMove;
        for (; it != endit; ++it) {
            //        LPINFO("visiting dlb");
            unsigned int variableIndex = it.getData();
            Numerical::Double dj = m_reducedCosts.at(variableIndex);
            if (Numerical::equal( dj, 0,degeneracyTolerance)) {
//                LPINFO("D_lb candidate index: "<<variableIndex<<" dj: "<<dj);
                increaseDepth = true;
                if (sameForDepth) {
                    m_reducedCosts.set(variableIndex, dj + delta);
                } else {
                    Numerical::Double delta = distribution(engine);
                    m_reducedCosts.set(variableIndex, dj + delta);
                }
                positionsToMove.push_back(variableIndex);
            } else if (dj + m_optimalityTolerance < 0) {
                m_wolfeActive = false;
#ifndef NDEBUG
                LPINFO("step 1 fallback at d_j: "<<m_reducedCosts.at(variableIndex));
                LPINFO("m_state: "<<m_variableStates.where(variableIndex));
                exit(-1);
#endif
                throw FallbackException("Infeasible variable in phase 2");
            }
        }
        //increase the depth of degeneracy
        for (unsigned i = 0; i < positionsToMove.size(); i++) {
            m_degenerateAtLB.move(positionsToMove[i],m_degenDepth+1);
        }
        positionsToMove.clear();

        //D_Ub
        m_degenerateAtUB.getIterators(&it,&endit,m_degenDepth);
        for (; it != endit; ++it) {
    //        LPINFO("visiting dub");
            unsigned int variableIndex = it.getData();
            Numerical::Double dj = m_reducedCosts.at(variableIndex);
            if (Numerical::equal(dj,0,degeneracyTolerance)) {
    //            LPINFO("D_ub candidate index: "<<variableIndex<<" dj: "<<dj);
                increaseDepth = true;
                if (sameForDepth) {
                    m_reducedCosts.set(variableIndex, dj - delta);
                } else {
                    Numerical::Double delta = distribution(engine);
                    m_reducedCosts.set(variableIndex, dj - delta);
                }
                positionsToMove.push_back(variableIndex);
            }else if (dj - m_optimalityTolerance > 0) {
                m_wolfeActive = false;
#ifndef NDEBUG
                LPINFO("step 1 fallback at d_j: "<<m_reducedCosts.at(variableIndex));
                LPINFO("m_state: "<<m_variableStates.where(variableIndex));
                exit(-1);
#endif
                throw FallbackException("Infeasible variable in phase 2");
            }
        }
        //increase the depth of degeneracy
        for (unsigned i = 0; i < positionsToMove.size(); i++) {
            m_degenerateAtUB.move(positionsToMove[i],m_degenDepth+1);
        }
        positionsToMove.clear();

        if (increaseDepth) {
            m_degenDepth++;
    //        LPINFO("Wolfe: depth increased to "<<m_degenDepth);
        }
        do{
//            LPINFO("Performing Wolfe");
            //step 2: perform Wolfe ratiotest with ratios whose depth was increased
            bool pivotFound = performWolfeRatiotest(alpha);
            //step 3: apply special update
            if (pivotFound) {
    //            LPINFO("Wolfe: special update with variable: "<<m_incomingVariableIndex<<" theta: "<<
    //                   m_dualSteplength);
#ifndef NDEBUG
                if (m_degenDepth == 0) {
                    LPERROR("degendepth 0");
                    exit(-1);
                }
#endif
                return;
            //step 4: no pivot row, reset perturbed values to bounds, decrease depth
            } else {
                if (m_degenDepth == 0) break;
                m_degenerateAtLB.getIterators(&it, &endit, m_degenDepth);
                for (; it != endit; ++it) {
                    unsigned int variableIndex = it.getData();
                    m_reducedCosts.set(variableIndex, 0);
                    positionsToMove.push_back(variableIndex);
                }
                for (unsigned i = 0; i < positionsToMove.size(); i++) {
                    m_degenerateAtLB.move(positionsToMove[i], m_degenDepth-1);
                }
                positionsToMove.clear();

                m_degenerateAtUB.getIterators(&it, &endit, m_degenDepth);
                for (; it != endit; ++it) {
                    unsigned int variableIndex = it.getData();
                    m_reducedCosts.set(variableIndex, 0);
                    positionsToMove.push_back(variableIndex);
                }
                for (unsigned i = 0; i < positionsToMove.size(); i++) {
                    m_degenerateAtUB.move(positionsToMove[i], m_degenDepth-1);
                }
                positionsToMove.clear();

                m_degenDepth--;
//                LPINFO("Wolfe: depth decreased to "<<m_degenDepth);
            }
        }while(true);
        //stop Wolfe
        m_wolfeActive = false;
        m_degenerateAtLB.clearAllPartitions();
        m_degenerateAtUB.clearAllPartitions();
    //    LPINFO("Wolfe: stop");
        performRatiotestPhase2(outgoingVariableIndex,alpha,workingTolerance);
}

void DualRatiotest::setToleranceStep(Numerical::Double step)
{
    m_toleranceStep = step;
}
