#include <simplex/primalratiotest.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>

static const int nonlinearPrimalPhaseIFunction =
        SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_primal_phaseI_function");

void PrimalRatiotest::shift(std::vector<BreakPoint>* breakpoints, unsigned int startId, unsigned int stopId) {
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
void PrimalRatiotest::getNextElement(std::vector<BreakPoint>* breakpoints, unsigned int length) {
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
    m_phaseIIObjectiveValue(0)
    //m_primalRatiotestUpdater(primalRatiotestUpdater)
{

}

void PrimalRatiotest::generateBreakpointsPhase1(const Vector &alpha, int incomingVariableIndex, Numerical::Double phaseIReducedCost)
{
    //determining t>=0 or t<=0 cases
        bool tPositive;
        if (phaseIReducedCost < 0) {
           tPositive = true;
        } else{
            tPositive = false;
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
        if (tPositive) {

        //computing ratios in M
            m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::MINUS);
            for (; it != endit; it++) {
                basisIndex = it.getData();
                variableIndex = m_basishead.at(basisIndex);
                if (alpha.at(basisIndex) < 0) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getLowerBound() / alpha.at(basisIndex);
                    currentRatio.functionValue = 0;
                    m_breakpoints.push_back(currentRatio);
                    if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                        m_breakpoints.push_back(currentRatio);
                    } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                        currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                        m_breakpoints.push_back(currentRatio);
                    }
                }
            }

        //computing ratios in P
            m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::PLUS);
            for (; it != endit; it++) {
                basisIndex = it.getData();
                variableIndex = m_basishead.at(basisIndex);
                if (alpha.at(basisIndex) > 0) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                    currentRatio.functionValue = 0;
                    m_breakpoints.push_back(currentRatio);
                    if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                        m_breakpoints.push_back(currentRatio);
                    } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                        currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
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
                if (alpha.at(basisIndex) < 0 &&
                        m_model.getVariable(variableIndex).getUpperBound() != Numerical::Infinity) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                        currentRatio.functionValue = 0;
                        m_breakpoints.push_back(currentRatio);
              //F->M
                } else if (alpha.at(basisIndex) > 0 &&
                           m_model.getVariable(variableIndex).getLowerBound() != - Numerical::Infinity) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                        currentRatio.functionValue = 0;
                        m_breakpoints.push_back(currentRatio);
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
                        currentRatio.index = variableIndex;
                        currentRatio.value = - (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                        currentRatio.functionValue = 0;
                        m_breakpoints.push_back(currentRatio);
                        if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                            m_breakpoints.push_back(currentRatio);
                        } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                            currentRatio.value = - (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
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
                        currentRatio.index = variableIndex;
                        currentRatio.value = - (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                        currentRatio.functionValue = 0;
                        m_breakpoints.push_back(currentRatio);
                        if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                            m_breakpoints.push_back(currentRatio);
                        } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                            currentRatio.value = - (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
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
                            currentRatio.index = variableIndex;
                            currentRatio.value = -(m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                            currentRatio.functionValue = 0;
                            m_breakpoints.push_back(currentRatio);
                  //F->M
                    } else if (alpha.at(basisIndex) < 0 &&
                               m_model.getVariable(variableIndex).getLowerBound() != - Numerical::Infinity) {
                            currentRatio.index = variableIndex;
                            currentRatio.value = - (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                            currentRatio.functionValue = 0;
                            m_breakpoints.push_back(currentRatio);
                        }
                }
        }
      //breakpoint defined by the bound of the incoming variable
        if (m_model.getVariable(incomingVariableIndex).getType() == Variable::BOUNDED) {
            currentRatio.index = -1;
            currentRatio.functionValue = 0;
            currentRatio.value =
                    m_model.getVariable(incomingVariableIndex).getUpperBound() -
                    m_model.getVariable(incomingVariableIndex).getLowerBound();
        }
}

void PrimalRatiotest::computeFunctionPhase1(const Vector &alpha, Numerical::Double functionSlope)
{
    unsigned int iterationCounter = 0,length = m_breakpoints.size();
    while (functionSlope > 0 && iterationCounter < length-1) {
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
}

void PrimalRatiotest::performRatiotestPhase1(int incomingVariableIndex,
                                             const Vector &alpha,
                                             Numerical::Double phaseIReducedCost,
                                             Numerical::Double phaseIObjectiveValue)
{
    Numerical::Double functionSlope = Numerical::fabs(phaseIReducedCost);
    unsigned int iterationCounter = 0,length = 0;

    m_phaseIObjectiveValue = phaseIObjectiveValue;
    m_outgoingVariableIndex = -1;
    m_primalSteplength = 0;

    generateBreakpointsPhase1(alpha,incomingVariableIndex,phaseIReducedCost);
    length = m_breakpoints.size();


    //Init the heap with the breakpoint at 0
    getNextElement(&m_breakpoints,length);

    if (m_breakpoints.size() > 1) {
        int num = nonlinearPrimalPhaseIFunction;
        //using traditional one step method
        if (num == 0) {
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
            iterationCounter++;
            getNextElement(&m_breakpoints,length-1);

            m_phaseIObjectiveValue += functionSlope * (m_breakpoints[length-1].value -
                    m_breakpoints[length].value);
            m_breakpoints[length-1].functionValue = m_phaseIObjectiveValue;
           } else{
        //using piecewise linear function
                computeFunctionPhase1(alpha,functionSlope);
        //numerical issues
//                if(num == 2){
//                }
           }

//determining primal steplength, outgoing variable
        if (m_breakpoints[length-1-iterationCounter].index != -1)  {
            m_outgoingVariableIndex = m_breakpoints[length-1-iterationCounter].index;
            m_primalSteplength = m_breakpoints[length-1-iterationCounter].value;

      //boundflip
        } else {
            m_boundflips.at(incomingVariableIndex) = 1;
            m_outgoingVariableIndex = -1;
            m_primalSteplength = m_breakpoints[length-1-iterationCounter].value -
                    Numerical::fabs(m_model.getVariable(incomingVariableIndex).getUpperBound()-
                    m_model.getVariable(incomingVariableIndex).getLowerBound());

        }
   //no breakpoints found
    } else {
   //TODO: throwing exception?
    }
}

void PrimalRatiotest::generateBreakpointsPhase2(const Vector &alpha,
                                                int incomingVariableIndex)
{
    BreakPoint currentBreakpoint;
    IndexList<>::Iterator it;
    IndexList<>::Iterator endit;
    m_basicVariableFeasibilities.getIterators(&it,&endit,Simplex::BASIC);

    //determining t<=0 ot t>=0 cases
        bool tPositive;
        if (m_variableStates.where(incomingVariableIndex) == Simplex::NONBASIC_AT_LB) {
            tPositive = true;
        } else {
            tPositive = false;
        }

        unsigned int variableIndex = 0;
        unsigned int basisIndex = 0;

      //defining breakpoints
        for (; it != endit; it++) {
            basisIndex = it.getData();
            variableIndex = m_basishead.at(basisIndex);
          //t>=0 case
            if (tPositive) {
                if ( alpha.at(basisIndex) > 0 &&
                     m_model.getVariable(variableIndex).getLowerBound() != - Numerical::Infinity) {
                    currentBreakpoint.index = variableIndex;
                    currentBreakpoint.value = (m_basicVariableValues.at(basisIndex) -
                                              m_model.getVariable(variableIndex).getLowerBound() ) /
                                              alpha.at(basisIndex);
                    m_breakpoints.push_back(currentBreakpoint);
                } else if (alpha.at(basisIndex) < 0 &&
                           m_model.getVariable(variableIndex).getUpperBound() != Numerical::Infinity) {
                    currentBreakpoint.index = variableIndex;
                    currentBreakpoint.value = (m_basicVariableValues.at(basisIndex) -
                            (m_model.getVariable(variableIndex).getUpperBound() -
                            m_model.getVariable(variableIndex).getLowerBound()) ) /
                            alpha.at(basisIndex);
                    m_breakpoints.push_back(currentBreakpoint);
                }
          //t<=0 case
            } else{
                if ( - alpha.at(basisIndex) > 0 &&
                     m_model.getVariable(variableIndex).getLowerBound() != - Numerical::Infinity) {
                    currentBreakpoint.index = variableIndex;
                    currentBreakpoint.value = (m_basicVariableValues.at(basisIndex) -
                                              m_model.getVariable(variableIndex).getLowerBound() ) /
                                              alpha.at(basisIndex);
                    m_breakpoints.push_back(currentBreakpoint);
                } else if ( - alpha.at(basisIndex) < 0 &&
                           m_model.getVariable(variableIndex).getUpperBound() != Numerical::Infinity) {
                    currentBreakpoint.index = variableIndex;
                    currentBreakpoint.value = (m_basicVariableValues.at(basisIndex) -
                            (m_model.getVariable(variableIndex).getUpperBound() -
                            m_model.getVariable(variableIndex).getLowerBound()) ) /
                            alpha.at(basisIndex);
                    m_breakpoints.push_back(currentBreakpoint);
                }
            }
        }
}

void PrimalRatiotest::performRatiotestPhase2(int incomingVariableIndex,
                                             const Vector &alpha){
    #ifdef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    m_breakpoints.reserve(alpha.length()*2);
    m_boundflips.clear();
    m_boundflips.reserve(alpha.nonZeros());

    generateBreakpointsPhase2(alpha,incomingVariableIndex);

  //determining primal steplength, outgoing variable
    unsigned int length = m_breakpoints.size();
    getNextElement(&m_breakpoints,length);
    getNextElement(&m_breakpoints,length-1);

    if ( m_breakpoints.size() > 1 ) {
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
