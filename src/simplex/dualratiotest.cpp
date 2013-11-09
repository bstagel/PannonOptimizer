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

#include <utils/thirdparty/prettyprint.h>

static const double nonlinearDualPhaseIFunction =
        SimplexParameterHandler::getInstance().getParameterValue("nonlinear_dual_phaseI_function");
static const double nonlinearDualPhaseIIFunction =
        SimplexParameterHandler::getInstance().getParameterValue("nonlinear_dual_phaseII_function");

DualRatiotest::DualRatiotest(const SimplexModel & model,
                             const Vector& reducedCosts,
                             const IndexList<>& reducedCostFeasibilities,
                             const IndexList<const Numerical::Double*>& variableStates,
                             DualRatiotestUpdater &dualRatiotestUpdater  ) :
    m_model(model),
    m_reducedCosts(reducedCosts),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_variableStates(variableStates),
    m_dualRatiotestUpdater(dualRatiotestUpdater),
    m_incomingVariableIndex(-1),
    m_dualSteplength(0),
    m_primalSteplength(0),
    m_phaseIObjectiveValue(0),
    m_phaseIIObjectiveValue(0)
{
}

void DualRatiotest::shift(std::vector<BreakPoint>* breakpoints, unsigned int startId, unsigned int stopId) {
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
void DualRatiotest::getNextElement(std::vector<BreakPoint>* breakpoints, unsigned int length) {
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

void DualRatiotest::performRatiotestPhase1(unsigned int outgoingVariableIndex,
                                           const Vector& alpha,
                                           Numerical::Double phaseIReducedCost,
                                           Numerical::Double phaseIObjectiveValue)
                                           throw (NumericalException){

    #ifdef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif

    //LPINFO("PHASE1 RATIO TEST");
    //TODO: AZ updatevector szamitasa hibas.
    m_dualRatiotestUpdater.m_updateVector.clear();
    //BUG FIXED
//    m_dualRatiotestUpdater.m_updateVector.reserve(m_model.getMatrix().columnCount());
    m_dualRatiotestUpdater.m_updateVector.reserve(m_model.getMatrix().columnCount() + m_model.getMatrix().rowCount());

    Numerical::Double functionSlope = Numerical::fabs(phaseIReducedCost);
    m_phaseIObjectiveValue = phaseIObjectiveValue;

//determining t>=0 or t<=0 cases

        bool tPositive;
        if (phaseIReducedCost > 0) {
            tPositive = false;
        } else{
            tPositive = true;
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
        OBJECTIVE_TYPE objectiveType = m_model.getObjectiveType();

    //t>=0 case

        if (tPositive) {
        //computing ratios in M
            m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::MINUS);

            for (; it!=endit; it++){
                variableIndex = it.getData();
                if (alpha.at(variableIndex) < 0) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                    currentRatio.functionValue = 0;
                    breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                    if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                    }
                }
            }

        //computing ratios in P

            m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::PLUS);

            for (; it!=endit; it++){
                variableIndex = it.getData();
                if (alpha.at(variableIndex) > 0){
                    currentRatio.index = variableIndex;
                    currentRatio.value = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                    currentRatio.functionValue = 0;
                    breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                    if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                    }
                 }
            }

        //computing ratios in F

            m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);

            for (; it!=endit; it++){
                variableIndex = it.getData();
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();
                if ( alpha.at(variableIndex) != 0 ) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = m_reducedCosts.at(variableIndex) / alpha.at(variableIndex);
                    currentRatio.functionValue = 0;
                }

        //min problem

                if (objectiveType == MINIMIZE) {
                    if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) > 0) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                    } else
                    if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) < 0) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                    }

        //max problem

                } else {
                    if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) < 0) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                    } else
                    if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) > 0) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                    }
                }

        //both cases

                if (typeOfIthVariable == Variable::FREE && alpha.at(variableIndex) > 0) {
                    breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                } else
                if (typeOfIthVariable == Variable::FREE && alpha.at(variableIndex) < 0) {
                    breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                }
            }

    //t<=0 case
        } else{

        //computing ratios in M
            m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::MINUS);

                for (; it!=endit; it++) {
                    variableIndex = it.getData();
                    if (alpha.at(variableIndex) > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts.at(variableIndex) / alpha.at(variableIndex));
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                        if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                            breakpoints.push_back(currentRatio);
                            m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                        }
                    }
                }

        //computing ratios in P

                m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::PLUS);

                for (; it!=endit; it++) {
                    variableIndex = it.getData();
                    if (alpha.at(variableIndex) < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts.at(variableIndex) / alpha.at(variableIndex));
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                        if (m_model.getVariable(variableIndex).getType() == Variable::FREE) {
                            breakpoints.push_back(currentRatio);
                            m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                        }
                    }
                }

        //computing ratios in F

                m_reducedCostFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE);

                for (; it!=endit; it++) {
                    variableIndex = it.getData();
                    typeOfIthVariable = m_model.getVariable(variableIndex).getType();
                    if (alpha.at(variableIndex) != 0 ) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts.at(variableIndex) / alpha.at(variableIndex));
                        currentRatio.functionValue = 0;
                    }

        //min problem

                    if (objectiveType == MINIMIZE) {
                        if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) < 0) {
                            breakpoints.push_back(currentRatio);
                            m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                        } else
                        if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) > 0) {
                            breakpoints.push_back(currentRatio);
                            m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                        }

        //max problem

                    } else{
                        if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) > 0) {
                            breakpoints.push_back(currentRatio);
                            m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                        } else
                        if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) < 0) {
                            breakpoints.push_back(currentRatio);
                            m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                        }
                    }

        //both cases

                    if (typeOfIthVariable == Variable::FREE && alpha.at(variableIndex) < 0) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                    } else
                    if (typeOfIthVariable == Variable::FREE && alpha.at(variableIndex) > 0) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                    }
                }
            }

//dual steplength, incoming variable

    unsigned int iterationCounter = 0,length = breakpoints.size();

    //Init the heap with the breakpoint at 0
    getNextElement(&breakpoints,length);

    Numerical::Double valueOfOutgoingVariable;
    int num = (int)nonlinearDualPhaseIFunction;
    switch (num) {
  //using traditional one step method
      case 0:
        //TODO: KEZELNI KELL HA NINCS TORESPONT
        iterationCounter++;
        getNextElement(&breakpoints,length-1);
        m_phaseIObjectiveValue += functionSlope * (breakpoints[length-1].value -
                breakpoints[length].value);
        breakpoints[length-1].functionValue = m_phaseIObjectiveValue;
        //TODO: Ez itt nem length-1?
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
        m_dualSteplength = tPositive ? breakpoints[length-iterationCounter-1].value : - breakpoints[length-iterationCounter-1].value;
        m_incomingVariableIndex = breakpoints.at(length-1-iterationCounter).index;
        typeOfIthVariable = m_model.getVariable(outgoingVariableIndex).getType();

    //determining primal steplength

        valueOfOutgoingVariable = *(m_variableStates.getAttachedData(outgoingVariableIndex));

        if(m_incomingVariableIndex != -1){
            if (typeOfIthVariable == Variable::FIXED) {
                m_primalSteplength = (valueOfOutgoingVariable - m_model.getVariable(outgoingVariableIndex).getLowerBound()) /
                        alpha.at(m_incomingVariableIndex);
            }
            else if (typeOfIthVariable == Variable::BOUNDED) {
                m_primalSteplength = (valueOfOutgoingVariable - m_model.getVariable(outgoingVariableIndex).getLowerBound()) /
                        alpha.at(m_incomingVariableIndex);
            }
            else if (typeOfIthVariable == Variable::PLUS) {
                m_primalSteplength = (valueOfOutgoingVariable - m_model.getVariable(outgoingVariableIndex).getLowerBound()) /
                        alpha.at(m_incomingVariableIndex);
            }
            else if (typeOfIthVariable == Variable::FREE) {
                m_primalSteplength = valueOfOutgoingVariable / alpha.at(m_incomingVariableIndex);
            }
            else if (typeOfIthVariable == Variable::MINUS) {
                m_primalSteplength = (valueOfOutgoingVariable - m_model.getVariable(outgoingVariableIndex).getUpperBound()) /
                        alpha.at(m_incomingVariableIndex);
            }
        } else {
            m_primalSteplength = 0;
        }
        break;
    case 2:
    //case 1 begin
        while (functionSlope > 0 && iterationCounter < length-1) {
            iterationCounter++;
            getNextElement(&breakpoints,length-iterationCounter);

            m_phaseIObjectiveValue += functionSlope * (breakpoints.at(length-1-iterationCounter).value -
                    breakpoints[length-iterationCounter].value);
            breakpoints.at(length-1-iterationCounter).functionValue = m_phaseIObjectiveValue;
            functionSlope -= Numerical::fabs(alpha.at(breakpoints.at(length-1-iterationCounter).index));
        }
        m_dualSteplength = tPositive ? breakpoints[length-iterationCounter-1].value : - breakpoints[length-iterationCounter-1].value;
        m_incomingVariableIndex = breakpoints.at(length-1-iterationCounter).index;

        typeOfIthVariable = m_model.getVariable(outgoingVariableIndex).getType();
        valueOfOutgoingVariable = *(m_variableStates.getAttachedData(outgoingVariableIndex));

        if(m_incomingVariableIndex != -1){
            if (typeOfIthVariable == Variable::FIXED) {
                m_primalSteplength = (valueOfOutgoingVariable - m_model.getVariable(outgoingVariableIndex).getLowerBound()) /
                        alpha.at(m_incomingVariableIndex);
            }
            else if (typeOfIthVariable == Variable::BOUNDED) {
                m_primalSteplength = (valueOfOutgoingVariable - m_model.getVariable(outgoingVariableIndex).getLowerBound()) /
                        alpha.at(m_incomingVariableIndex);
            }
            else if (typeOfIthVariable == Variable::PLUS) {
                m_primalSteplength = (valueOfOutgoingVariable - m_model.getVariable(outgoingVariableIndex).getLowerBound()) /
                        alpha.at(m_incomingVariableIndex);
            }
            else if (typeOfIthVariable == Variable::FREE) {
                m_primalSteplength = valueOfOutgoingVariable / alpha.at(m_incomingVariableIndex);
            }
            else if (typeOfIthVariable == Variable::MINUS) {
                m_primalSteplength = (valueOfOutgoingVariable - m_model.getVariable(outgoingVariableIndex).getUpperBound()) /
                        alpha.at(m_incomingVariableIndex);
            }
        } else {
            m_primalSteplength = 0;
        }
    //case 1 end

    //numerical threshold
        unsigned int alphaId = length-1-iterationCounter;
        unsigned int prevAlphaId = alphaId, nextAlphaId = alphaId;
        unsigned int prevIterationCounter = 0, nextIterationCounter = 0;
        Numerical::Double prevObjValue = m_phaseIObjectiveValue, nextObjValue = m_phaseIObjectiveValue;
        Numerical::Double pivotThreshold = SimplexParameterHandler::getInstance().getParameterValue("e_pivot");

        if (Numerical::fabs(alpha.at(breakpoints.at(alphaId).index)) <= pivotThreshold) {
            LPWARNING("BAD NUMERICAL VALUE, ACTIVATING THRESHOLD for "<<alpha.at(breakpoints.at(alphaId).index));
            if (iterationCounter - prevIterationCounter > 0) {
                prevAlphaId++;
                prevIterationCounter++;
                prevObjValue=breakpoints[prevAlphaId].functionValue;
            }
            if (iterationCounter + nextIterationCounter < length-1) {
                nextAlphaId--;
                nextIterationCounter++;
                getNextElement(&breakpoints, length - (iterationCounter+nextIterationCounter));
                nextObjValue += functionSlope * (breakpoints[length-1-(iterationCounter + nextIterationCounter)].value -
                        breakpoints[length-(iterationCounter + nextIterationCounter)].value);
                breakpoints.at(nextAlphaId).functionValue = nextObjValue;
                functionSlope -= Numerical::fabs(alpha.at(breakpoints.at(nextAlphaId).index));
            }
        }

        bool prevIsBetter = nextObjValue > prevObjValue ? false : true;
        bool step = true;

        while (step && prevIsBetter ? (Numerical::fabs(alpha.at(breakpoints.at(prevAlphaId).index)) <= pivotThreshold) :
              (Numerical::fabs(alpha.at(breakpoints.at(nextAlphaId).index)) <= pivotThreshold)) {
            step = false;
            if (prevIsBetter) {
                if (iterationCounter - prevIterationCounter > 0) {
                    prevAlphaId++;
                    prevIterationCounter++;
                    prevObjValue=breakpoints[prevAlphaId].functionValue;
                    step = true;
                } else if ( prevObjValue != -Numerical::Infinity) {
                    prevObjValue = -Numerical::Infinity;
                    step = true;
                }
            } else {
                if (iterationCounter + nextIterationCounter < length -1 && nextObjValue != -Numerical::Infinity) {
                    nextAlphaId--;
                    nextIterationCounter++;
                    getNextElement(&breakpoints, length - (iterationCounter+nextIterationCounter));
                    nextObjValue += functionSlope * (breakpoints[length-1-(iterationCounter + nextIterationCounter)].value -
                            breakpoints[length-(iterationCounter + nextIterationCounter)].value);
                    functionSlope -= Numerical::fabs(alpha.at(breakpoints[nextAlphaId].index));
                    if (breakpoints[length-1].functionValue > nextObjValue) {
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
            m_incomingVariableIndex=breakpoints[alphaId].index;
            m_dualSteplength = breakpoints[alphaId].value;
        } else if (prevIsBetter) {
            m_incomingVariableIndex = breakpoints[prevAlphaId].index;
            m_dualSteplength = breakpoints[prevAlphaId].value;
        } else {
            m_incomingVariableIndex = breakpoints[nextAlphaId].index;
            m_dualSteplength = breakpoints[nextAlphaId].value;
        }
    break;
    }

}

void DualRatiotest::performRatiotestPhase2(unsigned int outgoingVariableIndex,
                                           const Vector& alpha,
                                           Numerical::Double objectiveFunction)
                                           throw (DualUnboundedException, NumericalException){
    #ifdef NDEBUG
    if (alpha.getType() == Vector::SPARSE_VECTOR) LPWARNING("Alpha is sparse vector!");
    #endif
    //LPINFO("PHASE2 RATIO TEST");
    std::vector<BreakPoint> breakpoints;
    Numerical::Double functionSlope = 0;
    Numerical::Double previousSlope = 0;
    bool transform = false, tPositive = false;
    m_phaseIIObjectiveValue = objectiveFunction;
    m_boundflips.clear();
    m_boundflips.reserve(alpha.nonZeros());
    breakpoints.reserve(alpha.nonZeros());
    BreakPoint currentRatio;
    currentRatio.index = -1;
    currentRatio.value = 0;
    currentRatio.functionValue = objectiveFunction;
    breakpoints.push_back(currentRatio);

//determining t>0 or t<0 cases

    if ( *(m_variableStates.getAttachedData(outgoingVariableIndex)) <
         m_model.getVariable(outgoingVariableIndex).getLowerBound()) {
        tPositive = true;
    } else if (*(m_variableStates.getAttachedData(outgoingVariableIndex)) >
               m_model.getVariable(outgoingVariableIndex).getUpperBound()) {
        tPositive = false;
    }

//computing ratios

    Variable::VARIABLE_TYPE typeOfIthVariable;
    IndexList<const Numerical::Double*>::Iterator it;
    IndexList<const Numerical::Double*>::Iterator endit;
    m_variableStates.getIterators(&it,&endit,0,Simplex::FEASIBILITY_ENUM_LENGTH);
    unsigned int variableIndex = it.getData();

    //t>=0 case

    if (tPositive) {
        while (!transform && it != endit) {
            variableIndex = it.getData();
            if (m_variableStates.where(variableIndex) != Simplex::BASIC) {
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();

        //free variables

                if (typeOfIthVariable == Variable::FREE && alpha.at(variableIndex) != 0) {
                    transform = true;
                    m_incomingVariableIndex = variableIndex;
                 } else{

        //bounded variables

                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_LB &&
                            typeOfIthVariable == Variable::BOUNDED && alpha.at(variableIndex) < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts.at(variableIndex) / alpha.at(variableIndex));
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    } else
                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_UB &&
                            typeOfIthVariable == Variable::BOUNDED && alpha.at(variableIndex) > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts.at(variableIndex) / alpha.at(variableIndex));
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    } else

        //plus type variables

                    if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts.at(variableIndex) / alpha.at(variableIndex));
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    } else

        //minus type variables

                    if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts.at(variableIndex) / alpha.at(variableIndex));
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    }
                 }
            }
            it++;
        }
        functionSlope = Numerical::fabs(*(m_variableStates.getAttachedData(outgoingVariableIndex)));

    //t<=0 case

    } else{
        while (!transform && it != endit) {
            variableIndex = it.getData();
            if (m_variableStates.where(variableIndex) != Simplex::BASIC) {
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();

        //free variables

                 if (typeOfIthVariable == Variable::FREE && alpha.at(variableIndex) != 0) {
                    transform = true;
                    m_incomingVariableIndex = variableIndex;
                 } else{

        //bounded variables

                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_LB &&
                            typeOfIthVariable == Variable::BOUNDED && alpha.at(variableIndex) > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts.at(variableIndex) / alpha.at(variableIndex));
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    } else
                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_UB &&
                            typeOfIthVariable == Variable::BOUNDED && alpha.at(variableIndex) < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts.at(variableIndex) / alpha.at(variableIndex));
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    } else

        //plus type variables

                    if (typeOfIthVariable == Variable::PLUS && alpha.at(variableIndex) > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts.at(variableIndex) / alpha.at(variableIndex));
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    } else

        //minus type variables

                    if (typeOfIthVariable == Variable::MINUS && alpha.at(variableIndex) < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts.at(variableIndex) / alpha.at(variableIndex));
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    }
                 }
            }
            it++;
        }
        functionSlope = *(m_variableStates.getAttachedData(outgoingVariableIndex)) -
                m_model.getVariable(outgoingVariableIndex).getUpperBound();
        }

//determining primal,dual steplength incoming variable

    int num =(int)nonlinearDualPhaseIIFunction;
    switch (num) {
    //using traditional one step method
      case 0:
        if (!transform) {
            unsigned int length = breakpoints.size();
            getNextElement(&breakpoints,length);
            if (functionSlope > 0) {
                getNextElement(&breakpoints,length-1);
                m_phaseIIObjectiveValue += functionSlope * (breakpoints[length-1].value-breakpoints[length].value);
                breakpoints[length-1].functionValue = m_phaseIIObjectiveValue;
                m_boundflips.push_back(breakpoints[length-1].index);
            }
            //TODO: A PRIMAL STEPLENGTH NEM JO VMIERT
            if (tPositive) {
                m_primalSteplength =- functionSlope / alpha.at(breakpoints[length-1].index);
            } else{
                m_primalSteplength = functionSlope / alpha.at(breakpoints[length-1].index);
            }
            m_incomingVariableIndex = breakpoints[length-1].index;
            m_dualSteplength = tPositive ? breakpoints[length-1].value : - breakpoints[length-1].value;
        }
        break;
    //using piecewise linear function
      case 1:
        if (!transform) {
//            LPWARNING("breakpoints: "<<breakpoints);
            unsigned int iterationCounter = 0, length = breakpoints.size(),id = 0;
            getNextElement(&breakpoints,length);
            while (!transform && functionSlope > 0 && iterationCounter < length-1) {
                iterationCounter++;
                getNextElement(&breakpoints,length-iterationCounter);
//                LPWARNING("breakpoints: "<<breakpoints);
                id = length-1-iterationCounter;
                m_phaseIIObjectiveValue += functionSlope * (breakpoints[id].value-breakpoints[id+1].value);
                breakpoints[id].functionValue = m_phaseIIObjectiveValue;
                previousSlope = functionSlope;

                const Variable & variable = m_model.getVariable(breakpoints[id].index);
                functionSlope -= Numerical::fabs(alpha.at(breakpoints[id].index)) *
                        Numerical::fabs(variable.getUpperBound() - variable.getLowerBound());

                if (tPositive) {
                    m_primalSteplength =- previousSlope / alpha.at(breakpoints[id].index);
                } else{
                    m_primalSteplength = previousSlope / alpha.at(breakpoints[id].index);
                }

                if(variable.getType() == Variable::BOUNDED){
                    if((variable.getUpperBound() - variable.getLowerBound()) < m_primalSteplength) {
                        m_boundflips.push_back(breakpoints[id].index);
                    } else{
                        transform = true;
                    }
                }

                if (functionSlope < 0) {
                    transform = true;
                }
            }

        m_incomingVariableIndex = breakpoints.at(length-1-iterationCounter).index;
        m_dualSteplength = tPositive ? breakpoints.at(length-1-iterationCounter).value : - breakpoints.at(length-1-iterationCounter).value;
        }
        break;
////numerical threshold
//    case 2:
//        unsigned int alphaId = length-1-iterationCounter;
//        unsigned int prevAlphaId = alphaId, nextAlphaId = alphaId;
//        unsigned int prevIterationCounter = 0, nextIterationCounter = 0;
//        Numerical::Double prevObjValue = m_phaseIObjectiveValue, nextObjValue = m_phaseIObjectiveValue;
//        Numerical::Double pivotThreshold = SimplexParameterHandler::getInstance().getParameterValue("pivot_threshold");

//        if (Numerical::fabs(alpha[breakpoints[alphaId].index]) <= pivotThreshold) {
//            LPWARNING("BAD NUMERICAL VALUE, ACTIVATING THRESHOLD for "<<alpha[breakpoints[alphaId].index]);
//            if (iterationCounter - prevIterationCounter > 0) {
//                prevAlphaId++;
//                prevIterationCounter++;
//                prevObjValue=breakpoints[prevAlphaId].functionValue;
//            }
//            if (iterationCounter + nextIterationCounter < length-1) {
//                nextAlphaId--;
//                nextIterationCounter++;
//                getNextElement(&breakpoints, length - (iterationCounter+nextIterationCounter));
//                nextObjValue += functionSlope * (breakpoints[length-1-(iterationCounter + nextIterationCounter)].value -
//                        breakpoints[length-(iterationCounter + nextIterationCounter)].value);
//                breakpoints[nextAlphaId].functionValue = nextObjValue;
//                functionSlope -= Numerical::fabs(alpha[breakpoints[nextAlphaId].index]) * (
//                                  Numerical::fabs( m_model.getVariable(breakpoints[nextAlphaId].index).getUpperBound() -
//                                          m_model.getVariable(breakpoints[nextAlphaId].index).getLowerBound() )
//                                   );
//            }
//        }

//        bool prevIsBetter = nextObjValue > prevObjValue ? false : true;
//        bool step = true;

//        while (step && prevIsBetter ? (Numerical::fabs(alpha[breakpoints[prevAlphaId].index]) <= pivotThreshold) :
//              (Numerical::fabs(alpha[breakpoints[nextAlphaId].index]) <= pivotThreshold)) {
//            step = false;
//            if (prevIsBetter) {
//                if (iterationCounter - prevIterationCounter > 0) {
//                    prevAlphaId++;
//                    prevIterationCounter++;
//                    prevObjValue=breakpoints[prevAlphaId].functionValue;
//                    step = true;
//                } else if ( prevObjValue != -Numerical::Infinity) {
//                    prevObjValue = -Numerical::Infinity;
//                    step = true;
//                }
//            } else {
//                if (iterationCounter + nextIterationCounter < length-1 && nextObjValue != -Numerical::Infinity) {
//                    nextAlphaId--;
//                    nextIterationCounter++;
//                    getNextElement(&breakpoints, length - (iterationCounter+nextIterationCounter));
//                    nextObjValue += functionSlope * (breakpoints[length-1-(iterationCounter + nextIterationCounter)].value -
//                            breakpoints[length-(iterationCounter + nextIterationCounter)].value);
//                    functionSlope -= Numerical::fabs(alpha[breakpoints[nextAlphaId].index]) * (
//                                        Numerical::fabs( m_model.getVariable(breakpoints[nextAlphaId].index).getUpperBound() -
//                                              m_model.getVariable(breakpoints[nextAlphaId].index).getLowerBound() )
//                                       );
//                    if (breakpoints[length-1].functionValue > nextObjValue) {
//                        nextObjValue = -Numerical::Infinity;
//                    }
//                    step = true;
//                } else if( nextObjValue != -Numerical::Infinity) {
//                    nextObjValue = -Numerical::Infinity;
//                    step = true;
//                }
//            }
//            prevIsBetter = nextObjValue > prevObjValue? false : true;
//        }

//        if ((prevObjValue == - Numerical::Infinity) && (nextObjValue == - Numerical::Infinity)) {
//            m_incomingVariableIndex=breakpoints[alphaId].index;
//            m_dualSteplength = breakpoints[alphaId].value;
//        } else if (prevIsBetter) {
//            m_incomingVariableIndex = breakpoints[prevAlphaId].index;
//            m_dualSteplength = breakpoints[prevAlphaId].value;
//        } else {
//            m_incomingVariableIndex = breakpoints[nextAlphaId].index;
//            m_dualSteplength = breakpoints[nextAlphaId].value;
//        }
//        break;
    }
}

DualRatiotest::~DualRatiotest() {

}



