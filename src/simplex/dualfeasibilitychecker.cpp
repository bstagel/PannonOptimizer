//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

/**
 * @file dualfeasibilitychecker.cpp
 */

#include <simplex/dualfeasibilitychecker.h>
#include <lp/model.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/pfibasis.h>

DualFeasibilityChecker::DualFeasibilityChecker(const SimplexModel& model,
                                               const std::vector<int>& basisHead,
                                               IndexList<const Numerical::Double *> *variableStates,
                                               IndexList<>* reducedCostFeasibilities,
                                               const DenseVector &reducedCosts,
                                               const Basis& basis):
    m_model(model),
    m_basisHead(basisHead),
    m_variableStates(variableStates),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_reducedCosts(reducedCosts),
    m_basis(basis)
{}

bool DualFeasibilityChecker::computeFeasibility(Numerical::Double tolerance){
//this function determines M/F/P sets, phase I objective function value
    m_reducedCostFeasibilities->clearPartition(Simplex::FEASIBLE);
    m_reducedCostFeasibilities->clearPartition(Simplex::MINUS);
    m_reducedCostFeasibilities->clearPartition(Simplex::PLUS);
    m_phaseIObjectiveValue = 0;

    Variable::VARIABLE_TYPE typeOfIthVariable;

    for(unsigned int variableIndex = 0; variableIndex < m_reducedCosts.length(); variableIndex++){
        if (m_variableStates->where(variableIndex) != Simplex::BASIC) {
            typeOfIthVariable = m_model.getVariable(variableIndex).getType();
    //nonbasic variables with M type infeasibility
                if (m_reducedCosts.at(variableIndex) < -tolerance &&
                        (typeOfIthVariable == Variable::PLUS || typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::MINUS,variableIndex);
                    m_phaseIObjectiveValue += m_reducedCosts.at(variableIndex);
                } else

    //nonbasic variables with P type infeasibility
                if (m_reducedCosts.at(variableIndex) > tolerance &&
                        (typeOfIthVariable == Variable::MINUS || typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::PLUS,variableIndex);
                    m_phaseIObjectiveValue -= m_reducedCosts.at(variableIndex);
    //nonbasic variables with F type infeasibility
                } else if(typeOfIthVariable != Variable::BOUNDED && typeOfIthVariable != Variable::FIXED){
                    m_reducedCostFeasibilities->insert(Simplex::FEASIBLE,variableIndex);
                }
        }
    }

    IndexList<>::PartitionIterator setMit;
    IndexList<>::PartitionIterator setMendit;
    IndexList<>::PartitionIterator setPit;
    IndexList<>::PartitionIterator setPendit;

    m_reducedCostFeasibilities->getIterators(&setMit,&setMendit,Simplex::MINUS);
    m_reducedCostFeasibilities->getIterators(&setPit,&setPendit,Simplex::PLUS);
    return ((setMit == setMendit) && (setPit == setPendit));
}

void DualFeasibilityChecker::feasibilityCorrection(DenseVector* basicVariableValues, Numerical::Double * objValue, Numerical::Double tolerance) {
    unsigned int rowCount = m_model.getRowCount();
    unsigned int columnCount = m_model.getColumnCount();

    DenseVector transformVector(basicVariableValues->length());

    //the objective value should be corrected here, feas correction changes the x_b values, thus c_j * x_b changes too
    //as well as the boundflips if the corresponding c_j is nonzero
    //TODO: Increase efficiency by checking only NONBASIC_AT_*B lists
    for(unsigned int variableIndex = 0; variableIndex < m_reducedCosts.length(); variableIndex++){
        const Variable& variable = m_model.getVariable(variableIndex);
        if (variable.getType() == Variable::BOUNDED){
            if (m_variableStates->where(variableIndex) == Simplex::NONBASIC_AT_LB && m_reducedCosts.at(variableIndex) < -tolerance){
                //Do a bound flip LB -> UB (T+ set)
                m_variableStates->move(variableIndex, Simplex::NONBASIC_AT_UB, &(variable.getUpperBound()));
                //Compute (l_j-u_j)*a_j
                Numerical::Double theta = variable.getUpperBound() - variable.getLowerBound();
                if(variableIndex < columnCount){
                    transformVector.addVector(-1 * theta, m_model.getMatrix().column(variableIndex));
                } else {
                    SparseVector logical(rowCount);
                    logical.setNewNonzero(variableIndex - columnCount,1);
                    transformVector.addVector(-1 * theta, logical);
                }
                //correction if c_j is nonzero
                Numerical::Double c_j = m_model.getCostVector().at(variableIndex);
                if ( c_j != 0) {
                    (*objValue) += theta * c_j;
                }
            } else
                if (m_variableStates->where(variableIndex) == Simplex::NONBASIC_AT_UB && m_reducedCosts.at(variableIndex) > tolerance){
                //Do a bound flip UB -> LB (T- set)
                //Swap states
                m_variableStates->move(variableIndex, Simplex::NONBASIC_AT_LB, &(variable.getLowerBound()));
                //Compute (u_j-l_j)*a_j
                Numerical::Double theta = variable.getLowerBound() - variable.getUpperBound();
                if(variableIndex < columnCount){
                    transformVector.addVector(-1 * theta, m_model.getMatrix().column(variableIndex));
                } else {
                    SparseVector logical(rowCount);
                    logical.setNewNonzero(variableIndex - columnCount,1);
                    transformVector.addVector(-1 * theta, logical);
                }
                //correction if c_j is nonzero
                Numerical::Double c_j = m_model.getCostVector().at(variableIndex);
                if ( c_j != 0) {
                    (*objValue) += theta * c_j;
                }
            }
        }
    }
    //Transform x_b
    m_basis.Ftran(transformVector);
    basicVariableValues->addVector(1, transformVector);

    //Correcting z with delta x_b
    DenseVector::NonzeroIterator it = transformVector.beginNonzero();
    DenseVector::NonzeroIterator endit = transformVector.endNonzero();
    for(; it < endit; ++it) {
        unsigned variableIndex = m_basisHead.at(it.getIndex());
        Numerical::Double c_j = m_model.getCostVector().at(variableIndex);
        if (c_j != 0) {
            (*objValue) += c_j * (*it);
        }
    }
}

bool DualFeasibilityChecker::updateFeasibilities(const std::vector<std::pair<int, char> > &updateFeasibilitySets,
                                                 const std::vector<int>& becomesFeasible)
{
    //TODO: reservek koknstruktorbol
    LPINFO("UPDATE FEASIBILITIES");
    for (unsigned int index = 0; index < updateFeasibilitySets.size(); index++) {
        int variableIndex = updateFeasibilitySets[index].first;
        if (updateFeasibilitySets[index].second == 1) {
            if (m_reducedCostFeasibilities->where(variableIndex) == Simplex::MINUS) {
                m_reducedCostFeasibilities->move(variableIndex,Simplex::FEASIBLE);
            } else
            if (m_reducedCostFeasibilities->where(variableIndex) == Simplex::FEASIBLE) {
                m_reducedCostFeasibilities->move(variableIndex,Simplex::PLUS);
            }
        } else
        if (updateFeasibilitySets[index].second == 0) {
            if (m_reducedCostFeasibilities->where(variableIndex) == Simplex::PLUS) {
                m_reducedCostFeasibilities->move(variableIndex,Simplex::FEASIBLE);
            } else
            if (m_reducedCostFeasibilities->where(variableIndex) == Simplex::FEASIBLE) {
                m_reducedCostFeasibilities->move(variableIndex,Simplex::MINUS);
            }
        }
    }
    for(unsigned index = 0; index < becomesFeasible.size(); index++){
        m_reducedCostFeasibilities->move(becomesFeasible[index],Simplex::FEASIBLE);
    }
    //TODO: Compute phase 1 objective function too!
    IndexList<>::PartitionIterator setMit;
    IndexList<>::PartitionIterator setMendit;
    IndexList<>::PartitionIterator setPit;
    IndexList<>::PartitionIterator setPendit;

    m_reducedCostFeasibilities->getIterators(&setMit,&setMendit,Simplex::MINUS);
    m_reducedCostFeasibilities->getIterators(&setPit,&setPendit,Simplex::PLUS);

    return ((setMit == setMendit) && (setPit == setPendit));
}
