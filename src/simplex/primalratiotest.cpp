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
                                 const IndexList<const Numerical::Double *>& variableStates,
                                 const Vector& reducedcosts
                                 //PrimalRatiotestUpdater& primalRatiotestUpdater
                                 ):
    m_model(model),
    m_basicVariableValues(basicVariableValues),
    m_basishead(basishead),
    m_basicVariableFeasibilities(basicVariableFeasibilities),
    m_variableStates(variableStates),
    m_reducedcosts(reducedcosts),
    m_outgoingVariableIndex(-1),
    m_dualSteplength(0),
    m_primalSteplength(0),
    m_phaseIObjectiveValue(0),
    m_phaseIIObjectiveValue(0)
    //m_primalRatiotestUpdater(primalRatiotestUpdater)
{

}

void PrimalRatiotest::performRatiotestPhase1(int incomingVariableIndex,
                                             const Vector &alpha,
                                             Numerical::Double phaseIReducedCost,
                                             Numerical::Double phaseIObjectiveValue)
{
//    m_primalRatiotestUpdater.m_updateVector.clear();
//    m_primalRatiotestUpdater.m_updateVector.reserve(
//                m_model.getColumnCount() + m_model.getRowCount());
    #ifdef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    Numerical::Double functionSlope = Numerical::fabs(phaseIReducedCost);
    m_phaseIObjectiveValue = phaseIObjectiveValue;

//determining t>=0 or t<=0 cases
    bool tPositive;
    if (phaseIReducedCost < 0) {
       tPositive = true;
    } else{
        tPositive = false;
    }

//computing ratios
    std::vector <BreakPoint> breakpoints;
    breakpoints.reserve(alpha.nonZeros()*2);
    BreakPoint currentRatio;
    currentRatio.functionValue = m_phaseIObjectiveValue;
    currentRatio.index = -1;
    currentRatio.value = 0;
    breakpoints.push_back(currentRatio);
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
                breakpoints.push_back(currentRatio);
                if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                    breakpoints.push_back(currentRatio);
                } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                    currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                    breakpoints.push_back(currentRatio);
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
                breakpoints.push_back(currentRatio);
                if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                    breakpoints.push_back(currentRatio);
                } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                    currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                    breakpoints.push_back(currentRatio);
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
                    breakpoints.push_back(currentRatio);
          //F->M
            } else if (alpha.at(basisIndex) > 0 &&
                       m_model.getVariable(variableIndex).getLowerBound() != - Numerical::Infinity) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                    currentRatio.functionValue = 0;
                    breakpoints.push_back(currentRatio);
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
                    currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                    currentRatio.functionValue = 0;
                    breakpoints.push_back(currentRatio);
                    if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                        breakpoints.push_back(currentRatio);
                    } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                        currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                        breakpoints.push_back(currentRatio);
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
                    currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                    currentRatio.functionValue = 0;
                    breakpoints.push_back(currentRatio);
                    if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                        breakpoints.push_back(currentRatio);
                    } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                        currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                        breakpoints.push_back(currentRatio);
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
                        currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(basisIndex);
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
              //F->M
                } else if (alpha.at(basisIndex) < 0 &&
                           m_model.getVariable(variableIndex).getLowerBound() != - Numerical::Infinity) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = (m_basicVariableValues.at(basisIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(basisIndex);
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
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
//sorting ratios, determining funciton maximum

    unsigned int iterationCounter = 0,length = breakpoints.size();

    //Init the heap with the breakpoint at 0
    getNextElement(&breakpoints,length);

    if (breakpoints.size() > 1) {
        int num = nonlinearPrimalPhaseIFunction;
        switch (num) {
          case 0:
      //using traditional one step method
            iterationCounter++;
            getNextElement(&breakpoints,length-1);
            m_phaseIObjectiveValue += functionSlope * (breakpoints[length-1].value -
                    breakpoints[length].value);
            breakpoints[length-1].functionValue = m_phaseIObjectiveValue;
          break;
    //using piecewise linear function
          case 1:
            while (functionSlope > 0 && iterationCounter < length-1) {
                iterationCounter++;
                getNextElement(&breakpoints,length-iterationCounter);

                m_phaseIObjectiveValue += functionSlope * (breakpoints.at(length-1-iterationCounter).value -
                        breakpoints[length-iterationCounter].value);
                breakpoints.at(length-1-iterationCounter).functionValue = m_phaseIObjectiveValue;
              //ratio defined by the bound of the incoming variable
              //TODO: nem hatékony
                if (breakpoints.at(length-1-iterationCounter).index == -1 ) {
                    functionSlope -= 1;
                } else{
                    functionSlope -= Numerical::fabs(alpha.at(breakpoints.at(length-1-iterationCounter).index));
                }
            }
          break;
    //numerical issues
//          case 2:
//          break;
        }

//determining primal, dual steplength, outgoing variable
        if (breakpoints[length-1-iterationCounter].index != -1)  {

            m_outgoingVariableIndex = breakpoints[length-1-iterationCounter].index;

            m_primalSteplength = breakpoints[length-1-iterationCounter].value;

            m_dualSteplength = m_reducedcosts.at(incomingVariableIndex) / alpha.at(m_outgoingVariableIndex);

      //boundflip
        } else {
            m_boundflips.at(incomingVariableIndex) = 1;

            m_outgoingVariableIndex = -1;

            m_dualSteplength = 0;

            m_primalSteplength = breakpoints[length-1-iterationCounter].value -
                    Numerical::fabs(m_model.getVariable(incomingVariableIndex).getUpperBound()-
                    m_model.getVariable(incomingVariableIndex).getLowerBound());

        }
   //no breakpoints found
    } else {
   //TODO: throwing exception?
    }
}

void PrimalRatiotest::performRatiotestPhase2(int incomingVariableIndex,
                                             const Vector &alpha){
    #ifdef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    BreakPoint currentBreakpoint;
    std::vector<BreakPoint> breakpoints;
    breakpoints.reserve(alpha.length()*2);
    m_boundflips.clear();
    m_boundflips.reserve(alpha.nonZeros());

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
                breakpoints.push_back(currentBreakpoint);
            } else if (alpha.at(basisIndex) < 0 &&
                       m_model.getVariable(variableIndex).getUpperBound() != Numerical::Infinity) {
                currentBreakpoint.index = variableIndex;
                currentBreakpoint.value = (m_basicVariableValues.at(basisIndex) -
                        (m_model.getVariable(variableIndex).getUpperBound() -
                        m_model.getVariable(variableIndex).getLowerBound()) ) /
                        alpha.at(basisIndex);
                breakpoints.push_back(currentBreakpoint);
            }
      //t<=0 case
        } else{
            if ( - alpha.at(basisIndex) > 0 &&
                 m_model.getVariable(variableIndex).getLowerBound() != - Numerical::Infinity) {
                currentBreakpoint.index = variableIndex;
                currentBreakpoint.value = (m_basicVariableValues.at(basisIndex) -
                                          m_model.getVariable(variableIndex).getLowerBound() ) /
                                          alpha.at(basisIndex);
                breakpoints.push_back(currentBreakpoint);
            } else if ( - alpha.at(basisIndex) < 0 &&
                       m_model.getVariable(variableIndex).getUpperBound() != Numerical::Infinity) {
                currentBreakpoint.index = variableIndex;
                currentBreakpoint.value = (m_basicVariableValues.at(basisIndex) -
                        (m_model.getVariable(variableIndex).getUpperBound() -
                        m_model.getVariable(variableIndex).getLowerBound()) ) /
                        alpha.at(basisIndex);
                breakpoints.push_back(currentBreakpoint);
            }
        }
    }
  //determining primal,dual steplength, outgoing variable
    unsigned int length = breakpoints.size();
    getNextElement(&breakpoints,length);
    getNextElement(&breakpoints,length-1);

    if ( breakpoints.size() > 1 ) {
        if ( breakpoints.at(length-1).value < ( m_model.getVariable(incomingVariableIndex).getUpperBound() -
                                                 m_model.getVariable(incomingVariableIndex).getLowerBound()) ) {

            m_outgoingVariableIndex = breakpoints.at(length-1).index;

            m_primalSteplength = breakpoints.at(length-1).value;

            m_dualSteplength = m_reducedcosts.at(incomingVariableIndex) / alpha.at(m_outgoingVariableIndex);
      //boundflip
        } else {
            m_boundflips.at(incomingVariableIndex) = 1;

            m_outgoingVariableIndex = -1;

            m_primalSteplength = (m_model.getVariable(incomingVariableIndex).getUpperBound() -
                                  m_model.getVariable(incomingVariableIndex).getLowerBound());

            m_dualSteplength = 0;
        }
  //no breakpoints found
    } else{
    //TODO: throwing exception
    }
}
