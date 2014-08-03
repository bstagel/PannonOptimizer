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
    m_outgoingVariableIndex(-1),
    m_primalSteplength(0),
    m_phaseIObjectiveValue(0),
    m_phaseIIObjectiveValue(0),
    m_nonlinearPrimalPhaseIFunction(static_cast<PRIMAL_RATIOTEST_METHOD>
                                    (SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_primal_phaseI_function"))),
    m_pivotTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("e_pivot")),
    m_enableFakeFeasibility(SimplexParameterHandler::getInstance().getIntegerParameterValue("enable_fake_feasibility"))
{

}

void PrimalRatiotest::generateBreakpointsPhase1(const Vector &alpha,
                                                int incomingVariableIndex)
{
        m_boundflips.clear();

        IndexList<>::Iterator it;
        IndexList<>::Iterator endit;
        unsigned int basisIndex = 0;
        Numerical::Double value = 0;

      //t>=0 case
            if (m_tPositive) {

        //computing ratios in M
            m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::MINUS);
            for (; it != endit; it++) {
                basisIndex = it.getData();
                const Variable & variable = m_model.getVariable(m_basishead.at(basisIndex));
                if (alpha.at(basisIndex) < 0) {
//                    LPINFO("Ratio in M "<<basisIndex<<" type: "<<variable.getType());
                    value = (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / alpha.at(basisIndex);
                    m_breakpointHandler.insertBreakpoint(basisIndex,value);
                    if (variable.getType() == Variable::FIXED) {
                        m_breakpointHandler.insertBreakpoint(basisIndex,value);
                    } else if (variable.getType() == Variable::BOUNDED) {
                        value = (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / alpha.at(basisIndex);
                        m_breakpointHandler.insertBreakpoint(basisIndex,value);
                    }
                }
            }

        //computing ratios in P
            m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::PLUS);
            for (; it != endit; it++) {
                basisIndex = it.getData();
                const Variable & variable = m_model.getVariable(m_basishead.at(basisIndex));
                if (alpha.at(basisIndex) > 0) {
//                    LPINFO("Ratio in P "<<basisIndex<<" type: "<<variable.getType());
                    value = (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / alpha.at(basisIndex);
                    m_breakpointHandler.insertBreakpoint(basisIndex,value);
                    if (variable.getType() == Variable::FIXED) {
                        m_breakpointHandler.insertBreakpoint(basisIndex,value);
                    } else if (variable.getType() == Variable::BOUNDED) {
                        value = (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / alpha.at(basisIndex);
                        m_breakpointHandler.insertBreakpoint(basisIndex,value);
                    }
                }
            }

        //computing ratios in F
            m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);
            for (; it != endit; it++) {
                basisIndex = it.getData();
                const Variable & variable = m_model.getVariable(m_basishead.at(basisIndex));
              //F->P
                if (alpha.at(basisIndex) < 0 && (variable.getUpperBound() != Numerical::Infinity) ) {
//                    LPINFO("Ratio in F->P "<<basisIndex<<" type: "<<variable.getType());
                    value = (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / alpha.at(basisIndex);
                    m_breakpointHandler.insertBreakpoint(basisIndex,value);
              //F->M
                } else if (alpha.at(basisIndex) > 0 && (variable.getLowerBound() != - Numerical::Infinity) ) {
//                    LPINFO("Ratio in F->M "<<basisIndex<<" type: "<<variable.getType());
                    value = (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / alpha.at(basisIndex);
                    m_breakpointHandler.insertBreakpoint(basisIndex,value);
                }
            }

      //t<=0 case
        } else{
            //computing ratios in M
                m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::MINUS);
                for (; it != endit; it++) {
                    basisIndex = it.getData();
                    const Variable & variable = m_model.getVariable(m_basishead.at(basisIndex));
                    if (alpha.at(basisIndex) > 0) {
//                        LPINFO("Ratio in M "<<basisIndex<<" type: "<<variable.getType());
                        value = - (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / alpha.at(basisIndex);
                        m_breakpointHandler.insertBreakpoint(basisIndex,value);
                        if (variable.getType() == Variable::FIXED) {
                            m_breakpointHandler.insertBreakpoint(basisIndex,value);
                        } else if (variable.getType() == Variable::BOUNDED) {
                            value = - (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / alpha.at(basisIndex);
                            m_breakpointHandler.insertBreakpoint(basisIndex,value);
                        }
                    }
                }

            //computing ratios in P
                m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::PLUS);
                for (; it != endit; it++) {
                    basisIndex = it.getData();
                    const Variable & variable = m_model.getVariable(m_basishead.at(basisIndex));
                    if (alpha.at(basisIndex) < 0) {
//                        LPINFO("Ratio in P "<<basisIndex<<" type: "<<variable.getType());
                        value = - (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / alpha.at(basisIndex);
                        m_breakpointHandler.insertBreakpoint(basisIndex,value);
                        if (variable.getType() == Variable::FIXED) {
                            m_breakpointHandler.insertBreakpoint(basisIndex,value);
                        } else if (variable.getType() == Variable::BOUNDED) {
                            value = - (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / alpha.at(basisIndex);
                            m_breakpointHandler.insertBreakpoint(basisIndex,value);
                        }
                    }
                }

            //computing ratios in F
                m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);
                for (; it != endit; it++) {
                    basisIndex = it.getData();
                    const Variable & variable = m_model.getVariable(m_basishead.at(basisIndex));
                  //F->P
                    if (alpha.at(basisIndex) > 0 && (variable.getUpperBound() != Numerical::Infinity) ) {
//                        LPINFO("Ratio in F->P "<<basisIndex<<" type: "<<variable.getType());
                        value = - (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / alpha.at(basisIndex);
                        m_breakpointHandler.insertBreakpoint(basisIndex,value);
                  //F->M
                    } else if (alpha.at(basisIndex) < 0 && (variable.getLowerBound() != - Numerical::Infinity) ) {
//                        LPINFO("Ratio in F->P "<<basisIndex<<" type: "<<variable.getType());
                        value = - (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / alpha.at(basisIndex);
                        m_breakpointHandler.insertBreakpoint(basisIndex,value);
                    }
                }
        }
      //breakpoint defined by the bound of the incoming variable
        if (m_model.getVariable(incomingVariableIndex).getType() == Variable::BOUNDED) {
            value = m_model.getVariable(incomingVariableIndex).getUpperBound() - m_model.getVariable(incomingVariableIndex).getLowerBound();
//            LPINFO("Bounded incoming variable defined ratio at "<<value);
            m_breakpointHandler.insertBreakpoint(-2,value);
        }
}

void PrimalRatiotest::computeFunctionPhase1(const Vector &alpha,
                                            unsigned int& iterationCounter,
                                            Numerical::Double functionSlope)
{
    Numerical::Double t_prev = 0;
    Numerical::Double t_actual = 0;
    const BreakpointHandler::BreakPoint * actualBreakpoint = NULL;

    while (functionSlope >= 0 && iterationCounter < m_breakpointHandler.getNumberOfBreakpoints()) {
        actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
        t_actual = actualBreakpoint->value;

        m_phaseIObjectiveValue += functionSlope * (t_actual - t_prev);
        actualBreakpoint->functionValue = m_phaseIObjectiveValue;
        iterationCounter++;
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
        m_primalSteplength = m_tPositive ? actualBreakpoint->value : - actualBreakpoint->value;
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
        prevObjValue = m_breakpointHandler.getBreakpoint(prevBreakpointId)->functionValue;
    }
    if (iterationCounter < length-1) {
        nextBreakpointId++;
        Numerical::Double t_prev = m_breakpointHandler.getBreakpoint(iterationCounter)->value;
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

    while (step &&( prevIsBetter ? Numerical::fabs(alpha.at(prevBreakpointId)) < m_pivotTolerance:
                    Numerical::fabs(alpha.at(nextBreakpointId)) < m_pivotTolerance)) {
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

                const BreakpointHandler::BreakPoint * actualBreakpoint = m_breakpointHandler.getBreakpoint(iterationCounter+nextIterationCounter);
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
        m_outgoingVariableIndex = -1;
        m_primalSteplength = 0.0;
        m_phaseIObjectiveValue = m_initialPhaseIObjectiveValue;
    } else if (prevIsBetter) {
        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(prevBreakpointId);
        m_phaseIObjectiveValue = breakpoint->functionValue;
        m_outgoingVariableIndex = breakpoint->variableIndex;
        m_primalSteplength = m_tPositive ? breakpoint->value : - breakpoint->value;
    } else {
        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(nextBreakpointId);
        m_phaseIObjectiveValue = breakpoint->functionValue;
        m_outgoingVariableIndex = breakpoint->variableIndex;
        m_primalSteplength = m_tPositive ? breakpoint->value : - breakpoint->value;
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
    if (phaseIReducedCost < 0) {
       m_tPositive = true;
    } else{
        m_tPositive = false;
    }

    generateBreakpointsPhase1(alpha,incomingVariableIndex);

//    LPINFO("BREAKPOINTS:");
//    m_breakpointHandler.printBreakpoints();

    if (m_breakpointHandler.getNumberOfBreakpoints() > 0) {
        m_breakpointHandler.selectMethod();

        //fake feasible variables
        if (m_enableFakeFeasibility) {
            const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
            int fakeFeasibilityCounter = 0;

            while (breakpoint->value < 0 ) {
//                LPWARNING("FAKE FEASIBILITY DETECTED!");
//                LPINFO("t: "<<breakpoint->value<<" xb: "<<m_basicVariableValues.at(breakpoint->variableIndex)<<" alpha: "<<alpha.at(breakpoint->variableIndex));
                fakeFeasibilityCounter++;
                functionSlope -= Numerical::fabs(alpha.at(breakpoint->variableIndex));
                iterationCounter++;

                if(iterationCounter < m_breakpointHandler.getNumberOfBreakpoints()){
                    breakpoint = m_breakpointHandler.getBreakpoint(iterationCounter);
                } else {
                    break;
                }
            }

            if(fakeFeasibilityCounter > 0){
                m_fakeFeasibilityActivationPhase1++;
                m_fakeFeasibilityCounterPhase1+=fakeFeasibilityCounter;
            }

            if( functionSlope < 0 || iterationCounter == m_breakpointHandler.getNumberOfBreakpoints()){
//                if(functionSlope < 0 ){ LPWARNING("functionSlope < 0 in the beginning: "<<functionSlope);
//                }else LPWARNING("Only fakefeasible breakpints found!");
                m_outgoingVariableIndex = -1;
                m_primalSteplength = 0;
                return;
            }
        }

        if (iterationCounter < m_breakpointHandler.getNumberOfBreakpoints()) {
            switch(m_nonlinearPrimalPhaseIFunction){
                case ONE_STEP:{
                    const BreakpointHandler::BreakPoint* breakpoint = m_breakpointHandler.getBreakpoint(0);

                    m_phaseIObjectiveValue += functionSlope * breakpoint->value;
                    breakpoint->functionValue = m_phaseIObjectiveValue;
                    m_outgoingVariableIndex = breakpoint->variableIndex;
                    m_primalSteplength = m_tPositive ? breakpoint->value : - breakpoint->value;
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
        }
    } else {
        LPWARNING(" - Ratiotest - No breakpoint found!");
    }

    if(m_outgoingVariableIndex == -2){
//        LPWARNING("Boundflip in phase 1!");
        m_outgoingVariableIndex = -1;
        m_boundflips.push_back(incomingVariableIndex);
        m_primalSteplength = 0;
    } else if (m_outgoingVariableIndex != -1){
        double ref = ((m_basicVariableValues.at(m_outgoingVariableIndex) - m_model.getVariable(m_basishead.at(m_outgoingVariableIndex)).getUpperBound())
                      / alpha.at(m_outgoingVariableIndex));

//        LPINFO("m_primalsteplength   : "<<setw(40)<<scientific<<setprecision(36)<<m_primalSteplength);
//        LPINFO("m_primalsteplengthREF: "<<setw(40)<<scientific<<setprecision(36)<< ref);
//        LPINFO("m_outgoingAtUpperBound: "<< m_outgoingAtUpperBound);
//        if(m_primalSteplength ==ref){
//            LPERROR("ASDASD "<<(m_primalSteplength ==ref) );
//        }
//        LPINFO("mrar "<<(m_primalSteplength ==ref));
        m_outgoingAtUpperBound = Numerical::equals(m_primalSteplength, ref);
    }

}

void PrimalRatiotest::generateBreakpointsPhase2(const Vector &alpha)
{
#ifndef NDEBUG
    if (m_basicVariableValues.getType() == Vector::SPARSE_VECTOR) LPWARNING("x_b is sparse vector!");
#endif

    Numerical::Double value = 0;

    for (unsigned int basisIndex = 0; basisIndex < m_basicVariableValues.length(); basisIndex++) {
        const Variable& variable = m_model.getVariable(m_basishead.at(basisIndex));
      //t>=0 case
        if (m_tPositive) {
            if ( alpha.at(basisIndex) > 0 && variable.getLowerBound() != - Numerical::Infinity) {
                value = (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / alpha.at(basisIndex);
                m_breakpointHandler.insertBreakpoint(basisIndex,value);
            } else if (alpha.at(basisIndex) < 0 && variable.getUpperBound() != Numerical::Infinity) {
                value = (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / alpha.at(basisIndex);
                m_breakpointHandler.insertBreakpoint(basisIndex,value);
            }
      //t<=0 case
        } else{
            if ( alpha.at(basisIndex) < 0 && variable.getLowerBound() != - Numerical::Infinity) {
                value = - (m_basicVariableValues.at(basisIndex) - variable.getLowerBound()) / alpha.at(basisIndex);
                m_breakpointHandler.insertBreakpoint(basisIndex,value);
            } else if ( alpha.at(basisIndex) > 0 && variable.getUpperBound() != Numerical::Infinity) {
                value = - (m_basicVariableValues.at(basisIndex) - variable.getUpperBound()) / alpha.at(basisIndex);
                m_breakpointHandler.insertBreakpoint(basisIndex,value);
            }
        }
    }
}

void PrimalRatiotest::performRatiotestPhase2(int incomingVariableIndex,
                                             const Vector &alpha,
                                             Numerical::Double reducedCost){
    #ifndef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    m_boundflips.clear();
    m_boundflips.reserve(alpha.nonZeros());
    m_breakpointHandler.init(alpha.nonZeros() * 2);

    m_outgoingVariableIndex = -1;
    m_primalSteplength = 0;

    //determining t<=0 ot t>=0 cases
    if (reducedCost < 0) {
        m_tPositive = true;
    } else {
        m_tPositive = false;
    }

    generateBreakpointsPhase2(alpha);
    m_breakpointHandler.selectMethod();

    if ( m_breakpointHandler.getNumberOfBreakpoints() > 0 ) {
        const BreakpointHandler::BreakPoint * breakpoint = m_breakpointHandler.getBreakpoint(0);
        Numerical::Double boundOfIncoming = m_model.getVariable(incomingVariableIndex).getUpperBound() - m_model.getVariable(incomingVariableIndex).getLowerBound();

        if ( breakpoint->value < boundOfIncoming ) {
            m_outgoingVariableIndex = breakpoint->variableIndex;
            m_primalSteplength = breakpoint->value;

            if (breakpoint->value < 0){
                LPERROR("Negative ratio in phase2! "<<breakpoint->value);
                LPINFO("t_pos: "<<m_tPositive<<
                       " xb: "<<m_basicVariableValues.at(breakpoint->variableIndex)<<
                       " alpha: "<<alpha.at(m_outgoingVariableIndex)<<
                       " lb: "<<m_model.getVariable(m_basishead[m_outgoingVariableIndex]).getLowerBound()<<
                       " ub: "<<m_model.getVariable(m_basishead[m_outgoingVariableIndex]).getUpperBound());
            }
      //boundflip
        } else {
//            LPINFO("Boundflip in phase 2!");
            m_boundflips.push_back(incomingVariableIndex);
            m_outgoingVariableIndex = -1;
            m_primalSteplength = boundOfIncoming;
        }
  //no breakpoints found
    } else{
        LPWARNING(" - Ratiotest - No breakpoint found!");
    }
}
