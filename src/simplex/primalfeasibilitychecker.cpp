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
 * @file primalfeasibilitychecker.cpp
 */
#include <simplex/primalfeasibilitychecker.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>

PrimalFeasibilityChecker::PrimalFeasibilityChecker(const SimplexModel& model,
                                                   const DenseVector & basicVariableValues,
                                                   IndexList<> * basicVariableFeasibilities,
                                                   const std::vector<int>& basisHead):
    m_model(model),
    m_basicVariableValues(basicVariableValues),
    m_basicVariableFeasibilities(basicVariableFeasibilities),
    m_basisHead(basisHead)
{}

bool PrimalFeasibilityChecker::computeFeasibility(Numerical::Double tolerance){
//this function determines M/F/P sets, phaseI objective value
    m_basicVariableFeasibilities->clearPartition(Simplex::MINUS);
    m_basicVariableFeasibilities->clearPartition(Simplex::PLUS);
    m_basicVariableFeasibilities->clearPartition(Simplex::FEASIBLE);
    m_phaseIObjectiveValue = 0;

    Numerical::Double lbOfVariable = 0;
    Numerical::Double ubOfVariable = 0;
    Numerical::Double valueOfVariable = 0;

    for(unsigned int basisIndex = 0; basisIndex < m_basisHead.size(); basisIndex++){
        const Variable & basicVariable = m_model.getVariable(m_basisHead[basisIndex]);
        lbOfVariable = basicVariable.getLowerBound();
        ubOfVariable = basicVariable.getUpperBound();
//        valueOfVariable = *(m_variableStates->getAttachedData(m_basisHead[basisIndex]));
        valueOfVariable = m_basicVariableValues[basisIndex];

    //basic variables with M type infeasibility
        if ( (valueOfVariable + tolerance) < lbOfVariable)  {
            m_basicVariableFeasibilities->insert(Simplex::MINUS, basisIndex);
            m_phaseIObjectiveValue += (valueOfVariable - lbOfVariable);
        } else

    //basic variables with P type infeasibility
        if ( (valueOfVariable - tolerance) > ubOfVariable) {
            m_basicVariableFeasibilities->insert(Simplex::PLUS, basisIndex);
            m_phaseIObjectiveValue -= (valueOfVariable - ubOfVariable);
    //basic variables with F type infeasibility
        } else {
            m_basicVariableFeasibilities->insert(Simplex::FEASIBLE, basisIndex);
        }
    }

    IndexList<>::PartitionIterator setMit;
    IndexList<>::PartitionIterator setMendit;
    IndexList<>::PartitionIterator setPit;
    IndexList<>::PartitionIterator setPendit;

    m_basicVariableFeasibilities->getIterators(&setMit,&setMendit,Simplex::MINUS);
    m_basicVariableFeasibilities->getIterators(&setPit,&setPendit,Simplex::PLUS);

    return ( (setMit == setMendit) && (setPit == setPendit) );
}
