#include <simplex/dualratiotest.h>
#include <lp/model.h>
#include <utility>
#include <simplex/simplex.h>

const static Numerical::Double pivotThreshold=0.00001;

DualRatiotest::DualRatiotest(const Model & m_model)
    :m_model(m_model),m_incomingVariableIndex(0),
      m_outgoingVariableIndex(0),m_dualSteplength(0),
      m_primalSteplength(0),m_objectiveFunctionPhase1(0),
      m_objectiveFunctionPhase2(0)
{
}

void DualRatiotest::shift(std::vector<BreakPoints>* breakpts, unsigned int startId, unsigned int stopId){
    unsigned int i=startId,  j=2*i+1;
    BreakPoints elementToShift=(*breakpts)[startId];

//choosing bigger son
    if(j<stopId && (*breakpts)[j+1].value > (*breakpts)[j].value) j++;
//shifting
    while(j<=stopId && (*breakpts)[j].value > elementToShift.value){
        (*breakpts)[i]=(*breakpts)[j];
        i=j;
        j=2*i+1;
        if(j<stopId && (*breakpts)[j+1].value > (*breakpts)[j].value ) j++;
    };
    (*breakpts)[i]=elementToShift;
}

//This function puts the smallest element to the last space considering the given length
void DualRatiotest::getNextElement(std::vector<BreakPoints>* breakpts, unsigned int length){
    BreakPoints temp;

//creating the heap
    if(length>=2){
        for(int i=(length-2)/2;i>=0;i--){
            shift(breakpts,i,length-1);
        }
    }

    temp=(*breakpts)[0];
    (*breakpts)[0]=(*breakpts)[length-1];
    (*breakpts)[length-1]=temp;
}

void DualRatiotest::performRatioTestPhase1(unsigned int outgoing,
                                            const Vector& alpha,
                                            const Vector& reducedCosts,
                                            const IndexList<>& reducedCostFeasibilities,
                                            const IndexList<Numerical::Double>& variableStates,
                                            const DualFeasibilityChecker& feasibilityChecker,
                                            Numerical::Double phaseIObjectiveValue){
    m_updateVector.clear();

    Numerical::Double phaseIreducedCost=feasibilityChecker.getPhaseIreducedCost(alpha);
    Numerical::Double functionSlope=Numerical::fabs(phaseIreducedCost);
    m_objectiveFunctionPhase1=phaseIObjectiveValue;
    m_outgoingVariableIndex=outgoing;

//determining t>=0 or t<=0 cases

        bool tPositive;
        if(phaseIreducedCost>0){
            tPositive=false;
        }else{
            tPositive=true;
        }

//computing ratios

        std::vector <BreakPoints> breakpoints;
        breakpoints.reserve(alpha.nonZeros()*2);
        m_updateVector.reserve(m_model.getMatrix().columnCount());
        BreakPoints currentRatio;
        currentRatio.functionValue=m_objectiveFunctionPhase1;
        currentRatio.index=0;
        currentRatio.value=0;
        breakpoints.push_back(currentRatio);
        IndexList<>::Iterator it;
        IndexList<>::Iterator endit;
        unsigned int variableIndex=0;
        Variable::VARIABLE_TYPE typeOfIthVariable;

    //t>=0 case

        if(tPositive){

        //computing ratios in M

            reducedCostFeasibilities.getIterators(&it,&endit,Simplex::MINUS,1);

            for(;it!=endit;it++){
                variableIndex=it.getData();
                if(alpha[variableIndex]<0){
                    currentRatio.index=variableIndex;
                    currentRatio.value=reducedCosts[variableIndex]/alpha[variableIndex];
                    currentRatio.functionValue=0;
                    breakpoints.push_back(currentRatio);
                    m_updateVector[variableIndex]++;
                    if(m_model.getVariable(variableIndex).getType()==Variable::FREE){
                        breakpoints.push_back(currentRatio);
                        m_updateVector[variableIndex]++;
                    }
                }
            }

        //computing ratios in P

            reducedCostFeasibilities.getIterators(&it,&endit,Simplex::PLUS,1);

            for(;it!=endit;it++){
                variableIndex=it.getData();
                if(alpha[variableIndex]>0){
                    currentRatio.index=variableIndex;
                    currentRatio.value=reducedCosts[variableIndex]/alpha[variableIndex];
                    currentRatio.functionValue=0;
                    breakpoints.push_back(currentRatio);
                    m_updateVector[variableIndex]--;
                    if(m_model.getVariable(variableIndex).getType()==Variable::FREE){
                        breakpoints.push_back(currentRatio);
                        m_updateVector[variableIndex]--;
                    }
                 }
            }

        //computing ratios in F

            reducedCostFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE,1);

            for(;it!=endit;it++){
                variableIndex=it.getData();
                typeOfIthVariable=m_model.getVariable(variableIndex).getType();
                currentRatio.index=variableIndex;
                currentRatio.value=reducedCosts[variableIndex]/alpha[variableIndex];
                currentRatio.functionValue=0;
                if(typeOfIthVariable==Variable::PLUS && alpha[variableIndex]>0){
                    breakpoints.push_back(currentRatio);
                    m_updateVector[variableIndex]--;
                }else
                if(typeOfIthVariable==Variable::MINUS && alpha[variableIndex]<0){
                    breakpoints.push_back(currentRatio);
                    m_updateVector[variableIndex]++;
                }else
                if(typeOfIthVariable==Variable::FREE && alpha[variableIndex]>0){
                    breakpoints.push_back(currentRatio);
                    m_updateVector[variableIndex]--;
                }else
                if(typeOfIthVariable==Variable::FREE && alpha[variableIndex]<0){
                    breakpoints.push_back(currentRatio);
                    m_updateVector[variableIndex]++;
                }
            }

    //t<=0 case

        }else{

        //computing ratios in M

            reducedCostFeasibilities.getIterators(&it,&endit,Simplex::MINUS,1);

                for(;it!=endit;it++){
                    variableIndex=it.getData();
                    if(alpha[variableIndex]>0){
                        currentRatio.index=variableIndex;
                        currentRatio.value=Numerical::fabs(reducedCosts[variableIndex]/alpha[variableIndex]);
                        currentRatio.functionValue=0;
                        breakpoints.push_back(currentRatio);
                        m_updateVector[variableIndex]++;
                        if(m_model.getVariable(variableIndex).getType()==Variable::FREE){
                            breakpoints.push_back(currentRatio);
                            m_updateVector[variableIndex]++;
                        }
                    }
                }

        //computing ratios in P

                reducedCostFeasibilities.getIterators(&it,&endit,Simplex::PLUS,1);

                for(;it!=endit;it++){
                    variableIndex=it.getData();
                    if(alpha[variableIndex]<0){
                        currentRatio.index=variableIndex;
                        currentRatio.value=Numerical::fabs(reducedCosts[variableIndex]/alpha[variableIndex]);
                        currentRatio.functionValue=0;
                        breakpoints.push_back(currentRatio);
                        m_updateVector[variableIndex]--;
                        if(m_model.getVariable(variableIndex).getType()==Variable::FREE){
                            breakpoints.push_back(currentRatio);
                            m_updateVector[variableIndex]--;
                        }
                    }
                }

        //computing ratios in F

                reducedCostFeasibilities.getIterators(&it,&endit,Simplex::FEASIBLE,1);

                for(;it!=endit;it++){
                    variableIndex=it.getData();
                    typeOfIthVariable=m_model.getVariable(variableIndex).getType();
                    currentRatio.index=variableIndex;
                    currentRatio.value=Numerical::fabs(reducedCosts[variableIndex]/alpha[variableIndex]);
                    currentRatio.functionValue=0;
                    if(typeOfIthVariable==Variable::PLUS && alpha[variableIndex]<0){
                        breakpoints.push_back(currentRatio);
                        m_updateVector[variableIndex]--;
                    }else
                    if(typeOfIthVariable==Variable::MINUS && alpha[variableIndex]>0){
                        breakpoints.push_back(currentRatio);
                        m_updateVector[variableIndex]++;
                    }else
                    if(typeOfIthVariable==Variable::FREE && alpha[variableIndex]<0){
                        breakpoints.push_back(currentRatio);
                        m_updateVector[variableIndex]--;
                    }else
                    if(typeOfIthVariable==Variable::FREE && alpha[variableIndex]>0){
                        breakpoints.push_back(currentRatio);
                        m_updateVector[variableIndex]++;
                    }
                }
            }

//dual steplength, incoming variable

    unsigned int iterationCounter=0,length=breakpoints.size();

    getNextElement(&breakpoints,length);
    while(functionSlope>0 && iterationCounter<length){
        iterationCounter++;
        getNextElement(&breakpoints,length-iterationCounter);
        m_objectiveFunctionPhase1+=functionSlope*(breakpoints[length-1-iterationCounter].value-breakpoints[length-iterationCounter].value);
        breakpoints[length-1-iterationCounter].functionValue=m_objectiveFunctionPhase1;
        functionSlope-=Numerical::fabs(alpha[breakpoints[length-1-iterationCounter].index]);
    }

    m_dualSteplength=tPositive ? breakpoints[length-iterationCounter-1].value : -breakpoints[length-iterationCounter-1].value;
    m_incomingVariableIndex=breakpoints[length-1-iterationCounter].index;
    typeOfIthVariable=m_model.getVariable(m_outgoingVariableIndex).getType();

//determining primal steplength

    Numerical::Double valueOfOutgoingVariable=variableStates.getAttachedData(m_outgoingVariableIndex);
    if(typeOfIthVariable==Variable::FIXED){
        m_primalSteplength=(valueOfOutgoingVariable-m_model.getVariable(m_outgoingVariableIndex).getLowerBound())/alpha[m_outgoingVariableIndex];
    }
    else if(typeOfIthVariable==Variable::BOUNDED){
        m_primalSteplength=(valueOfOutgoingVariable-m_model.getVariable(m_outgoingVariableIndex).getLowerBound())/alpha[m_outgoingVariableIndex];
    }
    else if(typeOfIthVariable==Variable::PLUS){
        m_primalSteplength=(valueOfOutgoingVariable-m_model.getVariable(m_outgoingVariableIndex).getLowerBound())/alpha[m_outgoingVariableIndex];
    }
    else if(typeOfIthVariable==Variable::FREE){
        m_primalSteplength=valueOfOutgoingVariable/alpha[m_outgoingVariableIndex];
    }
    else if(typeOfIthVariable==Variable::MINUS){
        m_primalSteplength=(valueOfOutgoingVariable-m_model.getVariable(m_outgoingVariableIndex).getUpperBound())/alpha[m_outgoingVariableIndex];
    }

//numerical threshold

    unsigned int alphaId=length-1-iterationCounter;
    unsigned int prevAlphaId = alphaId, nextAlphaId = alphaId;
    unsigned int prevIterationCounter = 0, nextIterationCounter = 0;
    Numerical::Double prevObjValue=m_objectiveFunctionPhase1, nextObjValue=m_objectiveFunctionPhase1;

    if(alpha[breakpoints[alphaId].index]<=pivotThreshold){
        if(iterationCounter - prevIterationCounter > 0){
            prevAlphaId++;
            prevIterationCounter++;
            prevObjValue=breakpoints[prevAlphaId].functionValue;
        }
        if(iterationCounter + nextIterationCounter < length){
            nextAlphaId--;
            nextIterationCounter++;
            getNextElement(&breakpoints, length - (iterationCounter+nextIterationCounter));
            nextObjValue += functionSlope*(breakpoints[length-1-(iterationCounter + nextIterationCounter)].value -
                    breakpoints[length-(iterationCounter + nextIterationCounter)].value);
            breakpoints[nextAlphaId].functionValue = nextObjValue;
            functionSlope-=Numerical::fabs(alpha[breakpoints[nextAlphaId].index]);
        }
    }

    bool prevIsBetter = nextObjValue>prevObjValue? false : true;
    bool step = true;

    while(step && prevIsBetter ? (alpha[breakpoints[prevAlphaId].index]<=pivotThreshold) : (alpha[breakpoints[nextAlphaId].index]<=pivotThreshold)){
        step = false;
        if(prevIsBetter){
            if(iterationCounter - prevIterationCounter > 0){
                prevAlphaId++;
                prevIterationCounter++;
                prevObjValue=breakpoints[prevAlphaId].functionValue;
                step = true;
            } else if( prevObjValue != -Numerical::Infinity){
                prevObjValue = -Numerical::Infinity;
                step = true;
            }
        } else {
            if(iterationCounter + nextIterationCounter < length && nextObjValue != -Numerical::Infinity){
                nextAlphaId--;
                nextIterationCounter++;
                getNextElement(&breakpoints, length - (iterationCounter+nextIterationCounter));
                nextObjValue += functionSlope*(breakpoints[length-1-(iterationCounter + nextIterationCounter)].value -
                        breakpoints[length-(iterationCounter + nextIterationCounter)].value);
                if(breakpoints[length-1].functionValue > nextObjValue){
                    nextObjValue = -Numerical::Infinity;
                }
                step = true;
            } else if( nextObjValue != -Numerical::Infinity){
                nextObjValue = -Numerical::Infinity;
                step = true;
            }
        }
        prevIsBetter = nextObjValue>prevObjValue? false : true;
    }

    if((prevObjValue == Numerical::Infinity) && (nextObjValue == Numerical::Infinity)){
        m_incomingVariableIndex=breakpoints[alphaId].index;
        m_dualSteplength=breakpoints[alphaId].value;
    } else if(prevIsBetter){
        m_incomingVariableIndex=breakpoints[prevAlphaId].index;
        m_dualSteplength=breakpoints[prevAlphaId].value;
    } else {
        m_incomingVariableIndex=breakpoints[nextAlphaId].index;
        m_dualSteplength=breakpoints[nextAlphaId].value;
    }
}

void DualRatiotest::performRatiotestPhase2(unsigned int outgoing,
                                            const Vector& alpha,
                                            const Vector& reducedCosts,
                                            Numerical::Double objVal,
                                            const IndexList<Numerical::Double>& variableStates){
    std::vector<BreakPoints> breakpoints;
    Numerical::Double functionSlope;
    Numerical::Double previousSlope=0;
    bool transform=false, tPositive=false;
    m_outgoingVariableIndex=outgoing;
    m_objectiveFunctionPhase2=objVal;
    m_boundflips.clear();
    m_boundflips.reserve(alpha.nonZeros());
    breakpoints.reserve(alpha.nonZeros());
    BreakPoints currentRatio;
    currentRatio.index=0;
    currentRatio.value=0;
    currentRatio.functionValue=objVal;
    breakpoints.push_back(currentRatio);

//determining t>0 or t<0 cases

    if( variableStates.getAttachedData(m_outgoingVariableIndex) < m_model.getVariable(m_outgoingVariableIndex).getLowerBound()){
        tPositive=true;
    }else if(variableStates.getAttachedData(m_outgoingVariableIndex) > m_model.getVariable(m_outgoingVariableIndex).getUpperBound()){
        tPositive=false;
    }

//computing ratios

    Variable::VARIABLE_TYPE typeOfIthVariable;
    IndexList<Numerical::Double>::Iterator it;
    IndexList<Numerical::Double>::Iterator endit;
    variableStates.getIterators(&it,&endit,Simplex::BASIC,Simplex::FEASIBILITY_ENUM_LENGTH);
    unsigned int variableIndex=it.getData();

    //t>=0 case

    if(tPositive){
        while(!transform && it!=endit){
            variableIndex=it.getData();
            if(variableStates.where(variableIndex) != Simplex::BASIC){
                typeOfIthVariable=m_model.getVariable(variableIndex).getType();

        //free variables

                if(typeOfIthVariable==Variable::FREE && alpha[variableIndex]!=0){
                    transform=true;
                    m_incomingVariableIndex=variableIndex;
                 }else{

        //bounded variables

                    if(variableStates.where(variableIndex)==Simplex::NONBASIC_AT_LB &&
                            typeOfIthVariable==Variable::BOUNDED && alpha[variableIndex]<0){
                        currentRatio.index=variableIndex;
                        currentRatio.value=Numerical::fabs(reducedCosts[variableIndex]/alpha[variableIndex]);
                        currentRatio.functionValue=0;
                        breakpoints.push_back(currentRatio);
                    }else
                    if(variableStates.where(variableIndex)==Simplex::NONBASIC_AT_UB &&
                            typeOfIthVariable==Variable::BOUNDED && alpha[variableIndex]>0){
                        currentRatio.index=variableIndex;
                        currentRatio.value=Numerical::fabs(reducedCosts[variableIndex]/alpha[variableIndex]);
                        currentRatio.functionValue=0;
                        breakpoints.push_back(currentRatio);
                    }else

        //plus type variables

                    if(typeOfIthVariable==Variable::PLUS && alpha[variableIndex]<0){
                        currentRatio.index=variableIndex;
                        currentRatio.value=Numerical::fabs(reducedCosts[variableIndex]/alpha[variableIndex]);
                        currentRatio.functionValue=0;
                        breakpoints.push_back(currentRatio);
                    }else

        //minus type variables

                    if(typeOfIthVariable==Variable::MINUS && alpha[variableIndex]>0){
                        currentRatio.index=variableIndex;
                        currentRatio.value=Numerical::fabs(reducedCosts[variableIndex]/alpha[variableIndex]);
                        currentRatio.functionValue=0;
                        breakpoints.push_back(currentRatio);
                    }
                 }
            }
            it++;
        }
        functionSlope=Numerical::fabs(variableStates.getAttachedData(m_outgoingVariableIndex));

    //t<=0 case

    }else{
        while(!transform && it!=endit){
            variableIndex=it.getData();
            if(variableStates.where(variableIndex) != Simplex::BASIC){
                typeOfIthVariable=m_model.getVariable(variableIndex).getType();

        //free variables

                 if(typeOfIthVariable==Variable::FREE && alpha[variableIndex]!=0){
                    transform=true;
                    m_incomingVariableIndex=variableIndex;
                 }else{

        //bounded variables

                    if(variableStates.where(variableIndex)==Simplex::NONBASIC_AT_LB &&
                            typeOfIthVariable==Variable::BOUNDED && alpha[variableIndex]>0){
                        currentRatio.index=variableIndex;
                        currentRatio.value=Numerical::fabs(reducedCosts[variableIndex]/alpha[variableIndex]);
                        currentRatio.functionValue=0;
                        breakpoints.push_back(currentRatio);
                    }else
                    if(variableStates.where(variableIndex)==Simplex::NONBASIC_AT_UB &&
                            typeOfIthVariable==Variable::BOUNDED && alpha[variableIndex]<0){
                        currentRatio.index=variableIndex;
                        currentRatio.value=Numerical::fabs(reducedCosts[variableIndex]/alpha[variableIndex]);
                        currentRatio.functionValue=0;
                        breakpoints.push_back(currentRatio);
                    }else

        //plus type variables

                    if(typeOfIthVariable==Variable::PLUS && alpha[variableIndex]>0){
                        currentRatio.index=variableIndex;
                        currentRatio.value=Numerical::fabs(reducedCosts[variableIndex]/alpha[variableIndex]);
                        currentRatio.functionValue=0;
                        breakpoints.push_back(currentRatio);
                    }else

        //minus type variables

                    if(typeOfIthVariable==Variable::MINUS && alpha[variableIndex]<0){
                        currentRatio.index=variableIndex;
                        currentRatio.value=Numerical::fabs(reducedCosts[variableIndex]/alpha[variableIndex]);
                        currentRatio.functionValue=0;
                        breakpoints.push_back(currentRatio);
                    }
                 }
            }
            it++;
        }
        functionSlope=variableStates.getAttachedData(m_outgoingVariableIndex)-m_model.getVariable(m_outgoingVariableIndex).getUpperBound();
        }

//determining primal,dual steplength incoming variable

    if(!transform){
        unsigned int iterationCounter=0, length=breakpoints.size(),id=0;
        getNextElement(&breakpoints,length);
        while(!transform && functionSlope>0 && iterationCounter<length){
            iterationCounter++;
            getNextElement(&breakpoints,length-iterationCounter);
            id=length-1-iterationCounter;
            m_objectiveFunctionPhase2+=functionSlope*(breakpoints[id].value-breakpoints[id+1].value);
            breakpoints[id].functionValue=m_objectiveFunctionPhase2;
            previousSlope=functionSlope;
            functionSlope-=Numerical::fabs(alpha[breakpoints[id].index])*Numerical::fabs(m_model.getVariable(breakpoints[id].index).getUpperBound()-
                m_model.getVariable(breakpoints[id].index).getLowerBound());
            m_boundflips.push_back(breakpoints[length-1-iterationCounter].index);
            if(Numerical::fabs(functionSlope)==Numerical::Infinity){
                transform=true;
            }
        };
        if(tPositive){
            m_primalSteplength=-previousSlope/alpha[breakpoints[length-1-iterationCounter].index];
        }else{
            m_primalSteplength=previousSlope/alpha[breakpoints[length-1-iterationCounter].index];
        }
        m_incomingVariableIndex=breakpoints[length-1-iterationCounter].index;
        m_dualSteplength=tPositive ? breakpoints[length-1-iterationCounter].value : -breakpoints[length-1-iterationCounter].value;

//numerical threshold

        unsigned int alphaId=length-1-iterationCounter;
        Numerical::Double prevObjValue=0,nextObjValue=0;

        while(alpha[breakpoints[alphaId].index]<=pivotThreshold){
            alphaId++;

    //if there are further breakpoints

            if(iterationCounter<length){
                prevObjValue=breakpoints[alphaId].functionValue;
                iterationCounter++;
                getNextElement(&breakpoints,length-iterationCounter);
                nextObjValue=m_objectiveFunctionPhase2+functionSlope*(breakpoints[alphaId-2].value-breakpoints[alphaId-1].value);
                m_objectiveFunctionPhase2=nextObjValue>prevObjValue ? nextObjValue : prevObjValue;

    //is the next one better or the previous one?

                if(nextObjValue>prevObjValue){
                    alphaId+=2;
                    breakpoints[alphaId].functionValue=nextObjValue;
                    functionSlope-=Numerical::fabs(alpha[breakpoints[alphaId].index]);
                }
            }
            m_incomingVariableIndex=breakpoints[alphaId].index;
            m_dualSteplength=breakpoints[alphaId].value;
        }
    }
}
DualRatiotest::~DualRatiotest(){
}
