#include "simplex/dualratiotestupdater.h"
#include "simplex/simplex.h"

DualRatiotestUpdater::DualRatiotestUpdater(IndexList<>* redcostF,IndexList<Numerical::Double>* varS):
    m_reducedcostFeasibilities(redcostF),
    m_variableStates(varS)
{
}
void DualRatiotestUpdater::updateFeasibilities(const std::vector<int>& updateVector){

    //TODO: updateVector not valid?!!
    for(unsigned int index=0;index<updateVector.size();index++){
        if(updateVector[index]==1){
            if(m_reducedcostFeasibilities->getPartitionCount()==Simplex::MINUS){
                m_reducedcostFeasibilities->move(index,Simplex::FEASIBLE);
            }else
            if(m_reducedcostFeasibilities->getPartitionCount()==Simplex::FEASIBLE){
                m_reducedcostFeasibilities->move(index,Simplex::PLUS);
            }
        }else
        if(updateVector[index]==-1){
            if(m_reducedcostFeasibilities->getPartitionCount()==Simplex::PLUS){
                m_reducedcostFeasibilities->move(index,Simplex::FEASIBLE);
            }else
            if(m_reducedcostFeasibilities->getPartitionCount()==Simplex::FEASIBLE){
                m_reducedcostFeasibilities->move(index,Simplex::MINUS);
            }
        }else
        if(updateVector[index]==2){
            if(m_reducedcostFeasibilities->getPartitionCount()==Simplex::MINUS){
                m_reducedcostFeasibilities->move(index,Simplex::PLUS);
            }
        }else
        if(updateVector[index]==-2){
            if(m_reducedcostFeasibilities->getPartitionCount()==Simplex::PLUS){
                m_reducedcostFeasibilities->move(index,Simplex::MINUS);
            }
        }
    }

}
