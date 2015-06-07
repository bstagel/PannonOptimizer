/**
 * @file primalratiotest.cpp
 */

#include <simplex/primalratiotest.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>
#include <random>

PrimalRatiotest::PrimalRatiotest(const SimplexModel &model,
                                 DenseVector &basicVariableValues,
                                 const std::vector<int>& basishead,
                                 const IndexList<>& basicVariableFeasibilities,
                                 const IndexList<const Numerical::Double *> &variableStates
                                 ):
    m_model(model),
    m_basicVariableValues(basicVariableValues),
    m_basishead(basishead),
    m_basicVariableFeasibilities(basicVariableFeasibilities),
    m_variableStates(variableStates),
    m_sigma(1),
    m_outgoingVariableIndex(-1),
    m_primalSteplength(0),
    m_phaseIObjectiveValue(0),
    m_wolfeActive(false),
    m_degenDepth(0),
    m_ePivotGeneration(SimplexParameterHandler::getInstance().getBoolParameterValue("Ratiotest.e_pivot_generation")),
    m_nonlinearPrimalPhaseIFunction(getPrimalRatiotestMethod(
                                        SimplexParameterHandler::getInstance().getStringParameterValue("Ratiotest.nonlinear_primal_phaseI_function"))),
    m_feasibilityTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_feasibility")),
    m_pivotTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_pivot")),
    m_enableFakeFeasibility(SimplexParameterHandler::getInstance().getBoolParameterValue("Ratiotest.enable_fake_feasibility")),
    m_expand(SimplexParameterHandler::getInstance().getStringParameterValue("Ratiotest.Expand.type")),
    m_avoidThetaMin(SimplexParameterHandler::getInstance().getBoolParameterValue("Ratiotest.Expand.avoidthetamin")),
    m_wolfe(SimplexParameterHandler::getInstance().getBoolParameterValue("Ratiotest.enable_wolfe_adhoc")),
    m_toleranceStep(m_feasibilityTolerance * (1 - SimplexParameterHandler::getInstance().getDoubleParameterValue("Ratiotest.Expand.multiplier")) /
                    SimplexParameterHandler::getInstance().getIntegerParameterValue("Ratiotest.Expand.divider")),
    m_degenerate(false)
{
    if (m_wolfe) {
        m_degenerateAtLB.init(m_basicVariableValues.length(), m_basicVariableValues.length());
        m_degenerateAtUB.init(m_basicVariableValues.length(), m_basicVariableValues.length());
    }
}

void PrimalRatiotest::generateSignedBreakpointsPhase1(const DenseVector &alpha, int incomingVariableIndex)
{
    IndexList<>::PartitionIterator it;
    IndexList<>::PartitionIterator endit;
    Numerical::Double epsilon = 0;
    if(m_ePivotGeneration){
        epsilon = m_pivotTolerance;
    }

    //computing ratios in M
    m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::MINUS);
    for (; it != endit; ++it) {
        unsigned basisIndex = it.getData();
        const Variable & variable = m_model.getVariable(m_basishead[basisIndex]);
        Numerical::Double signedAlpha = m_sigma * alpha.at(basisIndex);
        if (signedAlpha < -epsilon) {
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / signedAlpha);
            if (variable.getType() == Variable::FIXED) {
                m_breakpointHandler.insertBreakpoint(basisIndex,
                                                     (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / signedAlpha);
            } else if (variable.getType() == Variable::BOUNDED) {
                m_breakpointHandler.insertBreakpoint(basisIndex,
                                                     (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / signedAlpha);
            }
        }
    }

    //computing ratios in P
    m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::PLUS);
    for (; it != endit; ++it) {
        unsigned basisIndex = it.getData();
        const Variable & variable = m_model.getVariable(m_basishead[basisIndex]);
        Numerical::Double signedAlpha = m_sigma * alpha.at(basisIndex);
        if (signedAlpha > epsilon) {
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / signedAlpha);
            if (variable.getType() == Variable::FIXED) {
                m_breakpointHandler.insertBreakpoint(basisIndex,
                                                     (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / signedAlpha);
            } else if (variable.getType() == Variable::BOUNDED) {
                m_breakpointHandler.insertBreakpoint(basisIndex,
                                                     (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / signedAlpha);
            }
        }
    }

    //computing ratios in F
    m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);
    for (; it != endit; ++it) {
        unsigned basisIndex = it.getData();
        const Variable & variable = m_model.getVariable(m_basishead[basisIndex]);
        Numerical::Double signedAlpha = m_sigma * alpha.at(basisIndex);
      //F->P
        if (signedAlpha < -epsilon && (variable.getUpperBound() != Numerical::Infinity) ) {
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / signedAlpha);
      //F->M
        } else if (signedAlpha > epsilon && (variable.getLowerBound() != - Numerical::Infinity) ) {
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / signedAlpha);
        }
    }

  //breakpoint defined by the bound of the incoming variable
    const Variable & incomingVariable = m_model.getVariable(incomingVariableIndex);
    if (incomingVariable.getType() == Variable::BOUNDED) {
        m_breakpointHandler.insertBreakpoint(-2, incomingVariable.getUpperBound() - incomingVariable.getLowerBound());
    }

    m_breakpointHandler.finalizeBreakpoints();
}

void PrimalRatiotest::computeFunctionPhase1(const DenseVector &alpha,
                                            unsigned int& iterationCounter,
                                            Numerical::Double functionSlope)
{
    Numerical::Double t_prev = 0;
    Numerical::Double t_actual = 0;
    const BreakpointHandler::BreakPoint * actualBreakpoint = NULL;

    while (functionSlope > 0 && iterationCounter < m_breakpointHandler.getNumberOfBreakpoints()) {
        actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
        t_actual = actualBreakpoint->value;

        //prefer the removal of fixed variables
        if (actualBreakpoint->variableIndex >=0 && m_model.getVariable(actualBreakpoint->variableIndex).getType() == Variable::FIXED) {
//            LPINFO("Fix variable is chosen to leave the basis with theta: "<<t_actual);
            m_primalSteplength = m_sigma * t_actual;
            m_outgoingVariableIndex = actualBreakpoint->variableIndex;
            return;
        }

        m_phaseIObjectiveValue += functionSlope * (t_actual - t_prev);
        actualBreakpoint->additionalValue = m_phaseIObjectiveValue;
        ++iterationCounter;
        t_prev = t_actual;

        //ratio defined by the bound of the incoming variable
        if (actualBreakpoint->variableIndex == -2) {
            functionSlope -= 1;
        } else{
            functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));
        }
    }
    if (actualBreakpoint != NULL){
        m_primalSteplength = m_sigma * actualBreakpoint->value;
        m_outgoingVariableIndex = actualBreakpoint->variableIndex;
    } else {
        LPWARNING("In phase 1 NO function defined, num of bpts: "<<m_breakpointHandler.getNumberOfBreakpoints());
    }
}

void PrimalRatiotest::useNumericalThresholdPhase1(unsigned int iterationCounter,
                                          const DenseVector& alpha,
                                          Numerical::Double& functionSlope)
{
    m_stablePivotActivationPhase1++;
    LPINFO("Stable pivot activated in phase 1");
    unsigned int length = m_breakpointHandler.getNumberOfBreakpoints(),
    breakpointId = length-1-iterationCounter, prevBreakpointId = breakpointId, nextBreakpointId = breakpointId,
    prevIterationCounter = 0, nextIterationCounter = 0;

    Numerical::Double prevObjValue = m_phaseIObjectiveValue, nextObjValue = m_phaseIObjectiveValue;
    if (iterationCounter > 0) {
        prevBreakpointId--;
        prevIterationCounter++;
        prevObjValue = m_breakpointHandler.getBreakpoint(prevBreakpointId)->additionalValue;
    }
    if (iterationCounter < length-1) {
        nextBreakpointId++;
        Numerical::Double t_prev = m_breakpointHandler.getBreakpoint(iterationCounter)->value;
        nextIterationCounter++;

        const BreakpointHandler::BreakPoint * actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter+nextIterationCounter);
        Numerical::Double t_actual = actualBreakpoint->value;
        nextObjValue += functionSlope * (t_actual - t_prev);
        actualBreakpoint->additionalValue = nextObjValue;
        functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));
        if(nextObjValue < m_initialPhaseIObjectiveValue){
            nextObjValue = -Numerical::Infinity;
        }
    } else{
        nextObjValue = -Numerical::Infinity;
    }
    bool prevIsBetter = nextObjValue > prevObjValue ? false : true;
    bool step = true;

    while (step &&( prevIsBetter ? Numerical::fabs(alpha.at(prevBreakpointId)) < m_pivotTolerance:
                    Numerical::fabs(alpha.at(nextBreakpointId)) < m_pivotTolerance)) {
        step = false;
        if (prevIsBetter) {
            m_stablePivotBackwardStepsPhase1++;
            if (iterationCounter - prevIterationCounter > 0) {
                prevBreakpointId--;
                prevIterationCounter++;
                prevObjValue = m_breakpointHandler.getBreakpoint(prevBreakpointId)->additionalValue;
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

                const BreakpointHandler::BreakPoint * actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter+nextIterationCounter);
                Numerical::Double t_actual = actualBreakpoint->value;
                nextObjValue += functionSlope * (t_actual - t_prev);
                actualBreakpoint->additionalValue = nextObjValue;
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
        m_outgoingVariableIndex = -1;
        m_primalSteplength = 0.0;
        m_phaseIObjectiveValue = m_initialPhaseIObjectiveValue;
    } else if (prevIsBetter) {
        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(prevBreakpointId);
        m_phaseIObjectiveValue = breakpoint->additionalValue;
        m_outgoingVariableIndex = breakpoint->variableIndex;
        m_primalSteplength = m_sigma>0 ? breakpoint->value : - breakpoint->value;
    } else {
        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(nextBreakpointId);
        m_phaseIObjectiveValue = breakpoint->additionalValue;
        m_outgoingVariableIndex = breakpoint->variableIndex;
        m_primalSteplength = m_sigma>0 ? breakpoint->value : - breakpoint->value;
    }
}

void PrimalRatiotest::performRatiotestPhase1(int incomingVariableIndex,
                                             const DenseVector &alpha,
                                             Numerical::Double phaseIReducedCost,
                                             Numerical::Double phaseIObjectiveValue)
{
    m_boundflips.clear();

    m_breakpointHandler.init(2 * alpha.nonZeros());
    Numerical::Double functionSlope = Numerical::fabs(phaseIReducedCost);
    unsigned int iterationCounter = 0;

    m_phaseIObjectiveValue = phaseIObjectiveValue;
    m_initialPhaseIObjectiveValue = phaseIObjectiveValue;
    m_outgoingVariableIndex = -1;
    m_primalSteplength = 0;

    //determining t>=0 or t<=0 cases
    if (phaseIReducedCost > 0) {
        m_sigma = -1;
    } else{
        m_sigma = 1;
    }

    //Generate the breakpoints of the piecewise linear concave function
    generateSignedBreakpointsPhase1(alpha,incomingVariableIndex);

    //If is there any breakpoint
    if (m_breakpointHandler.getNumberOfBreakpoints() > 0) {
        //Initialize the handler to provide sorted breakpoints
        m_breakpointHandler.initSorting();
        //Handle fake feasible variables if enables
        if (m_enableFakeFeasibility) {
            const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
            int fakeFeasibilityCounter = 0;
            while (breakpoint->value < 0 ) {
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
            if( functionSlope < 0 || iterationCounter == m_breakpointHandler.getNumberOfBreakpoints()){
                m_outgoingVariableIndex = -1;
                m_primalSteplength = 0;
                return;
            }
        }

        //IterationCounter points to the first non-fake feasible breakpoint if there is any (otherwise returned)
        switch(m_nonlinearPrimalPhaseIFunction){
            case ONE_STEP:{
                const BreakpointHandler::BreakPoint* breakpoint = m_breakpointHandler.getBreakpoint(0);

                m_phaseIObjectiveValue += functionSlope * breakpoint->value;
                breakpoint->additionalValue = m_phaseIObjectiveValue;

                m_outgoingVariableIndex = breakpoint->variableIndex;
                m_primalSteplength = m_sigma * breakpoint->value;
                break;
            }
            case PIECEWISE_LINEAR_FUNCTION:{
                computeFunctionPhase1(alpha,iterationCounter,functionSlope);
                break;
            }
            case STABLE_PIVOT:{
                computeFunctionPhase1(alpha,iterationCounter,functionSlope);

                if( Numerical::fabs(alpha.at(m_outgoingVariableIndex)) < m_pivotTolerance ) {
                    useNumericalThresholdPhase1(iterationCounter, alpha, functionSlope);
                }
                break;
            }
        }
    } else {
        LPWARNING(" - Ratiotest - No breakpoint found!");
    }

    if(m_outgoingVariableIndex == -2){
        m_outgoingVariableIndex = -1;
        m_boundflips.push_back(incomingVariableIndex);
        m_primalSteplength = 0;
    } else if (m_outgoingVariableIndex != -1){
        //Ask for another row
        if(Numerical::fabs(alpha.at(m_outgoingVariableIndex)) < m_pivotTolerance){
            m_outgoingVariableIndex = -1;
            m_primalSteplength = 0;
        }else{
            Numerical::Double ref = ((m_basicVariableValues.at(m_outgoingVariableIndex) - m_model.getVariable(m_basishead[m_outgoingVariableIndex]).getUpperBound())
                              / alpha.at(m_outgoingVariableIndex));
            m_outgoingAtUpperBound = Numerical::equals(m_primalSteplength, ref);
        }
    }
}

void PrimalRatiotest::generateSignedBreakpointsPhase2(const DenseVector &alpha)
{
    Numerical::Double epsilon = 0;
    if(m_ePivotGeneration){
        epsilon = m_pivotTolerance;
    }

    //prefer the removal of fixed variables
    for (unsigned int basisIndex = 0; basisIndex < m_basicVariableValues.length(); basisIndex++) {
        const Variable& variable = m_model.getVariable(m_basishead[basisIndex]);

        if (variable.getType() == Variable::FIXED) {
            Numerical::Double steplength = m_basicVariableValues.at(basisIndex) / alpha.at(basisIndex);
            if (m_sigma * steplength >= 0 && Numerical::fabs(alpha.at(basisIndex)) > m_pivotTolerance) {
//                LPINFO("Fixed variable is chosen to leave the basis with theta: "<<steplength);
                m_outgoingVariableIndex = basisIndex;
                m_primalSteplength = steplength;
                return;
            }
        }
    }

    for (unsigned int basisIndex = 0; basisIndex < m_basicVariableValues.length(); basisIndex++) {
        const Variable& variable = m_model.getVariable(m_basishead[basisIndex]);
        Numerical::Double signedAlpha = m_sigma * alpha.at(basisIndex);

        if ( signedAlpha > epsilon && (variable.getLowerBound() != - Numerical::Infinity)) {
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / signedAlpha);
        } else if (signedAlpha < -epsilon && (variable.getUpperBound() != Numerical::Infinity)) {
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / signedAlpha);
        }
    }

    m_breakpointHandler.finalizeBreakpoints();
}

void PrimalRatiotest::generateExpandedBreakpointsPhase2(const DenseVector &alpha, Numerical::Double workingTolerance)
{
    //prefer the removal of fixed variables
    for (unsigned int basisIndex = 0; basisIndex < m_basicVariableValues.length(); basisIndex++) {
        const Variable& variable = m_model.getVariable(m_basishead[basisIndex]);

        if (variable.getType() == Variable::FIXED) {
            Numerical::Double steplength = m_basicVariableValues.at(basisIndex) / alpha.at(basisIndex);
            if (m_sigma * steplength >= 0 && Numerical::fabs(alpha.at(basisIndex)) > m_pivotTolerance) {
//                LPINFO("Fixed variable is chosen to leave the basis with theta: "<<steplength);
                m_outgoingVariableIndex = basisIndex;
                m_primalSteplength = steplength;
                break;
            }
        }
    }
    for (unsigned int basisIndex = 0; basisIndex < m_basicVariableValues.length(); basisIndex++) {
        const Variable& variable = m_model.getVariable(m_basishead[basisIndex]);
        Numerical::Double signedAlpha = m_sigma * alpha.at(basisIndex);
        const Numerical::Double & basicVariableValue = m_basicVariableValues.at(basisIndex);

        if ( signedAlpha > m_pivotTolerance && (variable.getLowerBound() != - Numerical::Infinity)) {
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (basicVariableValue - variable.getLowerBound()) / signedAlpha,
                                                 (basicVariableValue - variable.getLowerBound() + workingTolerance) / signedAlpha);
        } else if (signedAlpha < -m_pivotTolerance && (variable.getUpperBound() != Numerical::Infinity)) {
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (basicVariableValue - variable.getUpperBound()) / signedAlpha,
                                                 (basicVariableValue - variable.getUpperBound() - workingTolerance ) / signedAlpha);
        }
    }

    m_breakpointHandler.finalizeBreakpoints();
}

void PrimalRatiotest::performRatiotestPhase2(int incomingVariableIndex,
                                             const DenseVector &alpha,
                                             Numerical::Double reducedCost,
                                             Numerical::Double workingTolerance){
    m_boundflips.clear();

    m_breakpointHandler.init(alpha.nonZeros());

    m_outgoingVariableIndex = -1;
    m_primalSteplength = 0;

    //determining t<=0 ot t>=0 cases
    if (reducedCost > 0) {
        m_sigma = -1;
    } else {
        m_sigma = 1;
    }

    if (m_wolfeActive) {
        wolfeAdHocMethod(incomingVariableIndex,alpha,reducedCost,workingTolerance);
    } else {
        if(m_expand != "INACTIVE"){
            generateExpandedBreakpointsPhase2(alpha,workingTolerance);
        }else{
            generateSignedBreakpointsPhase2(alpha);
        }

        //fixed variables are removed from the basis
        if (m_outgoingVariableIndex == -1) {
            if ( m_breakpointHandler.getNumberOfBreakpoints() > 0 ) {
                m_breakpointHandler.initSorting();
                const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(0);
                const Variable & incomingVariable = m_model.getVariable(incomingVariableIndex);
                Numerical::Double boundOfIncoming = incomingVariable.getUpperBound() - incomingVariable.getLowerBound();

                if ( breakpoint->value < boundOfIncoming ) {
                    m_outgoingVariableIndex = breakpoint->variableIndex;
                    m_primalSteplength = m_sigma * breakpoint->value;
                    if(m_primalSteplength != 0){
                        m_degenerate = false;
                    }else{
                        m_degenerate = true;
                    }
//                    LPINFO("Ratiotest ended with variable "<<m_outgoingVariableIndex<<" with "<<m_primalSteplength);

                    Numerical::Double ref = ((m_basicVariableValues.at(m_outgoingVariableIndex) - m_model.getVariable(m_basishead[m_outgoingVariableIndex]).getUpperBound())
                                  / alpha.at(m_outgoingVariableIndex));
                    m_outgoingAtUpperBound = Numerical::equals(m_primalSteplength,ref);
                //Boundflip, no further investigations required -> return to simplex
                } else {
//                    LPINFO("Boundflip in phase 2!");
                    m_boundflips.push_back(incomingVariableIndex);
                    m_outgoingVariableIndex = -1;
                    m_primalSteplength = 0;
                    return;
                }

                //Harris, expand
                if(m_expand != "INACTIVE" && m_boundflips.empty()){
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
                    if(secondPassRatios[maxBreakpointId]->value < thetaMin){
                        if(m_avoidThetaMin){
                            int candidateBreakpointId = -1;
                            Numerical::Double maxAlpha = 0;
                            Numerical::Double limit = (m_expand == "HARRIS") ? (Numerical::Double)0 : thetaMin;
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
                                m_primalSteplength = m_sigma * secondPassRatios[candidateBreakpointId]->value;
                                m_outgoingVariableIndex = secondPassRatios[candidateBreakpointId]->variableIndex;
                                return;
                            }
                        }
        //                   LPINFO("ThetaMin selected: "<<m_thetaMin);
                            m_degenerate = true;
                            m_primalSteplength = m_sigma * thetaMin;
                        } else {
                            m_degenerate = false;
                            m_primalSteplength = m_sigma * secondPassRatios[maxBreakpointId]->value;
                        }
                        m_outgoingVariableIndex = maxAlphaId;
                        Numerical::Double ref = ((m_basicVariableValues.at(m_outgoingVariableIndex) - m_model.getVariable(m_basishead[m_outgoingVariableIndex]).getUpperBound())
                                      / alpha.at(m_outgoingVariableIndex));
                        m_outgoingAtUpperBound = Numerical::equal(m_primalSteplength, ref, workingTolerance);
                    } else if(m_primalSteplength != 0){
                        m_degenerate = false;
                    }else{
                        m_degenerate = true;
//                        LPINFO("Degeneracy at: "<<m_outgoingVariableIndex);
                        if (m_wolfe) {
                            wolfeAdHocMethod(incomingVariableIndex,alpha,reducedCost,workingTolerance);
                        }
                    }

            //no breakpoints found
            } else{
                const Variable & incomingVariable = m_model.getVariable(incomingVariableIndex);
                Numerical::Double boundOfIncoming = incomingVariable.getUpperBound() - incomingVariable.getLowerBound();
                if ( boundOfIncoming < Numerical::Infinity){
                    m_boundflips.push_back(incomingVariableIndex);
                    m_outgoingVariableIndex = -1;
                    m_primalSteplength = 0;
                } else {
                    LPWARNING(" - Ratiotest - No breakpoint found!");
                    m_outgoingVariableIndex = -1;
                }
            }
        }
    }
}

bool PrimalRatiotest::performWolfeRatiotest(const DenseVector &alpha)
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
        unsigned int basisIndex = it.getData();
        Numerical::Double lb = m_model.getVariable(m_basishead[basisIndex]).getLowerBound();
        Numerical::Double signedAlpha = m_sigma * alpha.at(basisIndex);

        if (lb > m_basicVariableValues.at(basisIndex) ) {
            LPINFO("lb: "<<lb);
            LPINFO("xb: "<<m_basicVariableValues.at(basisIndex));
            exit(-1);
        }
        if ( signedAlpha > epsilon) {
            if (Numerical::fabs((m_basicVariableValues.at(basisIndex) - lb) / signedAlpha) == Numerical::Infinity) {
                LPINFO("lb: "<<lb);
                LPINFO("m_basicVariableValues.at(basisIndex): "<<m_basicVariableValues.at(basisIndex));
                LPINFO("signedalpha: "<<signedAlpha);
                LPINFO("type: "<<m_model.getVariable(m_basishead[basisIndex]).getType());
                LPINFO("pos: "<<basisIndex<<" Xb: "<<m_basicVariableValues[basisIndex]<<" lb: "<<lb<<" alpha: "<<alpha.at(basisIndex));
                exit(0);
            }
            m_breakpointHandler.insertBreakpoint(basisIndex, (m_basicVariableValues.at(basisIndex) - lb) / signedAlpha);
        }
    }

    //variables in this set have finite UB
    m_degenerateAtUB.getIterators(&it,&endit,m_degenDepth);
    for (; it != endit; ++it) {
        unsigned int basisIndex = it.getData();
        Numerical::Double ub = m_model.getVariable(m_basishead[basisIndex]).getUpperBound();
        Numerical::Double signedAlpha = m_sigma * alpha.at(basisIndex);

        if (ub < m_basicVariableValues.at(basisIndex) ) {
            LPERROR("xb "<<m_basicVariableValues.at(basisIndex));
            LPERROR("ub "<<ub);
            exit(-1);
        }
        if ( signedAlpha < -epsilon) {
            if (Numerical::fabs((m_basicVariableValues.at(basisIndex) - ub) / signedAlpha) == Numerical::Infinity) {
                LPINFO("ub: "<<ub);
                LPINFO("m_basicVariableValues.at(basisIndex): "<<m_basicVariableValues.at(basisIndex));
                LPINFO("signedalpha: "<<signedAlpha);
                LPINFO("type: "<<m_model.getVariable(m_basishead[basisIndex]).getType());
                LPINFO("pos: "<<basisIndex<<" Xb: "<<m_basicVariableValues[basisIndex]<<" ub: "<<ub<<" alpha: "<<alpha.at(basisIndex));
                exit(0);
            }
            m_breakpointHandler.insertBreakpoint(basisIndex, (m_basicVariableValues.at(basisIndex) - ub) / signedAlpha);
        }
    }

    m_breakpointHandler.finalizeBreakpoints();

    if ( m_breakpointHandler.getNumberOfBreakpoints() > 0 ) {
        m_breakpointHandler.initSorting();
        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(0);

        m_outgoingVariableIndex = breakpoint->variableIndex;
        m_primalSteplength = m_sigma * breakpoint->value;
        if (m_primalSteplength >= 10E-4) {
            LPINFO("m_primalSteplength "<<m_primalSteplength);
            LPINFO("alpha: "<<alpha.at(m_outgoingVariableIndex));
            LPINFO("lb: "<<m_model.getVariable(m_basishead[m_outgoingVariableIndex]).getLowerBound());
            LPINFO("ub: "<<m_model.getVariable(m_basishead[m_outgoingVariableIndex]).getUpperBound());
        }
//        LPINFO("Wolfe: variable "<<m_outgoingVariableIndex<<" leaving with: "<<m_primalSteplength);

        Numerical::Double ref_ub = Numerical::fabs(m_primalSteplength - (m_basicVariableValues.at(m_outgoingVariableIndex) -
                      m_model.getVariable(m_basishead[m_outgoingVariableIndex]).getUpperBound())
                      / alpha.at(m_outgoingVariableIndex));
        Numerical::Double ref_lb = Numerical::fabs(m_primalSteplength - (m_basicVariableValues.at(m_outgoingVariableIndex) -
                      m_model.getVariable(m_basishead[m_outgoingVariableIndex]).getLowerBound())
                      / alpha.at(m_outgoingVariableIndex));
        m_outgoingAtUpperBound = ref_lb > ref_ub ? true : false;
        return true;
    }
    return false;
}

void PrimalRatiotest::wolfeAdHocMethod(int incomingVariableIndex, const DenseVector &alpha, Numerical::Double reducedCost, Numerical::Double workingTolerance)
{
//    LPINFO("wolfeadhocmethod called");
    //Wolfe's 'ad hoc' method, small pivot candidates are excluded in the ratiotest
    Numerical::Double degeneracyTolerance = m_feasibilityTolerance;
    //step 0: init Wolfe, compute degeneracy sets
    if (!m_wolfeActive) {
//        LPINFO("Wolfe: start");
        for (unsigned basisIndex = 0; basisIndex < m_basicVariableValues.length(); ++basisIndex) {
            Numerical::Double lb = m_model.getVariable(m_basishead[basisIndex]).getLowerBound();
            Numerical::Double ub = m_model.getVariable(m_basishead[basisIndex]).getUpperBound();
            Numerical::Double xb = m_basicVariableValues[basisIndex];
            if (Numerical::equal(xb, lb, degeneracyTolerance) ) {
                m_degenerateAtLB.insert(0,basisIndex);
            } else if (Numerical::equal(xb, ub, degeneracyTolerance) ) {
                m_degenerateAtUB.insert(0,basisIndex);
            }else if (xb + m_feasibilityTolerance < lb || xb - m_feasibilityTolerance > ub) {
                m_wolfeActive = false;
                throw FallbackException("Infeasible variable in phase 2");
            }
        }
        m_degenDepth = 0;
        m_wolfeActive = true;
    }
    //step 1: vitual perturbation from interval (e_feas, 2*e_feas)
    //TODO: addVector a set helyett?
    std::uniform_real_distribution<double> distribution(m_feasibilityTolerance, 2*m_feasibilityTolerance);
    std::default_random_engine engine;
    Numerical::Double delta = distribution(engine);
    //perturbation with the same delta value for a given depth
    bool sameForDepth = false;
    IndexList<>::PartitionIterator it;
    IndexList<>::PartitionIterator endit;
     bool increaseDepth = false;

    //D_Lb
    m_degenerateAtLB.getIterators(&it,&endit,m_degenDepth);
    std::vector<unsigned> positionsToMove;
    for (; it != endit; ++it) {
//        LPINFO("visiting dlb");
        unsigned int basisIndex = it.getData();
        Numerical::Double lb = m_model.getVariable(m_basishead[basisIndex]).getLowerBound();
        Numerical::Double xb = m_basicVariableValues[basisIndex];
        if (Numerical::equal( xb, lb,degeneracyTolerance)) {
//            if (xb <= lb) {
//            LPINFO("D_lb candidate index: "<<basisIndex<<" xb: "<<xb<<" lb: "<<lb);
            increaseDepth = true;
            if (sameForDepth) {
                m_basicVariableValues.set(basisIndex,xb + delta);
            } else {
                Numerical::Double delta = distribution(engine);
                m_basicVariableValues.set(basisIndex,xb + delta);
            }
            positionsToMove.push_back(basisIndex);
        } else if (xb + m_feasibilityTolerance < lb) {
            m_wolfeActive = false;
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
        unsigned int basisIndex = it.getData();
        Numerical::Double ub = m_model.getVariable(m_basishead[basisIndex]).getUpperBound();
        Numerical::Double xb = m_basicVariableValues[basisIndex];
        if (Numerical::equal(xb,ub,degeneracyTolerance)) {
//            if (xb >= ub) {
//            LPINFO("D_ub candidate index: "<<basisIndex<<" xb: "<<xb<<" ub: "<<ub);
            increaseDepth = true;
            if (sameForDepth) {
                m_basicVariableValues.set(basisIndex,xb - delta);
            } else {
                Numerical::Double delta = distribution(engine);
                m_basicVariableValues.set(basisIndex,xb - delta);
            }
            positionsToMove.push_back(basisIndex);
        }else if (xb - m_feasibilityTolerance > ub) {
            m_wolfeActive = false;
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
//        LPINFO("Performing Wolfe");
        //step 2: perform Wolfe ratiotest with ratios whose depth was increased
        bool pivotFound = performWolfeRatiotest(alpha);
        //step 3: apply special update
        if (pivotFound) {
//            LPINFO("Wolfe: special update with variable: "<<m_outgoingVariableIndex<<" theta: "<<
//                   m_primalSteplength);
            if (m_degenDepth == 0) {
                LPERROR("degendepth 0");
                exit(-1);
            }
            return;
        //step 4: no pivot row, reset perturbed values to bounds, decrease depth
        } else {
            if (m_degenDepth == 0) break;
            m_degenerateAtLB.getIterators(&it, &endit, m_degenDepth);
            for (; it != endit; ++it) {
                unsigned int basisIndex = it.getData();
                m_basicVariableValues.set(basisIndex, m_model.getVariable(m_basishead[basisIndex]).getLowerBound());
                positionsToMove.push_back(basisIndex);
            }
            for (unsigned i = 0; i < positionsToMove.size(); i++) {
                m_degenerateAtLB.move(positionsToMove[i], m_degenDepth-1);
            }
            positionsToMove.clear();

            m_degenerateAtUB.getIterators(&it, &endit, m_degenDepth);
            for (; it != endit; ++it) {
                unsigned int basisIndex = it.getData();
                m_basicVariableValues.set(basisIndex, m_model.getVariable(m_basishead[basisIndex]).getUpperBound());
                positionsToMove.push_back(basisIndex);
            }
            for (unsigned i = 0; i < positionsToMove.size(); i++) {
                m_degenerateAtUB.move(positionsToMove[i], m_degenDepth-1);
            }
            positionsToMove.clear();

            m_degenDepth--;
//            LPINFO("Wolfe: depth decreased to "<<m_degenDepth);
        }
    }while(true);
    //stop Wolfe
    m_wolfeActive = false;
    m_degenerateAtLB.clearAllPartitions();
    m_degenerateAtUB.clearAllPartitions();
//    LPINFO("Wolfe: stop");
    performRatiotestPhase2(incomingVariableIndex,alpha,reducedCost,workingTolerance);
}
