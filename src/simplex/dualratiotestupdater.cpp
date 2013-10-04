#include "simplex/dualratiotestupdater.h"
#include "simplex/simplex.h"

DualRatiotestUpdater::DualRatiotestUpdater(IndexList<>* reducedCostFeasibilities,IndexList<const Numerical::Double*>* variableStates):
    m_reducedcostFeasibilities(reducedCostFeasibilities),
    m_variableStates(variableStates)
{
}
void DualRatiotestUpdater::updateFeasibilities(const std::vector<int>& updateVector){

    //TODO: updateVector not valid?!!
    for(unsigned int index=0;index<updateVector.size();index++){
        if(updateVector[index]==1){
            if(m_reducedcostFeasibilities->where(index)==Simplex::MINUS){
                m_reducedcostFeasibilities->move(index,Simplex::FEASIBLE);
            }else
            if(m_reducedcostFeasibilities->where(index)==Simplex::FEASIBLE){
                m_reducedcostFeasibilities->move(index,Simplex::PLUS);
            }
        }else
        if(updateVector[index]==-1){
            if(m_reducedcostFeasibilities->where(index)==Simplex::PLUS){
                m_reducedcostFeasibilities->move(index,Simplex::FEASIBLE);
            }else
            if(m_reducedcostFeasibilities->where(index)==Simplex::FEASIBLE){
                m_reducedcostFeasibilities->move(index,Simplex::MINUS);
            }
        }else
        if(updateVector[index]==2){
            if(m_reducedcostFeasibilities->where(index)==Simplex::MINUS){
                m_reducedcostFeasibilities->move(index,Simplex::PLUS);
            }
        }else
        if(updateVector[index]==-2){
            if(m_reducedcostFeasibilities->where(index)==Simplex::PLUS){
                m_reducedcostFeasibilities->move(index,Simplex::MINUS);
            }
        }
    }

}
