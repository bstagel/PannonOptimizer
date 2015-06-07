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
 * @file sbf_crash.cpp
 **/

#include <simplex/startingbasisfinder/sbf_crash.h>
#include <simplex/simplex.h>
#include <utils/indexlist.h>

#include <utils/timer.h>

#include <fstream>

SbfCrash::SbfCrash(const SimplexModel& model,
                   std::vector<int>* basisHead,
                   IndexList<const Numerical::Double *> *variableStates,
                   StartingBasisFinder::STARTING_NONBASIC_STATES nonbasicStates):
    SbfSuper(model, basisHead, variableStates, nonbasicStates),
    m_time(0),
    m_structuralVariables(0)
{

}

SbfCrash::~SbfCrash()
{

}

void SbfCrash::run()
{
    Timer timer;

    LPINFO("CRASH eleje");
    timer.start();

    m_basisHead->clear();

    const unsigned int rowCount = m_model.getRowCount();
    const unsigned int columnCount = m_model.getColumnCount();

    std::vector<unsigned int> columnCounter(columnCount);

    // TODO: majd jo lenne meghatarozni a legsurubb sor suruseget, es
    // azzal memoriat sporolhatunk
    IndexList<> rowCounter(rowCount, columnCount + 1);

    unsigned int rowIndex;
    unsigned int columnIndex;

    const Matrix & matrix = m_model.getMatrix();

    unsigned int min = matrix.columnCount();
    for (rowIndex = 0; rowIndex < rowCount; rowIndex++) {
        unsigned int nonzeros = matrix.row(rowIndex).nonZeros();
        rowCounter.insert( nonzeros, rowIndex );
        if (nonzeros < min && nonzeros > 0) {
            min = nonzeros;
        }
    }

    for (columnIndex = 0; columnIndex < columnCount; columnIndex++) {
        columnCounter[columnIndex] = matrix.column(columnIndex).nonZeros();
    }

    unsigned int counter;
    bool ready = false;

    std::vector<char> nonbasicVarialbes(rowCount + columnCount, 1);
    std::vector<char> pivotRowsMap(rowCount, 1);

    for (counter = 0; counter < rowCount && ready == false; counter++) {
        unsigned int minIndex = 0;
        IndexList<>::Iterator rowCounterIter, rowCounterIterEnd;
        if (min == 0) {
            min = 1;
        }
        minIndex = min;
        rowCounter.getIterators(&rowCounterIter, &rowCounterIterEnd, minIndex, 1);
        while ( minIndex < rowCounter.getPartitionCount() && rowCounterIter == rowCounterIterEnd ) {
            minIndex++;
            if (minIndex < rowCounter.getPartitionCount()) {
                rowCounter.getIterators(&rowCounterIter, &rowCounterIterEnd, minIndex, 1);
            }
        }
        min = minIndex;
        if (minIndex >= columnCount) {
            ready = true;
        } else {

            minIndex = rowCounterIter.getData();

            // choose the column
            rowIndex = minIndex;
            SparseVector::NonzeroIterator iter = matrix.row(rowIndex).beginNonzero();
            SparseVector::NonzeroIterator iterEnd = matrix.row(rowIndex).endNonzero();

            while (iter != iterEnd && columnCounter[iter.getIndex()] == 0) {
                ++iter;
            }
            columnIndex = iter.getIndex();
            ++iter;
            for (; iter != iterEnd; ++iter) {
                if (columnCounter[ iter.getIndex() ] < columnCounter[columnIndex] &&
                        columnCounter[ iter.getIndex() ] > 0) {
                    columnIndex = iter.getIndex();
                }
            }

            m_basisHead->push_back(columnIndex);
            nonbasicVarialbes[columnIndex] = 0;

            rowCounter.move(rowIndex, 0);
            pivotRowsMap[rowIndex] = 0;

            iter = matrix.row(rowIndex).beginNonzero();
            iterEnd = matrix.row(rowIndex).endNonzero();
            for (; iter != iterEnd; ++iter) {
                columnCounter[ iter.getIndex() ] = 0;
                SparseVector::NonzeroIterator nonzIter = matrix.column( iter.getIndex() ).beginNonzero();
                SparseVector::NonzeroIterator nonzIterEnd = matrix.column( iter.getIndex() ).endNonzero();
                for (; nonzIter != nonzIterEnd; ++nonzIter) {
                    unsigned int currentCount = rowCounter.where( nonzIter.getIndex() );
                    if (currentCount > 0 && nonzIter.getIndex() != rowIndex) {
                        currentCount--;
                        rowCounter.move( nonzIter.getIndex(), currentCount );
                        if (currentCount < min) {
                            min = currentCount;
                        }
                    }
                }
            }
            columnCounter[columnIndex] = 0;
        }

    }

    // insert logical columns
    unsigned int index;
    m_structuralVariables = m_basisHead->size();

    for (index = 0; index < rowCount; index++) {
        if (pivotRowsMap[index] == 1) {
            m_basisHead->push_back(index + columnCount);
            nonbasicVarialbes[index + columnCount] = 0;
        }
    }

    // set variable types
    // basic variables
    for (index = 0; index < rowCount; index++) {
        m_variableStates->insert(Simplex::BASIC, (*m_basisHead)[index], &ZERO);
    }

    // nonbasic variables
    switch (m_nonbasicStates) {

    case StartingBasisFinder::LOWER: {
        for (index = 0; index < rowCount + columnCount; index++) {
            if (nonbasicVarialbes[index] == 1) {
                adjustVariableByType(index, Simplex::NONBASIC_AT_LB);
            }
        }
        break;
    }

    case StartingBasisFinder::UPPER: {
        for (index = 0; index < rowCount + columnCount; index++) {
            if (nonbasicVarialbes[index] == 1) {
                adjustVariableByType(index, Simplex::NONBASIC_AT_UB);
            }
        }
        break;
    }

    case StartingBasisFinder::MIXED: {
        const DenseVector & costs = m_model.getCostVector();
        //Always minimize
        for (index = 0; index < rowCount + columnCount; index++) {
            if (nonbasicVarialbes[index] == 1) {
                if (costs.at(index) < 0.0) {
                    adjustVariableByType(index, Simplex::NONBASIC_AT_LB);
                } else {
                    adjustVariableByType(index, Simplex::NONBASIC_AT_UB);
                }
            }
        }

        break;
    }

    default:
        throw ParameterException("Invalid starting nonbasic state given!");
        break;

    }

    timer.stop();

    m_time = timer.getTotalElapsed();

}
