/**
 * @file dualratiotestupdater.cpp
 */

#include "simplex/dualratiotestupdater.h"
#include "simplex/simplex.h"

DualRatiotestUpdater::DualRatiotestUpdater(IndexList<>* reducedcostFeasiblities):
    m_reducedcostFeasibilities(reducedcostFeasiblities)
{

}

void DualRatiotestUpdater::updateFeasibilities()
{

    //TODO: updateVector not valid?!!
    for (unsigned int index = 0; index < m_updateVector.size(); index++) {
        if (m_updateVector[index] == 1) {
            if (m_reducedcostFeasibilities->where(index) == Simplex::MINUS) {
                m_reducedcostFeasibilities->move(index,Simplex::FEASIBLE);
            } else
            if (m_reducedcostFeasibilities->where(index) == Simplex::FEASIBLE) {
                m_reducedcostFeasibilities->move(index,Simplex::PLUS);
            }
        } else
        if (m_updateVector[index] == -1) {
            if (m_reducedcostFeasibilities->where(index) == Simplex::PLUS) {
                m_reducedcostFeasibilities->move(index,Simplex::FEASIBLE);
            } else
            if (m_reducedcostFeasibilities->where(index) == Simplex::FEASIBLE) {
                m_reducedcostFeasibilities->move(index,Simplex::MINUS);
            }
        } else
        if (m_updateVector[index] == 2) {
            if (m_reducedcostFeasibilities->where(index) == Simplex::MINUS) {
                m_reducedcostFeasibilities->move(index,Simplex::PLUS);
            }
        } else
        if (m_updateVector[index] == -2) {
            if (m_reducedcostFeasibilities->where(index) == Simplex::PLUS) {
                m_reducedcostFeasibilities->move(index,Simplex::MINUS);
            }
        }
    }
}
