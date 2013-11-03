#include <simplex/primalratiotest.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>

static const double nonlinearPrimalPhaseIFunction =
        SimplexParameterHandler::getInstance().getParameterValue("nonlinear_primal_phaseI_function");

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
                                 const IndexList<>& variableFeasibilities,
                                 const IndexList<const Numerical::Double *>& variableStates,
                                 const Vector& reducedcosts
                                 //PrimalRatiotestUpdater& primalRatiotestUpdater
                                 ):
    m_model(model),
    m_basicVariableValues(basicVariableValues),
    m_variableFeasibilities(variableFeasibilities),
    m_variableStates(variableStates),
    m_reducedcosts(reducedcosts),
    m_outgoingVariableIndex(-1),
    m_dualSteplength(0),
    m_primalSteplength(0),
    m_phaseIObjectiveValue(0),
    m_phaseIIObjectiveValue(0),
    m_boundflip(false)
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
    IndexList<>::Iterator it;
    IndexList<>::Iterator endit;
    unsigned int variableIndex = 0;
    Variable::VARIABLE_TYPE typeOfIthVariable;

  //t>=0 case
    if (tPositive) {

    //computing ratios in M

        m_variableFeasibilities.getIterators(&it,&endit,Simplex::MINUS);
        for (; it != endit; it++) {
            variableIndex = it.getData();
            if (alpha.at(variableIndex) < 0) {
                currentRatio.index = variableIndex;
                currentRatio.value = (m_basicVariableValues.at(variableIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(variableIndex);
                currentRatio.functionValue = 0;
                breakpoints.push_back(currentRatio);
                if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                    breakpoints.push_back(currentRatio);
                } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                    currentRatio.value = (m_basicVariableValues.at(variableIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(variableIndex);
                    breakpoints.push_back(currentRatio);
                }
            }
        }

    //computing ratios in P

        m_variableFeasibilities.getIterators(&it,&endit,Simplex::PLUS);
        for (; it != endit; it++) {
            variableIndex = it.getData();
            if (alpha.at(variableIndex) > 0) {
                currentRatio.index = variableIndex;
                currentRatio.value = (m_basicVariableValues.at(variableIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(variableIndex);
                currentRatio.functionValue = 0;
                breakpoints.push_back(currentRatio);
                if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                    breakpoints.push_back(currentRatio);
                } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                    currentRatio.value = (m_basicVariableValues.at(variableIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(variableIndex);
                    breakpoints.push_back(currentRatio);
                }
            }
        }

    //computing ratios in F

        m_variableFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);
        for (; it != endit; it++) {
            variableIndex = it.getData();
            typeOfIthVariable = m_model.getVariable(variableIndex).getType();
          //F->P
            if (alpha.at(variableIndex) < 0 && (
                (typeOfIthVariable == Variable::MINUS) ||
                (typeOfIthVariable == Variable::FIXED) ||
                (typeOfIthVariable == Variable::BOUNDED) )) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = (m_basicVariableValues.at(variableIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(variableIndex);
                    currentRatio.functionValue = 0;
                    breakpoints.push_back(currentRatio);
          //F->M
            } else if (alpha.at(variableIndex) > 0 && (
                        (typeOfIthVariable == Variable::PLUS) ||
                        (typeOfIthVariable == Variable::FIXED) ||
                        (typeOfIthVariable == Variable::BOUNDED) )) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = (m_basicVariableValues.at(variableIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(variableIndex);
                    currentRatio.functionValue = 0;
                    breakpoints.push_back(currentRatio);
                }
        }

  //t<=0 case
    } else{

        //computing ratios in M

            m_variableFeasibilities.getIterators(&it,&endit,Simplex::MINUS);
            for (; it != endit; it++) {
                variableIndex = it.getData();
                if (alpha.at(variableIndex) > 0) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = (m_basicVariableValues.at(variableIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(variableIndex);
                    currentRatio.functionValue = 0;
                    breakpoints.push_back(currentRatio);
                    if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                        breakpoints.push_back(currentRatio);
                    } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                        currentRatio.value = (m_basicVariableValues.at(variableIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(variableIndex);
                        breakpoints.push_back(currentRatio);
                    }
                }
            }

        //computing ratios in P

            m_variableFeasibilities.getIterators(&it,&endit,Simplex::PLUS);
            for (; it != endit; it++) {
                variableIndex = it.getData();
                if (alpha.at(variableIndex) < 0) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = (m_basicVariableValues.at(variableIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(variableIndex);
                    currentRatio.functionValue = 0;
                    breakpoints.push_back(currentRatio);
                    if (m_model.getVariable(variableIndex).getType() == Variable::FIXED) {
                        breakpoints.push_back(currentRatio);
                    } else if (m_model.getVariable(variableIndex).getType() == Variable::BOUNDED) {
                        currentRatio.value = (m_basicVariableValues.at(variableIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(variableIndex);
                        breakpoints.push_back(currentRatio);
                    }
                }
            }

        //computing ratios in F

            m_variableFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);
            for (; it != endit; it++) {
                variableIndex = it.getData();
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();
              //F->P
                if (alpha.at(variableIndex) > 0 && (
                    (typeOfIthVariable == Variable::MINUS) ||
                    (typeOfIthVariable == Variable::FIXED) ||
                    (typeOfIthVariable == Variable::BOUNDED) )) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = (m_basicVariableValues.at(variableIndex) - m_model.getVariable(variableIndex).getUpperBound()) / alpha.at(variableIndex);
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
              //F->M
                } else if (alpha.at(variableIndex) < 0 && (
                            (typeOfIthVariable == Variable::PLUS) ||
                            (typeOfIthVariable == Variable::FIXED) ||
                            (typeOfIthVariable == Variable::BOUNDED) )) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = (m_basicVariableValues.at(variableIndex) - m_model.getVariable(variableIndex).getLowerBound()) / alpha.at(variableIndex);
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    }
            }
    }

//sorting ratios, determining funciton maximum

    unsigned int iterationCounter = 0,length = breakpoints.size();

    //Init the heap with the breakpoint at 0
    getNextElement(&breakpoints,length);

    if (breakpoints.size() > 1) {
        int num = (int)nonlinearPrimalPhaseIFunction;
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
                functionSlope -= Numerical::fabs(alpha.at(breakpoints.at(length-1-iterationCounter).index));
            }
          break;
    //numerical issues
//          case 2:
//          break;
        }

//determining primal, dual steplength, outgoing variable
        if (breakpoints[length-1-iterationCounter].value <
                Numerical::fabs(m_model.getVariable(incomingVariableIndex).getUpperBound()-
                m_model.getVariable(incomingVariableIndex).getLowerBound()) ) {

            m_outgoingVariableIndex = breakpoints[length-1-iterationCounter].index;

            m_primalSteplength = breakpoints[length-1-iterationCounter].value;

            m_dualSteplength = m_reducedcosts.at(incomingVariableIndex) / alpha.at(incomingVariableIndex);

      //boundflip
        } else {

            m_boundflip = true;

            m_outgoingVariableIndex = -1;

            m_dualSteplength = m_reducedcosts.at(incomingVariableIndex) / alpha.at(incomingVariableIndex);

            m_primalSteplength = breakpoints[length-1-iterationCounter].value -
                    Numerical::fabs(m_model.getVariable(incomingVariableIndex).getUpperBound()-
                    m_model.getVariable(incomingVariableIndex).getLowerBound());

        }
   //no breakpoints found
    } else {

        m_primalSteplength = 0;
        m_dualSteplength = 0;
        m_outgoingVariableIndex = -1;
      //TODO: throwing exception?
    }
}

void PrimalRatiotest::performRatiotestPhase2(int incomingVariableIndex,
                                             const Vector &alpha)
{
    BreakPoint currentBreakpoint;
    std::vector<BreakPoint> breakpoints;
    breakpoints.reserve(alpha.length()*2);

    Vector::Iterator it = m_basicVariableValues.begin();
    Vector::Iterator endit = m_basicVariableValues.end();
    Variable::VARIABLE_TYPE typeOfIthVariable;


//determining t<=0 ot t>=0 cases
    bool tPositive;
    if (m_variableStates.where(incomingVariableIndex) == Simplex::NONBASIC_AT_LB) {
        tPositive = true;
    } else if (m_variableStates.where(incomingVariableIndex) == Simplex::NONBASIC_AT_UB) {
        tPositive = false;
    } //else throwing exception?

  //index?? max problem?
    for (; it != endit; it++) {
        int index = *it;
        typeOfIthVariable = m_model.getVariable(index).getType();

      //t>=0 case
        if (tPositive) {
            if ( alpha.at(index) > 0 && (
                     typeOfIthVariable == Variable::FIXED ||
                     typeOfIthVariable == Variable::BOUNDED ||
                     typeOfIthVariable == Variable::PLUS) ) {
                currentBreakpoint.index = index;
                currentBreakpoint.value = m_basicVariableValues.at(index) / alpha.at(index);
                breakpoints.push_back(currentBreakpoint);
            } else if (alpha.at(index) < 0 && (
                           typeOfIthVariable == Variable::FIXED ||
                           typeOfIthVariable == Variable::BOUNDED ||
                           typeOfIthVariable == Variable::MINUS) ) {
                currentBreakpoint.index = index;
                currentBreakpoint.value = m_basicVariableValues.at(index) -
                        (m_model.getVariable(index).getUpperBound() - m_model.getVariable(index).getLowerBound())
                        / alpha.at(index);
                breakpoints.push_back(currentBreakpoint);
            }
      //t<=0 case
        } else{

        }
    }
}
