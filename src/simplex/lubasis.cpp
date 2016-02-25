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
 * @file lubasis.cpp
 */

#include <string>

#include <simplex/lubasis.h>
#include <simplex/simplex.h>

#include <simplex/simplexparameterhandler.h>

thread_local int LuBasis::m_inversionCount = 0;

LuBasis::LuBasis() :
    Basis(),
    m_threshold(SimplexParameterHandler::getInstance().getDoubleParameterValue("Factorization.pivot_threshold"))
{
    m_lower = new std::vector<DoubleETM>();
    m_upper = new std::vector<DoubleETM>();
    m_updateETMs = new std::vector<ETM>();

    m_transformationCount = 0;
    m_transformationAverage = 0;
    m_mNumAverage = 0;

    m_columnCountIndexList.init(0,0);
    m_rowCountIndexList.init(0,0);
}

LuBasis::~LuBasis()
{
    for (std::vector<DoubleETM>::iterator it = m_lower->begin(); it < m_lower->end(); ++it) {
        if(it->row != NULL){
            delete it->row;
        }
        if(it->column != NULL){
            delete it->column;
        }

    }
    delete m_lower;

    for (std::vector<DoubleETM>::iterator it = m_upper->begin(); it < m_upper->end(); ++it) {
        if(it->row != NULL){
            delete it->row;
        }
        if(it->column != NULL){
            delete it->column;
        }

    }
    delete m_upper;

    for (std::vector<ETM>::iterator it = m_updateETMs->begin(); it < m_updateETMs->end(); ++it) {
        delete it->eta;
    }
    delete m_updateETMs;
}

void LuBasis::registerThread() {

}

void LuBasis::releaseThread() {

}

void LuBasis::prepareForModel(const Model &model)
{
    __UNUSED(model);
}

void LuBasis::releaseModel()
{

}

void LuBasis::invert()
{
    m_transformationCount = 0;
    m_inversionCount++;

    if(m_lower->size() == 0){
        m_lower->resize(m_basisHead->size(),DoubleETM());
        m_upper->resize(m_basisHead->size(),DoubleETM());
    } else {
        for (std::vector<DoubleETM>::iterator it = m_lower->begin(); it < m_lower->end(); ++it) {
            delete it->row;
            it->row = NULL;
            delete it->column;
            it->column = NULL;
        }

        for (std::vector<DoubleETM>::iterator it = m_upper->begin(); it < m_upper->end(); ++it) {
            delete it->row;
            it->row = NULL;
            delete it->column;
            it->column = NULL;
        }
    }

    for (std::vector<ETM>::iterator it = m_updateETMs->begin(); it < m_updateETMs->end(); ++it) {
        delete it->eta;
    }

    m_updateETMs->clear();

    m_pivots.clear();
    m_pivotIndices.resize(m_basisHead->size());
    m_factorizedPart = 0;

    m_basisNewHead.clear();

    m_basisNonzeros = 0;
    m_inverseNonzeros = 0;

    //Copy the basis for computation
    copyBasis();

//    LPINFO("basis: ");
//    for(int i = 0; i<m_basisHead->size(); i++){
//        for(int j = 0; j<m_basisHead->size(); j++){
//            std::cout<<m_basicColumns.at(j)->at(i)<<" ";
//        }
//        std::cout<<"\n";
//    }

    //Invert the R, M, C parts separately
    invertC();
    invertR();
    invertM();

    //Free the copied columns
    for(unsigned int i=0; i<m_basicColumnCopies.size(); i++){
        if(m_basicColumnCopies[i] != NULL){
            delete m_basicColumnCopies[i];
            m_basicColumnCopies[i] = NULL;
        }
    }

    //Free the basic rows
    for(unsigned int i=0; i<m_basicRows.size(); i++){
        if(m_basicRows[i] != NULL){
            delete m_basicRows[i];
            m_basicRows[i] = NULL;
        }
    }

    //Verify the result of the inversion

    checkSingularity();

    int basisSize = m_basisHead->size();
    //Create the rowwise representation of L
    for(int i=0; i<basisSize; i++){
        SparseVector::NonzeroIterator it = (*m_lower)[i].column->beginNonzero();
        SparseVector::NonzeroIterator itend = (*m_lower)[i].column->endNonzero();
        for(; it != itend; ++it){
            (*m_lower)[it.getIndex()].row->setNewNonzero(i, *it);
        }
    }
    //Create the columnwise representation of U
    for(int i=0; i<basisSize; i++){
        SparseVector::NonzeroIterator it = (*m_upper)[i].row->beginNonzero();
        SparseVector::NonzeroIterator itend = (*m_upper)[i].row->endNonzero();
        for(; it != itend; ++it){
            (*m_upper)[it.getIndex()].column->setNewNonzero(i, *it);
        }
    }

    //Update the basis head
    setNewHead();

    m_isFresh = true;

    //printStatistics();
    m_transformationAverage += (m_transformationCount - m_transformationAverage) / m_inversionCount;

}


void LuBasis::copyBasis()
{
    DEVINFO(D::PFIMAKER, "Copy the basis");
    unsigned int columnCount = m_model->getColumnCount();
    unsigned int rowCount = m_model->getRowCount();

    //Containers to be resized directly
    m_basicColumns.clear();
    m_basicColumns.reserve(rowCount);
    m_basicColumnCopies.resize(rowCount, NULL);
    m_basicRows.resize(rowCount, NULL);

    m_basisNewHead.resize(rowCount, -1);

#ifndef NDEBUG
    std::vector<char> headChecker(rowCount + columnCount, 0);
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); ++it) {
        if (headChecker[*it] == 0) {
            headChecker[*it] = 1;
        } else {
            LPWARNING("Duplicate index in basis head: " << *it);
            throw PanOptException("Duplicate index in the basis head!");
        }
    }
#endif

    //Copy the active submatrix
    unsigned int maxColumnCount = 0;
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); ++it) {
        if (*it >= (int) columnCount) {
            DEVINFO(D::PFIMAKER, "Logical variable found in basis head: y" << *it - columnCount);
            //Ignore logical columns from the inverse
            int basisIndex = it-m_basisHead->begin();
            m_basicColumnCopies[basisIndex] = new SparseVector(rowCount);
            m_basicColumnCopies[basisIndex]->setNewNonzero(*it - columnCount, 1);
            m_basicColumns.push_back(m_basicColumnCopies[basisIndex]);
            if(maxColumnCount < 1){
                maxColumnCount = 1;
            }
        } else {
            DEVINFO(D::PFIMAKER, "Structural variable found in basis head: x" << *it);
            //The submatrix is the active submatrix needed for inversion
            m_basicColumns.push_back(&(m_model->getMatrix().column(*it)));
            //IGNORE//
            if (maxColumnCount < m_basicColumns.back()->nonZeros()) {
                maxColumnCount = m_basicColumns.back()->nonZeros();
            }
            m_basisNonzeros += m_basicColumns.back()->nonZeros();
        }
        m_basicRows[it - m_basisHead->begin()] = new SparseVector(rowCount);
    }
    //Set up row counts, column counts (r_i, c_i) and the corresponding row lists
    for (std::vector<const SparseVector*>::iterator it = m_basicColumns.begin(); it < m_basicColumns.end(); ++it) {
        int columnIndex = it - m_basicColumns.begin();
        SparseVector::NonzeroIterator vectorIt = (*it)->beginNonzero();
        SparseVector::NonzeroIterator vectorItEnd = (*it)->endNonzero();
        for (; vectorIt < vectorItEnd; ++vectorIt) {
            int rowIndex = vectorIt.getIndex();
            m_basicRows[rowIndex]->setNewNonzero(columnIndex, *vectorIt);
        }
    }
    unsigned int maxRowCount = 0;
    for (unsigned int i = 0; i < m_basicRows.size(); i++) {
        if (maxRowCount < m_basicRows[i]->nonZeros()) {
            maxRowCount = m_basicRows[i]->nonZeros();
        }
    }

    //TODO: Use smaller lists!
    //TODO: Use previously allocated memory pieces
    if(m_rowCountIndexList.getPartitionCount() == 0 ){
        m_rowCountIndexList.init(m_basisHead->size(), m_basicColumns.size()+1);
        m_columnCountIndexList.init(m_basisHead->size(), m_basicColumns.size()+1);
    }

    for (unsigned int i = 0; i < m_basicRows.size(); i++) {
        m_rowCountIndexList.insert(m_basicRows[i]->nonZeros(), i);
    }

    for (unsigned int i = 0; i < m_basicColumns.size(); i++) {
        m_columnCountIndexList.insert(m_basicColumns[i]->nonZeros(), i);
    }
}


void LuBasis::invertC()
{
    //The upper triangular part is called C part
    DEVINFO(D::PFIMAKER, "Search for the C part and invert it");
    unsigned int cNum = 0;

    if(m_columnCountIndexList.getPartitionCount() > 1){
        //If there is a column with the column count of 1
        while(m_columnCountIndexList.firstElement(1) != -1) {
            int columnindex = m_columnCountIndexList.firstElement(1);
            int rowindex;
            const SparseVector * currentColumn = m_basicColumns[columnindex];
            //Get the active nonzero from the column
            SparseVector::NonzeroIterator it = currentColumn->beginNonzero();
            SparseVector::NonzeroIterator itend = currentColumn->endNonzero();
            unsigned int rowPartitionCount = m_rowCountIndexList.getPartitionCount();
            int count = 0;
            for(; it != itend; ++it){
                if(m_rowCountIndexList.where(it.getIndex()) < rowPartitionCount){
                    rowindex = it.getIndex();
                    count++;
                }
            }
            if(count != 1){
                LPERROR("COUNT ERROR");
                exit(-1);
            }
            const SparseVector * currentRow = m_basicRows[rowindex];

            int basisSize = m_basisHead->size();
            SparseVector* unitColumn = new SparseVector(basisSize);
            unitColumn->setNewNonzero(rowindex, 1.0);
            (*m_lower)[columnindex] = DoubleETM(new SparseVector(basisSize), unitColumn);
            (*m_upper)[rowindex] = DoubleETM(new SparseVector(*currentRow), new SparseVector(basisSize));
            m_pivots.emplace_back(rowindex, columnindex);
            m_pivotIndices[rowindex].rowindex = m_factorizedPart;
            m_pivotIndices[columnindex].columnindex = m_factorizedPart;

            m_basisNewHead[columnindex] = (*m_basisHead)[columnindex];

            //Update the column lists and column counts
            it = currentRow->beginNonzero();
            itend = currentRow->endNonzero();
            unsigned int columnPartitionCount = m_columnCountIndexList.getPartitionCount();
            for (; it < itend; ++it) {
                int index = it.getIndex();
                //If the column of the iterated element is still active (Probably the if is unnecessary)
                if (m_columnCountIndexList.where(index) < columnPartitionCount) {
                    m_columnCountIndexList.move(index, m_columnCountIndexList.where(index)-1 );
                }
            }

            //Set the row count to zero to represent that which row has been chosen.
            m_columnCountIndexList.remove(columnindex);
            m_rowCountIndexList.remove(rowindex);

            cNum++;
            m_factorizedPart++;
        }
    }
//    LPINFO( "CPART num: " << cNum);
}

void LuBasis::invertR()
{
    //The upper triangular part is called R part
    DEVINFO(D::PFIMAKER, "Search for the R part and invert it");
    unsigned int rNum = 0;

    if(m_rowCountIndexList.getPartitionCount() > 1){
        //If there is a column with the column count of 1
        while(m_rowCountIndexList.firstElement(1) != -1) {
            int rowindex = m_rowCountIndexList.firstElement(1);
            int columnindex;
            const SparseVector * currentRow = m_basicRows[rowindex];
            //Get the active nonzero from the column
            SparseVector::NonzeroIterator it = currentRow->beginNonzero();
            SparseVector::NonzeroIterator itend = currentRow->endNonzero();
            unsigned int columnPartitionCount = m_columnCountIndexList.getPartitionCount();
            for(; it != itend; ++it){
                if(m_columnCountIndexList.where(it.getIndex()) < columnPartitionCount){
                    columnindex = it.getIndex();
                }
            }

//            LPINFO("PIVOT ELEMENT: rowindex: "<<rowindex<< " - columnindex: "<<columnindex);
            //Create the vector to be transformed into an eta vector
            const SparseVector * currentColumn = m_basicColumns[columnindex];
            SparseVector transformVector (currentColumn->length());
            it = currentColumn->beginNonzero();
            itend = currentColumn->endNonzero();
            unsigned int rowPartitionCount = m_rowCountIndexList.getPartitionCount();
            for(;it != itend; ++it){
                if(m_rowCountIndexList.where(it.getIndex()) < rowPartitionCount){
                    transformVector.setNewNonzero(it.getIndex(),*it);
                }
            }

            int basisSize = m_basisHead->size();
            SparseVector* unitRow = new SparseVector(basisSize);
            unitRow->setNewNonzero(columnindex, 1.0);
            (*m_lower)[columnindex] = DoubleETM(new SparseVector(basisSize), createEta(transformVector,rowindex));
            (*m_upper)[rowindex] = DoubleETM(unitRow, new SparseVector(basisSize));
            m_pivots.emplace_back(rowindex, columnindex);
            m_pivotIndices[rowindex].rowindex = m_factorizedPart;
            m_pivotIndices[columnindex].columnindex = m_factorizedPart;

            m_basisNewHead[columnindex] = (*m_basisHead)[columnindex];

            //Update the row lists and row counts
            it = transformVector.beginNonzero();
            itend = transformVector.endNonzero();
            for (; it < itend; ++it) {
                int index = it.getIndex();
                //If the row of the iterated element is still active (Probably the if is unnecessary)
                if (m_rowCountIndexList.where(index) < rowPartitionCount) {
                    m_rowCountIndexList.move(index, m_rowCountIndexList.where(index)-1);
                }
            }

            //Set the row count to zero to represent that which row has been chosen.
            m_columnCountIndexList.remove(columnindex);
            m_rowCountIndexList.remove(rowindex);

            rNum++;
            m_factorizedPart++;
        }
    }
//    LPINFO( "RPART num: " << rNum);
}

void LuBasis::invertM()
{
    DEVINFO(D::PFIMAKER, "Search for the M part and invert it");
    unsigned int mNum = 0;

    if(m_factorizedPart == m_basisHead->size()){
        return;
    }
    std::list<int> activeRows;
    int basisSize = m_basisHead->size();
    unsigned int rowPartitionCount = m_rowCountIndexList.getPartitionCount();
    unsigned int columnPartitionCount = m_columnCountIndexList.getPartitionCount();
    //Clear up the rowwise representation
    //Bild a list from the active rows
    //Get the absolute maximal elements of the rows
    std::vector<Numerical::Double> rowAbsMaxs;
    rowAbsMaxs.resize(basisSize,0);
    for(int rowIndex=0; rowIndex<basisSize; rowIndex++){
        if(m_rowCountIndexList.where(rowIndex) < rowPartitionCount){
            //The row is still active
            activeRows.push_back(rowIndex);
            //Collect the active nonzeros
            SparseVector* kernelRow = new SparseVector(basisSize);
            SparseVector::NonzeroIterator it = m_basicRows[rowIndex]->beginNonzero();
            SparseVector::NonzeroIterator itend = m_basicRows[rowIndex]->endNonzero();
            for(; it != itend; ++it){
                if(m_columnCountIndexList.where(it.getIndex()) < columnPartitionCount){
                    kernelRow->setNewNonzero(it.getIndex(),*it);
                    if(rowAbsMaxs[rowIndex] < Numerical::fabs(*it)){
                        rowAbsMaxs[rowIndex] = Numerical::fabs(*it);
                    }
                }
            }
            delete m_basicRows[rowIndex];
            m_basicRows[rowIndex] = kernelRow;
        }
    }
    while(activeRows.size()>0){
        {
//        LPWARNING("kernel: ");
//        std::list<int>::iterator activeRowIt;
//        std::list<int>::iterator activeRowItend;
//        activeRowIt = activeRows.begin();
//        activeRowItend = activeRows.end();
//        LPWARNING("rowcounts: "<<m_rowCountIndexList);
//        LPWARNING("columncounts: "<<m_columnCountIndexList);

//        for(; activeRowIt != activeRowItend; activeRowIt++){
//            LPWARNING("row: "<<*activeRowIt << " --- "<<*m_basicRows.at(*activeRowIt));
//        }
        }
        //Data structures for the Markowitz computation
        std::vector<int> rowMarkowitzs;
        std::vector<int> rowMarkowitzColumnIndices;
        rowMarkowitzs.resize(basisSize,basisSize*basisSize);
        rowMarkowitzColumnIndices.resize(basisSize);
        std::vector<int> columnMarkowitzs;
        std::vector<int> columnMarkowitzRowIndices;
        columnMarkowitzs.resize(basisSize,basisSize*basisSize);
        columnMarkowitzRowIndices.resize(basisSize);
        const double & pivotThreshold = SimplexParameterHandler::getInstance().getDoubleParameterValue("Factorization.pivot_threshold");
        std::list<int>::iterator activeRowIt;
        std::list<int>::iterator activeRowItend;
        //Compute the row and column Markowitz numbers
        activeRowIt = activeRows.begin();
        activeRowItend = activeRows.end();
        for(; activeRowIt != activeRowItend; activeRowIt++){
            int rowindex = *activeRowIt;
            int currentMarkowitz;
            SparseVector::NonzeroIterator it = m_basicRows[rowindex]->beginNonzero();
            SparseVector::NonzeroIterator itend = m_basicRows[rowindex]->endNonzero();
            for(; it != itend; ++it){
                int columnindex = it.getIndex();
                //If the column is singleton, set the only possible choice
                if(m_columnCountIndexList.where(columnindex) == 1){
                    columnMarkowitzs[columnindex] = 0;
                    columnMarkowitzRowIndices[columnindex] = rowindex;
                } else
                //Check the threshold pivot criterion
                if(Numerical::fabs(*it) >= rowAbsMaxs[rowindex]*pivotThreshold){
                    //Update the Markowitz numbers
                    currentMarkowitz = (m_rowCountIndexList.where(rowindex)-1) * (m_columnCountIndexList.where(columnindex)-1);
                    if(currentMarkowitz < rowMarkowitzs[rowindex]){
                        rowMarkowitzs[rowindex] = currentMarkowitz;
                        rowMarkowitzColumnIndices[rowindex] = columnindex;
                    }
                    if(currentMarkowitz < columnMarkowitzs[columnindex]){
                        columnMarkowitzs[columnindex] = currentMarkowitz;
                        columnMarkowitzRowIndices[columnindex] = rowindex;
                    }
                }
            }
        }

        //Procedure find pivot
        int rowindex;
        int columnindex;
        findPivot(rowindex, columnindex, rowMarkowitzs, columnMarkowitzs, rowMarkowitzColumnIndices, columnMarkowitzRowIndices);
        //pivotU first
        SparseVector * pivotRow = m_basicRows[rowindex];
        m_inverseNonzeros += pivotRow->nonZeros();
        (*m_upper)[rowindex] = DoubleETM(new SparseVector(*pivotRow), new SparseVector(basisSize));
        //Compute the eta column for pivotL and do the elimination
        SparseVector * etaColumn = new SparseVector(basisSize);
        //Set the unit element in the eta column
        etaColumn->setNewNonzero(rowindex, 1);
        //Scan the active rows, compute the eta column and do the updates
        activeRowIt = activeRows.begin();
        activeRowItend = activeRows.end();
        //Scatter currentRow here
        Numerical::Double pivotElement = pivotRow->at(columnindex);
//        LPINFO("PIVOT ELEMENT: rowindex: "<<rowindex<< " - columnindex: "<<columnindex);
        for(; activeRowIt != activeRowItend;){
            //Skip the pivot row from updating and reduce the kernel
            if(*activeRowIt == rowindex){
                //Reduce the kernel here
                SparseVector::NonzeroIterator rowIt = pivotRow->beginNonzero();
                SparseVector::NonzeroIterator rowItend = pivotRow->endNonzero();
                for(; rowIt != rowItend; ++rowIt){
                    m_columnCountIndexList.move(rowIt.getIndex(),m_columnCountIndexList.where(rowIt.getIndex())-1);
                }
                activeRows.erase(activeRowIt++);
                m_rowCountIndexList.remove(rowindex);
            } else {
                SparseVector * currentRow = m_basicRows[*activeRowIt];
                Numerical::Double pivotColumnValue = currentRow->at(columnindex);
                //If the row should be updated
                if(pivotColumnValue != 0){
                    //Remove the column counts on the active row
                    SparseVector::NonzeroIterator rowIt = currentRow->beginNonzero();
                    SparseVector::NonzeroIterator rowItend = currentRow->endNonzero();
                    for(; rowIt != rowItend; ++rowIt){
                        m_columnCountIndexList.move(rowIt.getIndex(),m_columnCountIndexList.where(rowIt.getIndex())-1);
                    }
                    //Compute the eta multiplier, store it in the eta column and update the row
                    Numerical::Double eta = - pivotColumnValue / pivotElement;
                    etaColumn->setNewNonzero(*activeRowIt, eta);
                    currentRow->addVector(eta, *pivotRow);

                    //Add the column counts on the updated row and update the max element
                    rowAbsMaxs[*activeRowIt] = 0.0;
                    rowIt = currentRow->beginNonzero();
                    rowItend = currentRow->endNonzero();
                    for(; rowIt != rowItend; ++rowIt){
                        m_columnCountIndexList.move(rowIt.getIndex(),m_columnCountIndexList.where(rowIt.getIndex())+1);
                        if(rowAbsMaxs[*activeRowIt] < *rowIt){
                            rowAbsMaxs[*activeRowIt] = *rowIt;
                        }
                    }
                    m_rowCountIndexList.move(*activeRowIt, currentRow->nonZeros());
                }
                activeRowIt++;
            }
        }
        //TODO: Debug check only
        if(m_columnCountIndexList.where(columnindex) != 0){
            LPERROR("Columncount is not zero when removing pivot column");
            exit(-1);
        }
        m_columnCountIndexList.remove(columnindex);
        //Do the pivotL and update the basis head
        m_inverseNonzeros += etaColumn->nonZeros();
        (*m_lower)[columnindex] = DoubleETM(new SparseVector(basisSize), etaColumn);
        m_pivots.emplace_back(rowindex, columnindex);
        m_pivotIndices[rowindex].rowindex = m_factorizedPart;
        m_pivotIndices[columnindex].columnindex = m_factorizedPart;

        m_basisNewHead[columnindex] = (*m_basisHead)[columnindex];

        mNum++;
        m_factorizedPart++;
    }
//    LPINFO( "MPART num: " << mNum);
}

void LuBasis::checkIndexListValidities(){
    unsigned int rowPartitionCount = m_rowCountIndexList.getPartitionCount();
    unsigned int columnPartitionCount = m_columnCountIndexList.getPartitionCount();
    for(int i=0; i<(int)rowPartitionCount; i++){
        IndexList<>::Iterator it;
        IndexList<>::Iterator itend;
        m_rowCountIndexList.getIterators(&it, &itend, i);
        for(; it != itend; ++it){
            int rowindex = it.getData();
            int activeCounter = 0;
            SparseVector::NonzeroIterator rowit = m_basicRows[rowindex]->beginNonzero();
            SparseVector::NonzeroIterator rowitend = m_basicRows[rowindex]->endNonzero();
            for(; rowit != rowitend; ++rowit){
                if(m_columnCountIndexList.where(rowit.getIndex()) < columnPartitionCount){
                    activeCounter++;
                }
            }
            if(activeCounter != i){
                LPERROR("INVALID ROW COUNT LIST ("<<rowindex<<"): partition index: "<<i << " ; activeCounter: "<<activeCounter);
            }
        }
    }
    for(int i=0; i<(int)columnPartitionCount; i++){
        IndexList<>::Iterator it;
        IndexList<>::Iterator itend;
        m_columnCountIndexList.getIterators(&it, &itend, i);
        for(; it != itend; ++it){
            int columnindex = it.getData();
            int activeCounter = 0;
            for(int j = 0; j < (int)m_basisHead->size(); j++){
                if(m_rowCountIndexList.where(j) < rowPartitionCount){
                    if(m_basicRows[j]->at(columnindex) != 0){
                        activeCounter++;
                    }
                }
            }
            if(activeCounter != i){
                LPERROR("INVALID COLUMN COUNT LIST ("<<columnindex<<"): partition index: "<<i << " ; activeCounter: "<<activeCounter);
            }
        }

    }
}

//TODO: Ignore columns with no available pivot element
void LuBasis::findPivot(int & rowindex, int & columnindex,
                        const std::vector<int> & rowMarkowitzs, const std::vector<int> & columnMarkowitzs,
                        const std::vector<int> & rowMarkowitzColumnIndices, const std::vector<int> & columnMarkowitzRowIndices)
{
    //TODO DEBUG
//    checkIndexListValidities();

    //Since the kernel is stored rowwise only, row singleton comes first
    if(m_rowCountIndexList.firstElement(1) != -1){
        rowindex = m_rowCountIndexList.firstElement(1);
        if(m_basicRows[rowindex]->nonZeros() != 1){
            LPERROR("False singleton row!");
        }
        columnindex = m_basicRows[rowindex]->beginNonzero().getIndex();
        return;
    }
    if(m_columnCountIndexList.firstElement(1) != -1){
        columnindex = m_columnCountIndexList.firstElement(1);
        rowindex = columnMarkowitzRowIndices[columnindex];
        return;
    }
    unsigned int basisSize = m_basisHead->size();
    //bestMarkowitz is mu in the book
    unsigned int bestMarkowitz = basisSize*basisSize;
    //pivotFound indicates that there is an active candidate
    bool pivotFound = false;
    //visitedVectorCount is nu in the book;
    unsigned int visitedVectorCount = 0;
    //visitedVectorLimit is r in the book;
//    int markowitzSearchLimit = SimplexParameterHandler::getInstance().getIntegerParameterValue("markowitz_search_limit");
    unsigned int markowitzSearchLimit = 4;
    unsigned int columnPartitionCount = m_columnCountIndexList.getPartitionCount();
    unsigned int rowPartitionCount = m_rowCountIndexList.getPartitionCount();
    for(unsigned int k=2; k < basisSize; k++){
        if(k<columnPartitionCount){
            IndexList<>::Iterator it;
            IndexList<>::Iterator itend;
            m_columnCountIndexList.getIterators(&it, &itend, k);
            for(; it != itend; ++it){
                if(columnMarkowitzs[it.getData()] < (int)bestMarkowitz){
                    bestMarkowitz = columnMarkowitzs[it.getData()];
                    columnindex = it.getData();
                    rowindex = columnMarkowitzRowIndices[it.getData()];
                    pivotFound = true;
                    if(bestMarkowitz < (k-1)*(k-1)){
                        return;
                    }
                }
                visitedVectorCount++;
                if(pivotFound && visitedVectorCount >= markowitzSearchLimit){
                    return;
                }
            }
        }
        if(k<rowPartitionCount){
            IndexList<>::Iterator it;
            IndexList<>::Iterator itend;
            m_rowCountIndexList.getIterators(&it, &itend, k);
            for(; it != itend; ++it){
                if(rowMarkowitzs[it.getData()] < (int)bestMarkowitz){
                    bestMarkowitz = rowMarkowitzs[it.getData()];
                    rowindex = it.getData();
                    columnindex = rowMarkowitzColumnIndices[it.getData()];
                    pivotFound = true;
                    if(bestMarkowitz < (k-1)*(k-1)){
                        return;
                    }
                }
                visitedVectorCount++;
                if(pivotFound && visitedVectorCount >= markowitzSearchLimit){
                    return;
                }
            }
        }
    }
    LPERROR("No acceptable pivot element found during inversion!");
    throw PanOptException("No acceptable pivot element found during inversion!");
}

void LuBasis::append(const SparseVector &vector, int pivotRow, int incoming, Simplex::VARIABLE_STATE outgoingState)
{
    int outgoing = (*m_basisHead)[pivotRow];
    const Variable & outgoingVariable = m_model->getVariable(outgoing);

    if (outgoingState == Simplex::NONBASIC_AT_LB) {
        if(!Numerical::equal(*(m_variableStates->getAttachedData(outgoing)), outgoingVariable.getLowerBound(),1.0e-4)){
#ifndef NDEBUG
            LPERROR("Outgoing variable is rounded to its lower bound!");
            LPERROR("Current value: " << setw(19) << scientific << setprecision(16) << *(m_variableStates->getAttachedData(outgoing)));
            LPERROR("Lower bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getLowerBound());
            LPERROR("Upper bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getUpperBound());
#endif
            cerr.unsetf(ios_base::floatfield);
        }
        m_updateETMs->emplace_back(createEta(vector, pivotRow), pivotRow);
        m_inverseNonzeros += m_updateETMs->back().eta->nonZeros();
        m_variableStates->move(outgoing,Simplex::NONBASIC_AT_LB, &(outgoingVariable.getLowerBound()));
    } else if (outgoingState == Simplex::NONBASIC_AT_UB) {
        if(!Numerical::equal(*(m_variableStates->getAttachedData(outgoing)), outgoingVariable.getUpperBound(),1.0e-4)){
#ifndef NDEBUG
            LPERROR("Outgoing variable is rounded to its upper bound!");
            LPERROR("Current value: " << setw(19) << scientific << setprecision(16) << *(m_variableStates->getAttachedData(outgoing)));
            LPERROR("Lower bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getLowerBound());
            LPERROR("Upper bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getUpperBound());
#endif
            cerr.unsetf(ios_base::floatfield);
        }
        m_updateETMs->emplace_back(createEta(vector, pivotRow), pivotRow);
        m_inverseNonzeros += m_updateETMs->back().eta->nonZeros();
        m_variableStates->move(outgoing,Simplex::NONBASIC_AT_UB, &(outgoingVariable.getUpperBound()));
    } else if ( outgoingState == Simplex::NONBASIC_FIXED) {
        if(!Numerical::equal(*(m_variableStates->getAttachedData(outgoing)), outgoingVariable.getLowerBound(),1.0e-4)){
#ifndef NDEBUG
            LPERROR("Outgoing fixed variable is rounded to its bound!");
            LPERROR("Current value: " << setw(19) << scientific << setprecision(16) << *(m_variableStates->getAttachedData(outgoing)));
            LPERROR("Bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getLowerBound());
#endif
            cerr.unsetf(ios_base::floatfield);
        }
        m_updateETMs->emplace_back(createEta(vector, pivotRow), pivotRow);
        m_inverseNonzeros += m_updateETMs->back().eta->nonZeros();
        m_variableStates->move(outgoing,Simplex::NONBASIC_FIXED, &(outgoingVariable.getLowerBound()));
    } else {
#ifndef NDEBUG
        LPERROR("Invalid outgoing variable state!");
#endif
        cerr.unsetf(ios_base::floatfield);
        throw NumericalException(std::string("Invalid outgoing variable state"));
    }
    (*m_basisHead)[pivotRow] = incoming;
    m_isFresh = false;
}

void LuBasis::Ftran(DenseVector &vector, FTRAN_MODE mode) const
{
    __UNUSED(mode);
    //FTRANL
    unsigned int basisSize = m_basisHead->size();

    for (unsigned int i = 0; i < basisSize; i++) {
        int rowindex = m_pivots[i].rowindex;
        int columnindex = m_pivots[i].columnindex;
        const Numerical::Double pivotValue = vector.at(rowindex);
        if (pivotValue == 0.0) {
            continue;
        }
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = (*m_lower)[columnindex].column->at(rowindex);
        //Add the vector
        vector.addVector(pivotValue, *((*m_lower)[columnindex].column));
        //Set the pivot position
        vector.set(rowindex, pivotValue * pivotMultiplier);
    }

    //FTRANU
    SparseVector alpha(vector.length());
    alpha.clear();
    for (int i = (int)basisSize-1; i >= 0; i--) {

        int rowindex = m_pivots[i].rowindex;
        int columnindex = m_pivots[i].columnindex;
        const Numerical::Double pivotValue = vector.at(rowindex);
        if (pivotValue == 0.0) {
            continue;
        }
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = (*m_upper)[columnindex].column->at(rowindex);
        const Numerical::Double newPivotValue = pivotValue / pivotMultiplier;
        //Add the vector
        vector.addVector(- newPivotValue, *((*m_upper)[columnindex].column));
        //Set the pivot position
        alpha.setNewNonzero(columnindex, newPivotValue);
    }
    vector.clear();
    SparseVector::NonzeroIterator alphaIt = alpha.beginNonzero();
    SparseVector::NonzeroIterator alphaItend = alpha.endNonzero();
    for(; alphaIt != alphaItend; ++alphaIt){
        vector.set(alphaIt.getIndex(), *alphaIt);
    }

    //PFI UPDATE
    std::vector<ETM>::const_iterator it = m_updateETMs->begin();
    std::vector<ETM>::const_iterator itend = m_updateETMs->end();

    for (; it != itend; ++it){
        int rowindex = it->index;
        const Numerical::Double pivotValue = vector.at(rowindex);
        if(pivotValue == 0){
            continue;
        }
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = it->eta->at(rowindex);
        //Add the vector
        vector.addVector(pivotValue, *(it->eta));
        //Set the pivot position
        vector.set(rowindex, pivotValue * pivotMultiplier);
    }
}

void LuBasis::Ftran(SparseVector &vector, FTRAN_MODE mode) const
{
    __UNUSED(mode);
    //FTRANL
    unsigned int basisSize = m_basisHead->size();

    for (unsigned int i = 0; i < basisSize; i++) {
        int rowindex = m_pivots[i].rowindex;
        int columnindex = m_pivots[i].columnindex;
        const Numerical::Double pivotValue = vector.at(rowindex);
        if (pivotValue == 0.0) {
            continue;
        }
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = (*m_lower)[columnindex].column->at(rowindex);
        //Add the vector
        vector.addVector(pivotValue, *((*m_lower)[columnindex].column));
        //Set the pivot position
        vector.set(rowindex, pivotValue * pivotMultiplier);
    }

    //FTRANU
    SparseVector alpha(vector.length());
    alpha.clear();
    for (int i = (int)basisSize-1; i >= 0; i--) {

        int rowindex = m_pivots[i].rowindex;
        int columnindex = m_pivots[i].columnindex;
        const Numerical::Double pivotValue = vector.at(rowindex);
        if (pivotValue == 0.0) {
            continue;
        }
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = (*m_upper)[columnindex].column->at(rowindex);
        const Numerical::Double newPivotValue = pivotValue / pivotMultiplier;
        //Add the vector
        vector.addVector(- newPivotValue, *((*m_upper)[columnindex].column));
        //Set the pivot position
        alpha.setNewNonzero(columnindex, newPivotValue);
    }
    vector.clear();
    SparseVector::NonzeroIterator alphaIt = alpha.beginNonzero();
    SparseVector::NonzeroIterator alphaItend = alpha.endNonzero();
    for(; alphaIt != alphaItend; ++alphaIt){
        vector.set(alphaIt.getIndex(), *alphaIt);
    }

    //PFI UPDATE
    std::vector<ETM>::const_iterator it = m_updateETMs->begin();
    std::vector<ETM>::const_iterator itend = m_updateETMs->end();

    for (; it != itend; ++it){
        int rowindex = it->index;
        const Numerical::Double pivotValue = vector.at(rowindex);
        if(pivotValue == 0){
            continue;
        }
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = it->eta->at(rowindex);
        //Add the vector
        vector.addVector(pivotValue, *(it->eta));
        //Set the pivot position
        vector.set(rowindex, pivotValue * pivotMultiplier);
    }
}

void LuBasis::Btran(DenseVector &vector, BTRAN_MODE mode) const
{
    __UNUSED(mode);

    //PFI UPDATE
    std::vector<ETM>::const_reverse_iterator it = m_updateETMs->rbegin();
    std::vector<ETM>::const_reverse_iterator itend = m_updateETMs->rend();

    Numerical::Summarizer summarizer;
    for (; it != itend; ++it){
        int rowindex = it->index;
        SparseVector::NonzeroIterator etaIt = it->eta->beginNonzero();
        SparseVector::NonzeroIterator etaItend = it->eta->endNonzero();
        summarizer.clear();
        for(; etaIt != etaItend; ++etaIt){
            summarizer.add(*etaIt * vector.at(etaIt.getIndex()));
        }
        vector.set(rowindex, summarizer.getResult());
    }

    //BTRANU
    unsigned int basisSize = m_basisHead->size();
    SparseVector alpha(vector.length());
    alpha.clear();
    for (unsigned int i = 0; i < basisSize; i++) {
        int rowindex = m_pivots[i].rowindex;
        int columnindex = m_pivots[i].columnindex;
        const Numerical::Double pivotValue = vector.at(columnindex);
        if (pivotValue == 0.0) {
            continue;
        }
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = (*m_upper)[rowindex].row->at(columnindex);
        const Numerical::Double newPivotValue = pivotValue / pivotMultiplier;
        //Add the vector
        vector.addVector(- newPivotValue, *((*m_upper)[rowindex].row));
        //Set the pivot position
        alpha.setNewNonzero(rowindex, newPivotValue);
    }

    vector.clear();
    SparseVector::NonzeroIterator alphaIt = alpha.beginNonzero();
    SparseVector::NonzeroIterator alphaItend = alpha.endNonzero();
    for(; alphaIt != alphaItend; ++alphaIt){
        vector.set(alphaIt.getIndex(), *alphaIt);
    }

    //BTRANL
    //Sparsity expoiting BTRAN
    std::vector<Numerical::Summarizer> summarizerVector(vector.length());
    for (int i = (int)basisSize-1; i >= 0; i--) {
        int rowindex = m_pivots[i].rowindex;
        int columnindex = m_pivots[i].columnindex;
        const Numerical::Double pivotValue = vector.at(rowindex);
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = (*m_lower)[rowindex].row->at(columnindex);
        const Numerical::Double newPivotValue = Numerical::stableAdd(summarizerVector[columnindex].getResult(), pivotValue * pivotMultiplier);
        //Add the vector
        if(newPivotValue != 0.0){
            SparseVector::NonzeroIterator it = (*m_lower)[rowindex].row->beginNonzero();
            SparseVector::NonzeroIterator itend = (*m_lower)[rowindex].row->endNonzero();
            for(; it != itend; ++it){
                summarizerVector[it.getIndex()].add(newPivotValue * *it);
            }
        }
        //Set the pivot position
        vector.set(rowindex, newPivotValue);
    }

    //Standard BTRAN method
//    for (int i = (int)basisSize-1; i >= 0; i--) {
//        int rowindex = m_pivots.at(i).rowindex;
//        int columnindex = m_pivots.at(i).columnindex;
//        Numerical::Summarizer summarizer;
//        Vector::NonzeroIterator it = m_lower->at(columnindex).column->beginNonzero();
//        Vector::NonzeroIterator itend = m_lower->at(columnindex).column->endNonzero();
//        for(; it != itend; ++it){
//            summarizer.add(*it * vector.at(it.getIndex()));
//        }
//        vector.set(rowindex, summarizer.getResult());
//    }

}

void LuBasis::Btran(SparseVector &vector, BTRAN_MODE mode) const
{
    __UNUSED(mode);

    //PFI UPDATE
    std::vector<ETM>::const_reverse_iterator it = m_updateETMs->rbegin();
    std::vector<ETM>::const_reverse_iterator itend = m_updateETMs->rend();

    Numerical::Summarizer summarizer;
    for (; it != itend; ++it){
        int rowindex = it->index;
        SparseVector::NonzeroIterator etaIt = it->eta->beginNonzero();
        SparseVector::NonzeroIterator etaItend = it->eta->endNonzero();
        summarizer.clear();
        for(; etaIt != etaItend; ++etaIt){
            summarizer.add(*etaIt * vector.at(etaIt.getIndex()));
        }
        vector.set(rowindex, summarizer.getResult());
    }

    //BTRANU
    unsigned int basisSize = m_basisHead->size();
    SparseVector alpha(vector.length());
    alpha.clear();
    for (unsigned int i = 0; i < basisSize; i++) {
        int rowindex = m_pivots[i].rowindex;
        int columnindex = m_pivots[i].columnindex;
        const Numerical::Double pivotValue = vector.at(columnindex);
        if (pivotValue == 0.0) {
            continue;
        }
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = (*m_upper)[rowindex].row->at(columnindex);
        const Numerical::Double newPivotValue = pivotValue / pivotMultiplier;
        //Add the vector
        vector.addVector(- newPivotValue, *((*m_upper)[rowindex].row));
        //Set the pivot position
        alpha.setNewNonzero(rowindex, newPivotValue);
    }

    vector.clear();
    SparseVector::NonzeroIterator alphaIt = alpha.beginNonzero();
    SparseVector::NonzeroIterator alphaItend = alpha.endNonzero();
    for(; alphaIt != alphaItend; ++alphaIt){
        vector.set(alphaIt.getIndex(), *alphaIt);
    }

    //BTRANL
    //Sparsity expoiting BTRAN
    std::vector<Numerical::Summarizer> summarizerVector(vector.length());
    for (int i = (int)basisSize-1; i >= 0; i--) {
        int rowindex = m_pivots[i].rowindex;
        int columnindex = m_pivots[i].columnindex;
        const Numerical::Double pivotValue = vector.at(rowindex);
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = (*m_lower)[rowindex].row->at(columnindex);
        const Numerical::Double newPivotValue = Numerical::stableAdd(summarizerVector[columnindex].getResult(), pivotValue * pivotMultiplier);
        //Add the vector
        if(newPivotValue != 0.0){
            SparseVector::NonzeroIterator it = (*m_lower)[rowindex].row->beginNonzero();
            SparseVector::NonzeroIterator itend = (*m_lower)[rowindex].row->endNonzero();
            for(; it != itend; ++it){
                summarizerVector[it.getIndex()].add(newPivotValue * *it);
            }
        }
        //Set the pivot position
        vector.set(rowindex, newPivotValue);
    }

    //Standard BTRAN method
//    for (int i = (int)basisSize-1; i >= 0; i--) {
//        int rowindex = m_pivots.at(i).rowindex;
//        int columnindex = m_pivots.at(i).columnindex;
//        Numerical::Summarizer summarizer;
//        Vector::NonzeroIterator it = m_lower->at(columnindex).column->beginNonzero();
//        Vector::NonzeroIterator itend = m_lower->at(columnindex).column->endNonzero();
//        for(; it != itend; ++it){
//            summarizer.add(*it * vector.at(it.getIndex()));
//        }
//        vector.set(rowindex, summarizer.getResult());
//    }

}

void LuBasis::FtranCheck(DenseVector &vector, Basis::FTRAN_MODE mode) const
{
__UNUSED(vector);
__UNUSED(mode);
}

void LuBasis::BtranCheck(DenseVector &vector) const
{
__UNUSED(vector);
}

void LuBasis::analyzeStability() const
{

}

void LuBasis::checkSingularity() {
    if(m_factorizedPart < m_basisHead->size()){
        LPINFO("Singular basis!");
        DEVINFO(D::PFIMAKER, "Checking singularity");
        //TODO: Handle singularity, manage the lower and upper parts
        int singularity = 0;
        for (std::vector<int>::iterator it = m_basisNewHead.begin(); it < m_basisNewHead.end(); ++it) {
            if (*it == -1) {
                DEVINFO(D::PFIMAKER, "Given basis column " << it - m_basisNewHead.begin() << " is singular, replacing with unit vector");
                *it = it - m_basisNewHead.begin() + m_model->getColumnCount();
                singularity++;
            }
        }
        if (singularity != 0) {
            LPERROR("Singular basis is not handled in the LU yet");
            m_singularityCounter += singularity;
            LPWARNING("The given basis is singular, the measure of singularity is " << singularity);
        }
    }
}

void LuBasis::printStatistics() const {
    LPINFO("INVERSION FINISHED");
    LPINFO("Run parameters during the inversion:");

    LPINFO("Nonzero statistics: ");
    LPINFO("Nonzeros in the basis: " << m_basisNonzeros);
    LPINFO("Nonzeros in the inverse: " << m_inverseNonzeros);
    LPINFO("Fill in amount: " << ((Numerical::Double)m_inverseNonzeros / (Numerical::Double)m_basisNonzeros)*100.0 - 100 << "% )");
}

void LuBasis::printTransformationStatistics() const {
    LPINFO("Inversion count: "<<m_inversionCount);
    //		LPINFO("Transformation count in the last inversion: "<<m_transformationCount);
    LPINFO("Transformation count average: "<<m_transformationAverage);
    LPINFO("Average size of the non-triangular part:"<<m_mNumAverage);
    printStatistics();
}
