#include <simplex/primalratiotest.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>

static const int nonlinearPrimalPhaseIFunction =
        SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_primal_phaseI_function");

void PrimalRatiotest::shift(std::vector<BreakPoint>* breakpoints,
                            unsigned int startId,unsigned int stopId) {
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
void PrimalRatiotest::getNextElement(std::vector<BreakPoint>* breakpoints,
                                     unsigned int length) {
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

PrimalRatiotest::PrimalRatiotest(const SimplexModel &model,
                                 const Vector& basicVariableValues,
                                 const std::vector<int>& basishead,
                                 const IndexList<>& basicVariableFeasibilities,
                                 const IndexList<const Numerical::Double *>& variableStates
                                 //PrimalRatiotestUpdater& primalRatiotestUpdater
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
    m_pivotThreshold(SimplexParameterHandler::getInstance().getDoubleParameterValue("e_pivot"))
    //m_primalRatiotestUpdater(primalRatiotestUpdater)
{

}

void PrimalRatiotest::generateBreakpointsPhase1(const Vector &alpha,
                                                int incomingVariableIndex,
                                                Numerical::Double phaseIReducedCost)
{
    //determining t>=0 or t<=0 cases
        if (phaseIReducedCost < 0) {
           m_tPositive = true;
        } else{
            m_tPositive = false;
        }

    //computing ratios
        m_breakpoints.reserve(alpha.nonZeros()*2);
        BreakPoint currentRatio;
        currentRatio.functionValue = m_phaseIObjectiveValue;
        currentRatio.index = -1;
        currentRatio.value = 0;
        m_breakpoints.push_back(currentRatio);
        m_boundflips.clear();
        m_boundflips.reserve(alpha.nonZeros());

        IndexList<>::Iterator it;
        IndexList<>::Iterator endit;
        unsigned int variableIndex = 0;
        unsigned int basisIndex = 0;
        Variable::VARIABLE_TYPE typeOfIthVariable;
        __UNUSED(typeOfIthVariable) //To avoid a warning

      //t>=0 case
        if (m_tPositive) {

        //computing ratios in M
            m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::MINUS);
            for (; it != endit; it++) {
                basisIndex = it.getData();
                variableIndex = m_basishead.at(basisIndex);
                if (alpha.at(basisIndex) < 0) {
                    currentRatio.index = basisIndex;
                    currentRatio.value = (m_basicVariableValues.at(basisIndex) -
                            m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                    currentRatio.functionValue = 0;
//                    LPINFO("M ratio L: "<<currentRatio.value << " index: "<< currentRatio.index << " alpha " << alpha.at(basisIndex));
                    m_breakpoints.push_back(currentRatio);
                    if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
//                        LPINFO("M ratio U: "<<currentRatio.value << " index: "<< currentRatio.index << " alpha " << alpha.at(basisIndex));
                        m_breakpoints.push_back(currentRatio);
                    } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                        currentRatio.value = (m_basicVariableValues.at(basisIndex) -
                                              m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                        m_breakpoints.push_back(currentRatio);
//                        LPINFO("M ratio U: "<<currentRatio.value << " index: "<< currentRatio.index << " alpha " << alpha.at(basisIndex));
                    }
                }
            }

        //computing ratios in P
            m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::PLUS);
            for (; it != endit; it++) {
                basisIndex = it.getData();
                variableIndex = m_basishead.at(basisIndex);
                if (alpha.at(basisIndex) > 0) {
                    currentRatio.index = basisIndex;
                    currentRatio.value = (m_basicVariableValues.at(basisIndex) -
                                          m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                    currentRatio.functionValue = 0;
//                    LPINFO("P ratio U: "<<currentRatio.value << " index: "<< currentRatio.index << " alpha " << alpha.at(basisIndex));
                    m_breakpoints.push_back(currentRatio);
                    if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
//                        LPINFO("P ratio L: "<<currentRatio.value << " index: "<< currentRatio.index << " alpha " << alpha.at(basisIndex));
                        m_breakpoints.push_back(currentRatio);
                    } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                        currentRatio.value = (m_basicVariableValues.at(basisIndex) -
                                              m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                        m_breakpoints.push_back(currentRatio);
//                        LPINFO("P ratio L: "<<currentRatio.value << " index: "<< currentRatio.index << " alpha " << alpha.at(basisIndex));
                    }
                }
            }

        //computing ratios in F
            m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);
            for (; it != endit; it++) {
                basisIndex = it.getData();
                variableIndex = m_basishead.at(basisIndex);
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();
              //F->P
                if (alpha.at(basisIndex) < 0 &&
                        m_model.getVariable(variableIndex).getUpperBound() != Numerical::Infinity) {
                        currentRatio.index = basisIndex;
                        currentRatio.value = (m_basicVariableValues.at(basisIndex) -
                                              m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                        currentRatio.functionValue = 0;
                        m_breakpoints.push_back(currentRatio);
//                        LPINFO("F ratio P: "<<currentRatio.value << " index: "<< currentRatio.index << " alpha " << alpha.at(basisIndex) << " type: "<<typeOfIthVariable << " basisindex " << basisIndex << " varindex " << variableIndex );
              //F->M
                } else if (alpha.at(basisIndex) > 0 &&
                           m_model.getVariable(variableIndex).getLowerBound() != - Numerical::Infinity) {
                        currentRatio.index = basisIndex;
                        currentRatio.value = (m_basicVariableValues.at(basisIndex) -
                                              m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                        currentRatio.functionValue = 0;
                        m_breakpoints.push_back(currentRatio);
//                        LPINFO("F ratio M: "<<currentRatio.value << " index: "<< currentRatio.index << " alpha " << alpha.at(basisIndex) << " type: "<<typeOfIthVariable << " basisindex " << basisIndex << " varindex " << variableIndex );
                    }
            }

      //t<=0 case
        } else{
            //computing ratios in M
                m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::MINUS);
                for (; it != endit; it++) {
                    basisIndex = it.getData();
                    variableIndex = m_basishead.at(basisIndex);
                    if (alpha.at(basisIndex) > 0) {
                        currentRatio.index = basisIndex;
                        currentRatio.value = - (m_basicVariableValues.at(basisIndex) -
                                                m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                        currentRatio.functionValue = 0;
                        m_breakpoints.push_back(currentRatio);
                        if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                            m_breakpoints.push_back(currentRatio);
                        } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                            currentRatio.value = - (m_basicVariableValues.at(basisIndex) -
                                                    m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                            m_breakpoints.push_back(currentRatio);
                        }
                    }
                }

            //computing ratios in P
                m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::PLUS);
                for (; it != endit; it++) {
                    basisIndex = it.getData();
                    variableIndex = m_basishead.at(basisIndex);
                    if (alpha.at(basisIndex) < 0) {
                        currentRatio.index = basisIndex;
                        currentRatio.value = - (m_basicVariableValues.at(basisIndex) -
                                                m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                        currentRatio.functionValue = 0;
                        m_breakpoints.push_back(currentRatio);
                        if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                            m_breakpoints.push_back(currentRatio);
                        } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                            currentRatio.value = - (m_basicVariableValues.at(basisIndex) -
                                                    m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                            m_breakpoints.push_back(currentRatio);
                        }
                    }
                }

            //computing ratios in F
                m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);
                for (; it != endit; it++) {
                    basisIndex = it.getData();
                    variableIndex = m_basishead.at(basisIndex);
                    typeOfIthVariable = m_model.getVariable(variableIndex).getType();
                  //F->P
                    if (alpha.at(basisIndex) > 0 &&
                            m_model.getVariable(variableIndex).getUpperBound() != Numerical::Infinity) {
                            currentRatio.index = basisIndex;
                            currentRatio.value = -(m_basicVariableValues.at(basisIndex) -
                                                   m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                            currentRatio.functionValue = 0;
                            m_breakpoints.push_back(currentRatio);
                  //F->M
                    } else if (alpha.at(basisIndex) < 0 &&
                               m_model.getVariable(variableIndex).getLowerBound() != - Numerical::Infinity) {
                            currentRatio.index = basisIndex;
                            currentRatio.value = - (m_basicVariableValues.at(basisIndex) -
                                                    m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                            currentRatio.functionValue = 0;
                            m_breakpoints.push_back(currentRatio);
                        }
                }
        }
      //breakpoint defined by the bound of the incoming variable
        if (m_model.getVariable(incomingVariableIndex).getType() == Variable::BOUNDED) {
            currentRatio.index = -2;
            currentRatio.functionValue = 0;
            currentRatio.value = m_model.getVariable(incomingVariableIndex).getUpperBound() -
                                 m_model.getVariable(incomingVariableIndex).getLowerBound();
        }

//        LPWARNING("breakpoints\t\t\t|\talpha values\t|");
//        for(unsigned i=0;i<m_breakpoints.size();i++){
//            if(m_breakpoints[i].index != -1){
//                LPWARNING(m_breakpoints[i]<<"     \t|\t"<<alpha.at(m_breakpoints[i].index)<<"    \t|\t"<< m_variableStates.where(variableIndex));
//            } else {
//                LPWARNING(m_breakpoints[i]<<"\t|\t"<<" - ");
//            }
//        }
}

void PrimalRatiotest::computeFunctionPhase1(const Vector &alpha,
                                            unsigned int& iterationCounter,
                                            Numerical::Double functionSlope)
{
    unsigned int length = m_breakpoints.size();
    while (functionSlope >= 0 && iterationCounter < length-1) {
        iterationCounter++;
        getNextElement(&m_breakpoints,length-iterationCounter);

        m_phaseIObjectiveValue += functionSlope * (m_breakpoints.at(length-1-iterationCounter).value -
                m_breakpoints[length-iterationCounter].value);
        m_breakpoints.at(length-1-iterationCounter).functionValue = m_phaseIObjectiveValue;
      //ratio defined by the bound of the incoming variable
      //TODO: nem hatékony
        if (m_breakpoints.at(length-1-iterationCounter).index == -1 ) {
            functionSlope -= 1;
        } else{
            functionSlope -= Numerical::fabs(alpha.at(m_breakpoints.at(length-1-iterationCounter).index));
        }
    }
    m_primalSteplength = m_tPositive ? m_breakpoints.at(length-1-iterationCounter).value :
                                       - m_breakpoints.at(length-1-iterationCounter).value;
    m_outgoingVariableIndex = m_breakpoints.at(
                length-1-iterationCounter).index;
}

void PrimalRatiotest::useNumericalThresholdPhase1(unsigned int iterationCounter,
                                          const Vector& alpha,
                                          Numerical::Double& functionSlope)
{
    unsigned int length = m_breakpoints.size(),
    alphaId = length-1-iterationCounter, prevAlphaId = alphaId, nextAlphaId = alphaId,
    prevIterationCounter = 0, nextIterationCounter = 0;

#ifndef NDEBUG
    if(m_breakpoints[alphaId].index != -1){
    LPWARNING("BAD NUMERICAL VALUE IN PHASE I,  ACTIVATING THRESHOLD for"
                                          "variable " << m_breakpoints[alphaId].index
                                          <<" with alpha" <<alpha.at(m_breakpoints[alphaId].index)
                                          <<" at f= "<<m_breakpoints[alphaId].functionValue);
    } else {
        LPWARNING("BAD NUMERICAL VALUE IN PHASE I for the INITIAL BREAKPOINT (-1)");
    }
#endif

    Numerical::Double prevObjValue = m_phaseIObjectiveValue, nextObjValue = m_phaseIObjectiveValue;
    if (iterationCounter > 0) {
        prevAlphaId++;
        prevIterationCounter++;
        prevObjValue = m_breakpoints[prevAlphaId].functionValue;
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
    } else{
        nextObjValue = -Numerical::Infinity;
    }
    bool prevIsBetter = nextObjValue > prevObjValue ? false : true;
    bool step = true;

    while (step &&( prevIsBetter ? Numerical::fabs(alpha.at(prevAlphaId)) > m_pivotThreshold:
                    Numerical::fabs(alpha.at(nextAlphaId)) > m_pivotThreshold)) {
        step = false;
        if (prevIsBetter) {
            if (iterationCounter - prevIterationCounter > 0) {
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
        m_outgoingVariableIndex = -1;
        m_primalSteplength = 0;
        m_phaseIObjectiveValue = m_breakpoints[length-1].functionValue;
#ifndef NDEBUG
        LPERROR(" - Threshold Error - No breakpoint found! num of breakpoints: " << m_breakpoints.size());
#endif
    } else if (prevIsBetter) {
        m_phaseIObjectiveValue = m_breakpoints[prevAlphaId].functionValue;
        m_outgoingVariableIndex = m_breakpoints[prevAlphaId].index;
        m_primalSteplength = m_breakpoints[prevAlphaId].value;
    } else {
        m_phaseIObjectiveValue = m_breakpoints[nextAlphaId].functionValue;
        m_outgoingVariableIndex = m_breakpoints[nextAlphaId].index;
        m_primalSteplength = m_breakpoints[nextAlphaId].value;
    }

#ifndef NDEBUG
        LPWARNING("steps(prev;next): " << prevIterationCounter<<
                                              " ; " << nextIterationCounter<<"\n");
        LPWARNING(m_breakpoints[alphaId+nextIterationCounter-prevIterationCounter]<<" chosen");
#endif
}

void PrimalRatiotest::performRatiotestPhase1(int incomingVariableIndex,
                                             const Vector &alpha,
                                             Numerical::Double phaseIReducedCost,
                                             Numerical::Double phaseIObjectiveValue)
{
    Numerical::Double functionSlope = Numerical::fabs(phaseIReducedCost);
    unsigned int iterationCounter = 0,length = 0;

//    LPINFO("ratiotest phaseI redcost: "<<phaseIReducedCost);

    m_phaseIObjectiveValue = phaseIObjectiveValue;
    m_outgoingVariableIndex = -1;
    m_primalSteplength = 0;

    m_breakpoints.clear();

    generateBreakpointsPhase1(alpha,incomingVariableIndex,phaseIReducedCost);
    length = m_breakpoints.size();


    //Init the heap with the breakpoint at 0
    getNextElement(&m_breakpoints,length);

    if (m_breakpoints.size() > 1) {
        int num = nonlinearPrimalPhaseIFunction;
        //fake feasible variables
        while(m_breakpoints[length-1-iterationCounter].index != -1) {
            functionSlope -= Numerical::fabs(alpha.at(m_breakpoints[length-1-iterationCounter].index));

            iterationCounter++;
            getNextElement(&m_breakpoints,length-iterationCounter);

            if (functionSlope < 0) {
                LPERROR("functionSlope < 0 in the beginning (PHASE I) - ougoing index: "<<m_outgoingVariableIndex);

                return;
            }
        }
        //using traditional one step method
        if (num == 0) {
            iterationCounter++;
            getNextElement(&m_breakpoints,length-1);

            m_phaseIObjectiveValue += functionSlope * (m_breakpoints[length-1-iterationCounter].value -
                    m_breakpoints[length-iterationCounter].value);
            m_breakpoints[length-1].functionValue = m_phaseIObjectiveValue;
           } else{
        //using piecewise linear function
                computeFunctionPhase1(alpha, iterationCounter, functionSlope);
        //numerical issues
                if(num == 2){
                    useNumericalThresholdPhase1(iterationCounter,alpha,functionSlope);
                }
           }
   //no breakpoints found
    } else {
   //TODO: throwing exception?
    }
    if(m_outgoingVariableIndex == -2){
        m_boundflips.push_back(incomingVariableIndex);
        m_primalSteplength = 0;
        m_outgoingVariableIndex = -1;
    }

}

void PrimalRatiotest::generateBreakpointsPhase2(const Vector &alpha,
                                                Numerical::Double reducedCost)
{
    BreakPoint currentBreakpoint;
    currentBreakpoint.functionValue = 0;

    //determining t<=0 ot t>=0 cases
        if (reducedCost < 0) {
            m_tPositive = true;
        } else {
            m_tPositive = false;
        }

    unsigned int variableIndex = 0;

#ifndef NDEBUG
    if (m_basicVariableValues.getType() == Vector::SPARSE_VECTOR) LPWARNING("x_b is sparse vector!");
#endif

    //defining breakpoints
    for (unsigned int basisIndex = 0; basisIndex < m_basicVariableValues.length(); basisIndex++) {
//            LPINFO("basisid: " <<basisIndex<<" alpha: "<<alpha.at(basisIndex)<<" t: "<<m_tPositive);
        variableIndex = m_basishead.at(basisIndex);
      //t>=0 case
        if (m_tPositive) {
            if ( alpha.at(basisIndex) > 0 &&
                 m_model.getVariable(variableIndex).getLowerBound() != - Numerical::Infinity) {
                currentBreakpoint.index = basisIndex;
                currentBreakpoint.value = (m_basicVariableValues.at(basisIndex) -
                                          m_model.getVariable(variableIndex).getLowerBound()) /
                                          alpha.at(basisIndex);
                m_breakpoints.push_back(currentBreakpoint);
            } else if (alpha.at(basisIndex) < 0 &&
                       m_model.getVariable(variableIndex).getUpperBound() != Numerical::Infinity) {
                currentBreakpoint.index = basisIndex;
                currentBreakpoint.value = (m_basicVariableValues.at(basisIndex) -
                                           m_model.getVariable(variableIndex).getUpperBound()) /
                                           alpha.at(basisIndex);
                m_breakpoints.push_back(currentBreakpoint);
            }
      //t<=0 case
        } else{
            if ( alpha.at(basisIndex) < 0 &&
                 m_model.getVariable(variableIndex).getLowerBound() != - Numerical::Infinity) {
                currentBreakpoint.index = basisIndex;
                currentBreakpoint.value = - (m_basicVariableValues.at(basisIndex) -
                                          m_model.getVariable(variableIndex).getLowerBound()) /
                                          alpha.at(basisIndex);
                m_breakpoints.push_back(currentBreakpoint);
            } else if ( alpha.at(basisIndex) > 0 &&
                       m_model.getVariable(variableIndex).getUpperBound() != Numerical::Infinity) {
                currentBreakpoint.index = basisIndex;
                currentBreakpoint.value = - (m_basicVariableValues.at(basisIndex) -
                                          m_model.getVariable(variableIndex).getUpperBound() )/
                                          alpha.at(basisIndex);
                m_breakpoints.push_back(currentBreakpoint);
            }
        }
    }
//    LPINFO("num of bpts: "<<m_breakpoints.size());
//    for(int i=0;i<m_breakpoints.size();i++){
//        LPINFO(m_breakpoints[i]);
//    }
}

void PrimalRatiotest::performRatiotestPhase2(int incomingVariableIndex,
                                             const Vector &alpha,
                                             Numerical::Double reducedCost){
    #ifndef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    m_breakpoints.clear();
    m_breakpoints.reserve(alpha.length()*2);
    m_boundflips.clear();
    m_boundflips.reserve(alpha.nonZeros());

    generateBreakpointsPhase2(alpha,reducedCost);
    unsigned int length = m_breakpoints.size();

  //determining primal steplength, outgoing variable
    getNextElement(&m_breakpoints,length);

    if ( m_breakpoints.size() > 0 ) {
        if ( m_breakpoints.at(length-1).value < ( m_model.getVariable(incomingVariableIndex).getUpperBound() -
                                                 m_model.getVariable(incomingVariableIndex).getLowerBound()) ) {
            m_outgoingVariableIndex = m_breakpoints.at(length-1).index;
            m_primalSteplength = m_breakpoints.at(length-1).value;

      //boundflip
        } else {
            m_boundflips.at(incomingVariableIndex) = 1;
            m_outgoingVariableIndex = -1;
            m_primalSteplength = (m_model.getVariable(incomingVariableIndex).getUpperBound() -
                                  m_model.getVariable(incomingVariableIndex).getLowerBound());
        }
  //no breakpoints found
    } else{
    //TODO: throwing exception
    }
}
