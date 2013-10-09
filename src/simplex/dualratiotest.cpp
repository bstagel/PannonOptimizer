/**
 * @file dualratiotest.cpp
 */

#include <simplex/dualratiotest.h>
#include <lp/model.h>
#include <utility>
#include <simplex/simplex.h>
#include <globals.h>

static const Numerical::Double pivotThreshold=0.00001;

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
    m_incomingVariableIndex(0),
    m_outgoingVariableIndex(0),
    m_dualSteplength(0),
    m_primalSteplength(0),
    m_objectiveFunctionPhase1(0),
    m_objectiveFunctionPhase2(0)
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

    temp = (*breakpoints)[0];
    (*breakpoints)[0] = (*breakpoints)[length-1];
    (*breakpoints)[length-1] = temp;
}

void DualRatiotest::performRatiotestPhase1(unsigned int outgoing,
                                           const Vector& alpha,
                                           Numerical::Double phaseIReducedCost,
                                           Numerical::Double phaseIObjectiveValue) {

    m_dualRatiotestUpdater.m_updateVector.reserve(m_model.getMatrix().columnCount());

    Numerical::Double functionSlope = Numerical::fabs(phaseIReducedCost);
    m_objectiveFunctionPhase1 = phaseIObjectiveValue;
    m_outgoingVariableIndex = outgoing;

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
        currentRatio.functionValue = m_objectiveFunctionPhase1;
        currentRatio.index = 0;
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
                if (alpha[variableIndex] < 0) {
                    currentRatio.index = variableIndex;
                    currentRatio.value = m_reducedCosts[variableIndex] / alpha[variableIndex];
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
                if (alpha[variableIndex] > 0){
                    currentRatio.index = variableIndex;
                    currentRatio.value = m_reducedCosts[variableIndex] / alpha[variableIndex];
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
                currentRatio.index = variableIndex;
                currentRatio.value = m_reducedCosts[variableIndex] / alpha[variableIndex];
                currentRatio.functionValue = 0;

        //min problem

                if (objectiveType == MINIMIZE) {
                    if (typeOfIthVariable == Variable::PLUS && alpha[variableIndex] > 0) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                    } else
                    if (typeOfIthVariable == Variable::MINUS && alpha[variableIndex] < 0) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                    }

        //max problem

                } else {
                    if (typeOfIthVariable == Variable::PLUS && alpha[variableIndex] < 0) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                    } else
                    if (typeOfIthVariable == Variable::MINUS && alpha[variableIndex] > 0) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                    }
                }

        //both cases

                if (typeOfIthVariable == Variable::FREE && alpha[variableIndex] > 0) {
                    breakpoints.push_back(currentRatio);
                    m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                } else
                if (typeOfIthVariable == Variable::FREE && alpha[variableIndex] < 0) {
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
                    if (alpha[variableIndex] > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts[variableIndex] / alpha[variableIndex]);
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
                    if (alpha[variableIndex] < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts[variableIndex] / alpha[variableIndex]);
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
                    currentRatio.index = variableIndex;
                    currentRatio.value = Numerical::fabs(m_reducedCosts[variableIndex] / alpha[variableIndex]);
                    currentRatio.functionValue = 0;

        //min problem

                    if (objectiveType == MINIMIZE) {
                        if (typeOfIthVariable == Variable::PLUS && alpha[variableIndex] < 0) {
                            breakpoints.push_back(currentRatio);
                            m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                        } else
                        if (typeOfIthVariable == Variable::MINUS && alpha[variableIndex] > 0) {
                            breakpoints.push_back(currentRatio);
                            m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                        }

        //max problem

                    } else{
                        if (typeOfIthVariable == Variable::PLUS && alpha[variableIndex] > 0) {
                            breakpoints.push_back(currentRatio);
                            m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                        } else
                        if (typeOfIthVariable == Variable::MINUS && alpha[variableIndex] < 0) {
                            breakpoints.push_back(currentRatio);
                            m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                        }
                    }

        //both cases

                    if (typeOfIthVariable == Variable::FREE && alpha[variableIndex] < 0) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]--;
                    } else
                    if (typeOfIthVariable == Variable::FREE && alpha[variableIndex] > 0) {
                        breakpoints.push_back(currentRatio);
                        m_dualRatiotestUpdater.m_updateVector[variableIndex]++;
                    }
                }
            }

//dual steplength, incoming variable

    unsigned int iterationCounter = 0,length = breakpoints.size();

    getNextElement(&breakpoints,length);
    while (functionSlope > 0 && iterationCounter < length) {
        iterationCounter++;
        getNextElement(&breakpoints,length-iterationCounter);
        m_objectiveFunctionPhase1 += functionSlope * (breakpoints[length-1-iterationCounter].value -
                breakpoints[length-iterationCounter].value);
        breakpoints[length-1-iterationCounter].functionValue = m_objectiveFunctionPhase1;
        functionSlope -= Numerical::fabs(alpha[breakpoints[length-1-iterationCounter].index]);
    }

    m_dualSteplength = tPositive ? breakpoints[length-iterationCounter-1].value : - breakpoints[length-iterationCounter-1].value;
    m_incomingVariableIndex = breakpoints[length-1-iterationCounter].index;
    typeOfIthVariable = m_model.getVariable(m_outgoingVariableIndex).getType();

//determining primal steplength
    Numerical::Double valueOfOutgoingVariable = *(m_variableStates.getAttachedData(m_outgoingVariableIndex));
    if (typeOfIthVariable == Variable::FIXED) {
        m_primalSteplength = (valueOfOutgoingVariable - m_model.getVariable(m_outgoingVariableIndex).getLowerBound()) /
                alpha[m_incomingVariableIndex];
    }
    else if (typeOfIthVariable == Variable::BOUNDED) {
        m_primalSteplength = (valueOfOutgoingVariable - m_model.getVariable(m_outgoingVariableIndex).getLowerBound()) /
                alpha[m_incomingVariableIndex];
    }
    else if (typeOfIthVariable == Variable::PLUS) {
        m_primalSteplength = (valueOfOutgoingVariable - m_model.getVariable(m_outgoingVariableIndex).getLowerBound()) /
                alpha[m_incomingVariableIndex];
    }
    else if (typeOfIthVariable == Variable::FREE) {
        m_primalSteplength = valueOfOutgoingVariable / alpha[m_incomingVariableIndex];
    }
    else if (typeOfIthVariable == Variable::MINUS) {
        m_primalSteplength = (valueOfOutgoingVariable - m_model.getVariable(m_outgoingVariableIndex).getUpperBound()) /
                alpha[m_incomingVariableIndex];
    }

//numerical threshold

    unsigned int alphaId = length-1-iterationCounter;
    unsigned int prevAlphaId = alphaId, nextAlphaId = alphaId;
    unsigned int prevIterationCounter = 0, nextIterationCounter = 0;
    Numerical::Double prevObjValue = m_objectiveFunctionPhase1, nextObjValue = m_objectiveFunctionPhase1;

    if (alpha[breakpoints[alphaId].index] <= pivotThreshold) {
        if (iterationCounter - prevIterationCounter > 0) {
            prevAlphaId++;
            prevIterationCounter++;
            prevObjValue=breakpoints[prevAlphaId].functionValue;
        }
        if (iterationCounter + nextIterationCounter < length) {
            nextAlphaId--;
            nextIterationCounter++;
            getNextElement(&breakpoints, length - (iterationCounter+nextIterationCounter));
            nextObjValue += functionSlope * (breakpoints[length-1-(iterationCounter + nextIterationCounter)].value -
                    breakpoints[length-(iterationCounter + nextIterationCounter)].value);
            breakpoints[nextAlphaId].functionValue = nextObjValue;
            functionSlope -= Numerical::fabs(alpha[breakpoints[nextAlphaId].index]);
        }
    }

    bool prevIsBetter = nextObjValue > prevObjValue ? false : true;
    bool step = true;

    while (step && prevIsBetter ? (alpha[breakpoints[prevAlphaId].index] <= pivotThreshold) :
          (alpha[breakpoints[nextAlphaId].index] <= pivotThreshold)) {
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
            if (iterationCounter + nextIterationCounter < length && nextObjValue != -Numerical::Infinity) {
                nextAlphaId--;
                nextIterationCounter++;
                getNextElement(&breakpoints, length - (iterationCounter+nextIterationCounter));
                nextObjValue += functionSlope * (breakpoints[length-1-(iterationCounter + nextIterationCounter)].value -
                        breakpoints[length-(iterationCounter + nextIterationCounter)].value);
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

    if ((prevObjValue == Numerical::Infinity) && (nextObjValue == Numerical::Infinity)) {
        m_incomingVariableIndex=breakpoints[alphaId].index;
        m_dualSteplength = breakpoints[alphaId].value;
    } else if (prevIsBetter) {
        m_incomingVariableIndex = breakpoints[prevAlphaId].index;
        m_dualSteplength = breakpoints[prevAlphaId].value;
    } else {
        m_incomingVariableIndex = breakpoints[nextAlphaId].index;
        m_dualSteplength = breakpoints[nextAlphaId].value;
    }
}

void DualRatiotest::performRatiotestPhase2(unsigned int outgoing,
                                            const Vector& alpha,
                                            Numerical::Double objectiveFunction){
    std::vector<BreakPoint> breakpoints;
    Numerical::Double functionSlope = 0;
    Numerical::Double previousSlope = 0;
    bool transform = false, tPositive = false;
    m_outgoingVariableIndex = outgoing;
    m_objectiveFunctionPhase2 = objectiveFunction;
    m_boundflips.clear();
    m_boundflips.reserve(alpha.nonZeros());
    breakpoints.reserve(alpha.nonZeros());
    BreakPoint currentRatio;
    currentRatio.index = 0;
    currentRatio.value = 0;
    currentRatio.functionValue = objectiveFunction;
    breakpoints.push_back(currentRatio);

//determining t>0 or t<0 cases

    if ( *(m_variableStates.getAttachedData(m_outgoingVariableIndex)) <
         m_model.getVariable(m_outgoingVariableIndex).getLowerBound()) {
        tPositive = true;
    } else if (*(m_variableStates.getAttachedData(m_outgoingVariableIndex)) >
               m_model.getVariable(m_outgoingVariableIndex).getUpperBound()) {
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

                if (typeOfIthVariable == Variable::FREE && alpha[variableIndex] != 0) {
                    transform = true;
                    m_incomingVariableIndex = variableIndex;
                 } else{

        //bounded variables

                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_LB &&
                            typeOfIthVariable == Variable::BOUNDED && alpha[variableIndex] < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts[variableIndex] / alpha[variableIndex]);
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    } else
                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_UB &&
                            typeOfIthVariable == Variable::BOUNDED && alpha[variableIndex] > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts[variableIndex] / alpha[variableIndex]);
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    } else

        //plus type variables

                    if (typeOfIthVariable == Variable::PLUS && alpha[variableIndex] < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts[variableIndex] / alpha[variableIndex]);
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    } else

        //minus type variables

                    if (typeOfIthVariable == Variable::MINUS && alpha[variableIndex] > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts[variableIndex] / alpha[variableIndex]);
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    }
                 }
            }
            it++;
        }
        functionSlope = Numerical::fabs(*(m_variableStates.getAttachedData(m_outgoingVariableIndex)));

    //t<=0 case

    } else{
        while (!transform && it != endit) {
            variableIndex = it.getData();
            if (m_variableStates.where(variableIndex) != Simplex::BASIC) {
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();

        //free variables

                 if (typeOfIthVariable == Variable::FREE && alpha[variableIndex] != 0) {
                    transform = true;
                    m_incomingVariableIndex = variableIndex;
                 } else{

        //bounded variables

                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_LB &&
                            typeOfIthVariable == Variable::BOUNDED && alpha[variableIndex] > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts[variableIndex] / alpha[variableIndex]);
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    } else
                    if (m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_UB &&
                            typeOfIthVariable == Variable::BOUNDED && alpha[variableIndex] < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts[variableIndex] / alpha[variableIndex]);
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    } else

        //plus type variables

                    if (typeOfIthVariable == Variable::PLUS && alpha[variableIndex] > 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts[variableIndex] / alpha[variableIndex]);
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    } else

        //minus type variables

                    if (typeOfIthVariable == Variable::MINUS && alpha[variableIndex] < 0) {
                        currentRatio.index = variableIndex;
                        currentRatio.value = Numerical::fabs(m_reducedCosts[variableIndex] / alpha[variableIndex]);
                        currentRatio.functionValue = 0;
                        breakpoints.push_back(currentRatio);
                    }
                 }
            }
            it++;
        }
        functionSlope = *(m_variableStates.getAttachedData(m_outgoingVariableIndex)) -
                m_model.getVariable(m_outgoingVariableIndex).getUpperBound();
        }

//determining primal,dual steplength incoming variable

    if (!transform) {
        unsigned int iterationCounter = 0, length = breakpoints.size(),id = 0;
        getNextElement(&breakpoints,length);
        while (!transform && functionSlope > 0 && iterationCounter < length) {
            iterationCounter++;
            getNextElement(&breakpoints,length-iterationCounter);
            id = length-1-iterationCounter;
            m_objectiveFunctionPhase2 += functionSlope * (breakpoints[id].value-breakpoints[id+1].value);
            breakpoints[id].functionValue = m_objectiveFunctionPhase2;
            previousSlope = functionSlope;
            functionSlope -= Numerical::fabs(alpha[breakpoints[id].index]) *
                    Numerical::fabs(m_model.getVariable(breakpoints[id].index).getUpperBound()-
                m_model.getVariable(breakpoints[id].index).getLowerBound());
            m_boundflips.push_back(breakpoints[length-1-iterationCounter].index);
            if (Numerical::fabs(functionSlope) == Numerical::Infinity) {
                transform = true;
            }
        };
        if (tPositive) {
            m_primalSteplength =- previousSlope / alpha[breakpoints[length-1-iterationCounter].index];
        } else{
            m_primalSteplength = previousSlope / alpha[breakpoints[length-1-iterationCounter].index];
        }
        m_incomingVariableIndex = breakpoints[length-1-iterationCounter].index;
        m_dualSteplength = tPositive ? breakpoints[length-1-iterationCounter].value : - breakpoints[length-1-iterationCounter].value;

////numerical threshold

//        unsigned int alphaId = length-1-iterationCounter;
//        Numerical::Double prevObjValue = 0,nextObjValue = 0;

//        while (alpha[breakpoints[alphaId].index] <= pivotThreshold) {
//            alphaId++;

//    //if there are further breakpoints

//            if (iterationCounter < length) {
//                prevObjValue = breakpoints[alphaId].functionValue;
//                iterationCounter++;
//                getNextElement(&breakpoints,length-iterationCounter);
//                nextObjValue = m_objectiveFunctionPhase2 + functionSlope *
//                        (breakpoints[alphaId-2].value - breakpoints[alphaId-1].value);
//                m_objectiveFunctionPhase2 = nextObjValue > prevObjValue ? nextObjValue : prevObjValue;

//    //is the next one better or the previous one?

//                if (nextObjValue > prevObjValue) {
//                    alphaId += 2;
//                    breakpoints[alphaId].functionValue = nextObjValue;
//                    functionSlope -= Numerical::fabs(alpha[breakpoints[alphaId].index]);
//                }
//            }
//            m_incomingVariableIndex = breakpoints[alphaId].index;
//            m_dualSteplength = breakpoints[alphaId].value;
//        }
    }
}

DualRatiotest::~DualRatiotest(){
}



