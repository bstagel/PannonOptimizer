/**
 * @file primalratiotest.cpp
 */

#include <simplex/primalratiotest.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>

PrimalRatiotest::PrimalRatiotest(const SimplexModel &model,
                                 const Vector& basicVariableValues,
                                 const std::vector<int>& basishead,
                                 const IndexList<>& basicVariableFeasibilities,
                                 const IndexList<const Numerical::Double *>& variableStates
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
    m_nonlinearPrimalPhaseIFunction(getPrimalRatiotestMethod(
                                        SimplexParameterHandler::getInstance().getStringParameterValue("Ratiotest.nonlinear_primal_phaseI_function"))),
    m_feasibilityTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_feasibility")),
    m_pivotTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_pivot")),
    m_enableFakeFeasibility(SimplexParameterHandler::getInstance().getBoolParameterValue("Ratiotest.enable_fake_feasibility")),
    m_expand(SimplexParameterHandler::getInstance().getStringParameterValue("Ratiotest.Expand.type")),
    m_toleranceStep(m_feasibilityTolerance * (1 - SimplexParameterHandler::getInstance().getDoubleParameterValue("Ratiotest.Expand.multiplier")) /
                    SimplexParameterHandler::getInstance().getIntegerParameterValue("Ratiotest.Expand.divider")),
    m_degenerate(false)
{}

void PrimalRatiotest::generateSignedBreakpointsPhase1(const Vector &alpha, int incomingVariableIndex)
{
    m_boundflips.clear();

    IndexList<>::PartitionIterator it;
    IndexList<>::PartitionIterator endit;
    Numerical::Double epsilon = 0;
    if(m_nonlinearPrimalPhaseIFunction < 2){
        epsilon = m_pivotTolerance;
    }

    //computing ratios in M
    m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::MINUS);
    for (; it != endit; ++it) {
        unsigned basisIndex = it.getData();
        const Variable & variable = m_model.getVariable(m_basishead[basisIndex]);
        Numerical::Double signedAlpha = m_sigma * alpha.at(basisIndex);
        if (signedAlpha < -epsilon) {
//            LPINFO("Ratio in M "<<basisIndex<<" type: "<<variable.getType());
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
//                    LPINFO("Ratio in P "<<basisIndex<<" type: "<<variable.getType());
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
//                    LPINFO("Ratio in F->P "<<basisIndex<<" type: "<<variable.getType());
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / signedAlpha);
      //F->M
        } else if (signedAlpha > epsilon && (variable.getLowerBound() != - Numerical::Infinity) ) {
//                    LPINFO("Ratio in F->M "<<basisIndex<<" type: "<<variable.getType());
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / signedAlpha);
        }
    }

  //breakpoint defined by the bound of the incoming variable
    if (m_model.getVariable(incomingVariableIndex).getType() == Variable::BOUNDED) {
        m_breakpointHandler.insertBreakpoint(-2,
                                             m_sigma * (m_model.getVariable(incomingVariableIndex).getUpperBound() - m_model.getVariable(incomingVariableIndex).getLowerBound()));
    }

    m_breakpointHandler.finalizeBreakpoints();
}

void PrimalRatiotest::computeFunctionPhase1(const Vector &alpha,
                                            unsigned int& iterationCounter,
                                            Numerical::Double functionSlope)
{
    Numerical::Double t_prev = 0;
    Numerical::Double t_actual = 0;
    const BreakpointHandler::BreakPoint * actualBreakpoint = NULL;

    while (functionSlope > 0 && iterationCounter < m_breakpointHandler.getNumberOfBreakpoints()) {
        actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
        t_actual = actualBreakpoint->value;

        m_phaseIObjectiveValue += functionSlope * (t_actual - t_prev);
        actualBreakpoint->additionalValue = m_phaseIObjectiveValue;
        ++iterationCounter;
        t_prev = t_actual;

      //ratio defined by the bound of the incoming variable
      //TODO: nem hatékony
        if (actualBreakpoint->variableIndex == -2) {
            functionSlope -= 1;
        } else{
            functionSlope -= Numerical::fabs(alpha.at(actualBreakpoint->variableIndex));
        }
//        LPINFO("s: "<<functionSlope<<" t_actual: "<<t_actual<<" index: "<<actualBreakpoint->variableIndex);
    }
    if (actualBreakpoint != NULL){
        m_primalSteplength = m_sigma * actualBreakpoint->value;
        m_outgoingVariableIndex = actualBreakpoint->variableIndex;
    } else {
        LPWARNING("In phase 1 NO function defined, num of bpts: "<<m_breakpointHandler.getNumberOfBreakpoints());
    }
}

void PrimalRatiotest::useNumericalThresholdPhase1(unsigned int iterationCounter,
                                          const Vector& alpha,
                                          Numerical::Double& functionSlope)
{
    m_stablePivotActivationPhase1++;
    unsigned int length = m_breakpointHandler.getNumberOfBreakpoints(),
    breakpointId = length-1-iterationCounter, prevBreakpointId = breakpointId, nextBreakpointId = breakpointId,
    prevIterationCounter = 0, nextIterationCounter = 0;

#ifndef NDEBUG
//    if(m_breakpoints[beakpointId].index != -1){
//    LPWARNING("BAD NUMERICAL VALUE IN PHASE I,  ACTIVATING THRESHOLD for"
//                                          "variable " << m_breakpoints[beakpointId].index
//                                          <<" with alpha" <<alpha.at(m_breakpoints[beakpointId].index)
//                                          <<" at f= "<<m_breakpoints[beakpointId].functionValue);
//    } else {
//        LPWARNING("BAD NUMERICAL VALUE IN PHASE I for the INITIAL BREAKPOINT (-1)");
//    }
#endif

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

#ifndef NDEBUG
        LPWARNING("steps(prev;next): " << prevIterationCounter<<" ; " << nextIterationCounter<<"\n");
#endif
}

void PrimalRatiotest::performRatiotestPhase1(int incomingVariableIndex,
                                             const Vector &alpha,
                                             Numerical::Double phaseIReducedCost,
                                             Numerical::Double phaseIObjectiveValue)
{
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

    generateSignedBreakpointsPhase1(alpha,incomingVariableIndex);

//    LPINFO("BREAKPOINTS:");
//    m_breakpointHandler.printBreakpoints();

    //If is there any breakpoint
    if (m_breakpointHandler.getNumberOfBreakpoints() > 0) {
        //Initialize the handler to provide sorted breakpoints
        m_breakpointHandler.initSorting();
        //Handle fake feasible variables if enables
        if (m_enableFakeFeasibility) {
            const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
            int fakeFeasibilityCounter = 0;

            while (breakpoint->value < 0 ) {
//                LPWARNING("FAKE FEASIBILITY DETECTED!");
//                LPINFO("t: "<<breakpoint->value<<" xb: "<<m_basicVariableValues.at(breakpoint->variableIndex)<<" alpha: "<<alpha.at(breakpoint->variableIndex));
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
//                if(functionSlope < 0 ){ LPWARNING("functionSlope < 0 in the beginning: "<<functionSlope);
//                }else LPWARNING("Only fakefeasible breakpints found!");
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
//            if (m_outgoingVariableIndex >= 0){
//                 LPINFO("type of outgoing: "<<m_model.getVariable(m_basishead[m_outgoingVariableIndex]).getType());
//            }else{
//                LPINFO("no outgoing");
//            }
    } else {
        LPWARNING(" - Ratiotest - No breakpoint found!");
    }

    if(m_outgoingVariableIndex == -2){
//        LPINFO("Boundflip in phase 1!");
        m_outgoingVariableIndex = -1;
        m_boundflips.push_back(incomingVariableIndex);
        Numerical::Double boundOfIncoming = m_model.getVariable(incomingVariableIndex).getUpperBound() - m_model.getVariable(incomingVariableIndex).getLowerBound();
        m_primalSteplength = m_sigma * boundOfIncoming;
    } else if (m_outgoingVariableIndex != -1){
        double ref = ((m_basicVariableValues.at(m_outgoingVariableIndex) - m_model.getVariable(m_basishead[m_outgoingVariableIndex]).getUpperBound())
                      / alpha.at(m_outgoingVariableIndex));
        m_outgoingAtUpperBound = Numerical::equals(m_primalSteplength, ref);
    }

}

void PrimalRatiotest::generateSignedBreakpointsPhase2(const Vector &alpha)
{
#ifndef NDEBUG
    if (m_basicVariableValues.getType() == Vector::SPARSE_VECTOR) LPWARNING("x_b is sparse vector!");
#endif
    Numerical::Double epsilon = m_pivotTolerance;

    for (unsigned int basisIndex = 0; basisIndex < m_basicVariableValues.length(); basisIndex++) {
        const Variable& variable = m_model.getVariable(m_basishead[basisIndex]);
        Numerical::Double signedAlpha = m_sigma * alpha.at(basisIndex);

        if ( signedAlpha > epsilon && variable.getLowerBound() != - Numerical::Infinity) {
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / signedAlpha);
        } else if (signedAlpha < -epsilon && variable.getUpperBound() != Numerical::Infinity) {
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / signedAlpha);
        }
    }

    m_breakpointHandler.finalizeBreakpoints();
}

void PrimalRatiotest::generateExpandedBreakpointsPhase2(const Vector &alpha, Numerical::Double workingTolerance)
{
#ifndef NDEBUG
    if (m_basicVariableValues.getType() == Vector::SPARSE_VECTOR) LPWARNING("x_b is sparse vector!");
#endif
    Numerical::Double epsilon = m_pivotTolerance;

    for (unsigned int basisIndex = 0; basisIndex < m_basicVariableValues.length(); basisIndex++) {
        const Variable& variable = m_model.getVariable(m_basishead[basisIndex]);
        Numerical::Double signedAlpha = m_sigma * alpha.at(basisIndex);
        const Numerical::Double & basicVariableValue = m_basicVariableValues.at(basisIndex);

        if ( signedAlpha > epsilon && variable.getLowerBound() != - Numerical::Infinity) {
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (basicVariableValue - variable.getLowerBound()) / signedAlpha,
                                                 (basicVariableValue - variable.getLowerBound() + workingTolerance) / signedAlpha);
        } else if (signedAlpha < -epsilon && variable.getUpperBound() != Numerical::Infinity) {
            m_breakpointHandler.insertBreakpoint(basisIndex,
                                                 (basicVariableValue - variable.getUpperBound()) / signedAlpha,
                                                 (basicVariableValue - variable.getUpperBound() - workingTolerance ) / signedAlpha);
        }
    }

    m_breakpointHandler.finalizeBreakpoints();
}

void PrimalRatiotest::performRatiotestPhase2(int incomingVariableIndex,
                                             const Vector &alpha,
                                             Numerical::Double reducedCost,
                                             Numerical::Double workingTolerance){
    #ifndef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    m_boundflips.clear();
    m_boundflips.reserve(alpha.nonZeros());
    m_breakpointHandler.init(alpha.nonZeros() * 2);

    m_outgoingVariableIndex = -1;
    m_primalSteplength = 0;

    //determining t<=0 ot t>=0 cases
    if (reducedCost > 0) {
        m_sigma = -1;
    } else {
        m_sigma = 1;
    }

    if(m_expand != "INACTIVE"){
        generateExpandedBreakpointsPhase2(alpha,workingTolerance);
    }else{
        generateSignedBreakpointsPhase2(alpha);
    }
    m_breakpointHandler.initSorting();

    if ( m_breakpointHandler.getNumberOfBreakpoints() > 0 ) {
        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(0);
        Numerical::Double boundOfIncoming = m_model.getVariable(incomingVariableIndex).getUpperBound() - m_model.getVariable(incomingVariableIndex).getLowerBound();

        if ( breakpoint->value < boundOfIncoming ) {
            m_outgoingVariableIndex = breakpoint->variableIndex;
            m_primalSteplength = m_sigma * breakpoint->value;
            if(m_primalSteplength != 0){
                m_degenerate = false;
            }else{
                m_degenerate = true;
            }
            double ref = ((m_basicVariableValues.at(m_outgoingVariableIndex) - m_model.getVariable(m_basishead[m_outgoingVariableIndex]).getUpperBound())
                          / alpha.at(m_outgoingVariableIndex));
            m_outgoingAtUpperBound = Numerical::equals(m_primalSteplength,ref);
      //boundflip
        } else {
//            LPINFO("Boundflip in phase 2!");
            m_boundflips.push_back(incomingVariableIndex);
            m_outgoingVariableIndex = -1;
            m_primalSteplength = m_sigma * boundOfIncoming;
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
            //TODO: megvizsgálni, hogy egy másik kiválasztása mit eredményez
            for(unsigned i=1; i < secondPassRatios.size(); i++){
                variableIndex = secondPassRatios[i]->variableIndex;
                if(Numerical::fabs(alpha.at(variableIndex)) > Numerical::fabs(alpha.at(maxAlphaId))){
                    maxAlphaId = variableIndex;
                    maxBreakpointId = i;
                }
            }

            Numerical::Double theta;
            //theta remains zero if the choosen breakpoint value is negative
            if(m_expand == "HARRIS"){
                //HARRIS
                if(secondPassRatios[maxBreakpointId]->value < 0){
//                            LPINFO("Harris ratiotest theta is zero!");
                    m_degenerate = true;
                    theta = 0;
                } else {
                    m_degenerate = false;
                    theta = m_sigma * secondPassRatios[maxBreakpointId]->value;
                }
            } else {
                //EXPAND
                Numerical::Double thetaMin = m_toleranceStep / Numerical::fabs(alpha.at(maxAlphaId));

                //Expand procedure ensures a positive step
                if(secondPassRatios[maxBreakpointId]->value < thetaMin){
//                            LPINFO("Expand procedure ensures a positive step");
                    m_degenerate = true;
                    theta = m_sigma * thetaMin;
                } else {
                    const Variable & variable = m_model.getVariable(m_basishead[m_outgoingVariableIndex]);
                    if( (Numerical::fabs(m_basicVariableValues.at(m_outgoingVariableIndex) -
                         variable.getLowerBound()) < workingTolerance) ||
                            (Numerical::fabs(m_basicVariableValues.at(m_outgoingVariableIndex) -
                                                     variable.getUpperBound()) < workingTolerance)){
                        m_degenerate = true;
//                                LPINFO("nondegenerate step");
                    }else{
                        m_degenerate = false;
                    }
                    theta = m_sigma * secondPassRatios[maxBreakpointId]->value;
                }
            }
            m_outgoingVariableIndex = maxAlphaId;
            m_primalSteplength = theta;
        }

  //no breakpoints found
    } else{
        LPWARNING(" - Ratiotest - No breakpoint found!");
        m_outgoingVariableIndex = -1;
    }
}
