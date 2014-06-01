/**
 * @file lubasis.cpp
 */

#include <string>

#include <simplex/lubasis.h>
#include <simplex/simplex.h>

#include <simplex/simplexparameterhandler.h>

#include <utils/thirdparty/prettyprint.h>

THREAD_STATIC_DEF int LuBasis::m_inversionCount = 0;

LuBasis::LuBasis(const SimplexModel& model,
                 std::vector<int>* basisHead,
                 IndexList<const Numerical::Double*>* variableStates,
                 const Vector& basicVariableValues) :
    Basis(model, basisHead, variableStates, basicVariableValues),
    m_threshold(SimplexParameterHandler::getInstance().getDoubleParameterValue("pivot_threshold"))
{
    m_lower = new std::vector<DoubleETM>();
    m_upper = new std::vector<DoubleETM>();

    m_transformationCount = 0;
    m_transformationAverage = 0;
    m_mNumAverage = 0;

    m_columnCountIndexList.init(0,0);
    m_rowCountIndexList.init(0,0);
}

LuBasis::~LuBasis()
{
    for (std::vector<DoubleETM>::iterator it = m_lower->begin(); it < m_lower->end(); it++) {
        if(it->row != NULL){
            delete it->row;
        }
        if(it->column != NULL){
            delete it->column;
        }

    }
    delete m_lower;

    for (std::vector<DoubleETM>::iterator it = m_upper->begin(); it < m_upper->end(); it++) {
        if(it->row != NULL){
            delete it->row;
        }
        if(it->column != NULL){
            delete it->column;
        }

    }
    delete m_upper;
}

void LuBasis::invert()
{
    m_transformationCount = 0;
    m_inversionCount++;

    if(m_lower->size() == 0){
        m_lower->resize(m_basisHead->size(),DoubleETM());
        m_upper->resize(m_basisHead->size(),DoubleETM());
    } else {
        for (std::vector<DoubleETM>::iterator it = m_lower->begin(); it < m_lower->end(); it++) {
            delete it->row;
            it->row = NULL;
            delete it->column;
            it->column = NULL;
        }

        for (std::vector<DoubleETM>::iterator it = m_upper->begin(); it < m_upper->end(); it++) {
            delete it->row;
            it->row = NULL;
            delete it->column;
            it->column = NULL;
        }
    }

    m_pivots.clear();
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
//    LPINFO("basicrows: ");
//    for(int i = 0; i<m_basisHead->size(); i++){
//        for(int j = 0; j<m_basisHead->size(); j++){
//            std::cout<<m_basicRows.at(i)->at(j)<<" ";
//        }
//        std::cout<<"\n";
//    }

    //Invert the R, M, C parts separately
    invertC();
    invertR();
    invertM();

    //Free the copied columns
    for(unsigned int i=0; i<m_basicColumnCopies.size(); i++){
        if(m_basicColumnCopies.at(i) != NULL){
            delete m_basicColumnCopies.at(i);
            m_basicColumnCopies.at(i) = NULL;
        }
    }

    //Free the basic rows
    for(unsigned int i=0; i<m_basicRows.size(); i++){
        if(m_basicRows.at(i) != NULL){
            delete m_basicRows.at(i);
            m_basicRows.at(i) = NULL;
        }
    }

    //Verify the result of the inversion

    checkSingularity();

    int basisSize = m_basisHead->size();
    //Create the rowwise representation of L
    for(int i=0; i<basisSize; i++){
        Vector::NonzeroIterator it = m_lower->at(i).column->beginNonzero();
        Vector::NonzeroIterator itend = m_lower->at(i).column->endNonzero();
        for(; it != itend; it++){
            m_lower->at(it.getIndex()).row->setNewNonzero(i, *it);
        }
    }
    //Create the columnwise representation of U
    for(int i=0; i<basisSize; i++){
        Vector::NonzeroIterator it = m_upper->at(i).row->beginNonzero();
        Vector::NonzeroIterator itend = m_upper->at(i).row->endNonzero();
        for(; it != itend; it++){
            m_upper->at(it.getIndex()).column->setNewNonzero(i, *it);
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
    unsigned int columnCount = m_model.getColumnCount();
    unsigned int rowCount = m_model.getRowCount();

    //Containers to be resized directly
    //NEW//
    m_basicColumns.clear();
    m_basicColumns.reserve(rowCount);
    m_basicColumnCopies.resize(rowCount, NULL);
    m_basicRows.resize(rowCount, NULL);
    //NEW//

    m_basisNewHead.resize(rowCount, -1);

#ifndef NDEBUG
    std::vector<char> headChecker(rowCount + columnCount, 0);
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); it++) {
        if (headChecker.at(*it) == 0) {
            headChecker.at(*it) = 1;
        } else {
            //TODO: Ennek az esetnek a kezelese vmiert nem jo
            LPWARNING("Duplicate index in basis head: " << *it);
        }
    }
#endif

    //Copy the active submatrix
    unsigned int maxColumnCount = 0;
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); it++) {
        if (*it >= (int) columnCount) {
            DEVINFO(D::PFIMAKER, "Logical variable found in basis head: y" << *it - columnCount);
            //Ignore logical columns from the inverse
            int basisIndex = it-m_basisHead->begin();
            m_basicColumnCopies.at(basisIndex) = new Vector(rowCount);
            m_basicColumnCopies.at(basisIndex)->setNewNonzero(*it - columnCount, 1);
            m_basicColumns.push_back(m_basicColumnCopies.at(basisIndex));
            if(maxColumnCount < 1){
                maxColumnCount = 1;
            }
        } else {
            DEVINFO(D::PFIMAKER, "Structural variable found in basis head: x" << *it);
            //The submatrix is the active submatrix needed for inversion
            m_basicColumns.push_back(&(m_model.getMatrix().column(*it)));
            //IGNORE//
            if (maxColumnCount < m_basicColumns.back()->nonZeros()) {
                maxColumnCount = m_basicColumns.back()->nonZeros();
            }
            m_basisNonzeros += m_basicColumns.back()->nonZeros();
        }
        m_basicRows.at(it - m_basisHead->begin()) = new Vector(rowCount);
    }
    //Set up row counts, column counts (r_i, c_i) and the corresponding row lists
    for (std::vector<const Vector*>::iterator it = m_basicColumns.begin(); it < m_basicColumns.end(); it++) {
        int columnIndex = it - m_basicColumns.begin();
        Vector::NonzeroIterator vectorIt = (*it)->beginNonzero();
        Vector::NonzeroIterator vectorItEnd = (*it)->endNonzero();
        for (; vectorIt < vectorItEnd; vectorIt++) {
            int rowIndex = vectorIt.getIndex();
            m_basicRows.at(rowIndex)->setNewNonzero(columnIndex, *vectorIt);
        }
    }
    unsigned int maxRowCount = 0;
    for (unsigned int i = 0; i < m_basicRows.size(); i++) {
        if (maxRowCount < m_basicRows.at(i)->nonZeros()) {
            maxRowCount = m_basicRows.at(i)->nonZeros();
        }
    }

    //TODO: Use smaller lists!
    //TODO: Use previously allocated memory pieces
    m_rowCountIndexList.init(m_basisHead->size(), m_basicColumns.size()+1);
    m_columnCountIndexList.init(m_basisHead->size(), m_basicColumns.size()+1);

    for (unsigned int i = 0; i < m_basicRows.size(); i++) {
        m_rowCountIndexList.insert(m_basicRows.at(i)->nonZeros(), i);
    }

    for (unsigned int i = 0; i < m_basicColumns.size(); i++) {
        m_columnCountIndexList.insert(m_basicColumns.at(i)->nonZeros(), i);
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
            const Vector * currentColumn = m_basicColumns.at(columnindex);
            //Get the active nonzero from the column
            Vector::NonzeroIterator it = currentColumn->beginNonzero();
            Vector::NonzeroIterator itend = currentColumn->endNonzero();
            unsigned int rowPartitionCount = m_rowCountIndexList.getPartitionCount();
            int count = 0;
            for(; it != itend; it++){
                if(m_rowCountIndexList.where(it.getIndex()) < rowPartitionCount){
                    rowindex = it.getIndex();
                    count++;
                }
            }
            if(count != 1){
                LPERROR("COUNT ERROR");
                exit(-1);
            }
            const Vector * currentRow = m_basicRows.at(rowindex);

            int basisSize = m_basisHead->size();
            Vector* unitColumn = new Vector(basisSize);
            unitColumn->setNewNonzero(rowindex, 1.0);
            m_lower->at(columnindex) = DoubleETM(new Vector(basisSize), unitColumn);
            m_upper->at(rowindex) = DoubleETM(new Vector(*currentRow), new Vector(basisSize));
            m_pivots.emplace_back(rowindex, columnindex);

//            Vector* unitColumn = new Vector(basisSize);
//            unitColumn->setNewNonzero(m_factorizedPart, 1.0);
//            m_lower->at(m_factorizedPart) = DoubleETM(new Vector(basisSize), unitColumn);
//            m_upper->at(m_factorizedPart) = DoubleETM(new Vector(*currentRow), new Vector(basisSize));
//            m_factorizedPart++;
//            m_pivots.emplace_back(rowindex, columnindex);

            m_basisNewHead.at(columnindex) = m_basisHead->at(columnindex);

            //Update the column lists and column counts
            it = currentRow->beginNonzero();
            itend = currentRow->endNonzero();
            unsigned int columnPartitionCount = m_columnCountIndexList.getPartitionCount();
            for (; it < itend; it++) {
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
        }
    }
    DEVINFO(D::PFIMAKER, "CPART num: " << cNum);
    LPINFO( "CPART num: " << cNum);
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
            const Vector * currentRow = m_basicRows.at(rowindex);
            //Get the active nonzero from the column
            Vector::NonzeroIterator it = currentRow->beginNonzero();
            Vector::NonzeroIterator itend = currentRow->endNonzero();
            unsigned int columnPartitionCount = m_columnCountIndexList.getPartitionCount();
            for(; it != itend; it++){
                if(m_columnCountIndexList.where(it.getIndex()) < columnPartitionCount){
                    columnindex = it.getIndex();
                }
            }

//            LPINFO("PIVOT ELEMENT: rowindex: "<<rowindex<< " - columnindex: "<<columnindex);
            //Create the vector to be transformed into an eta vector
            const Vector * currentColumn = m_basicColumns.at(columnindex);
            Vector transformVector (currentColumn->length());
            it = currentColumn->beginNonzero();
            itend = currentColumn->endNonzero();
            unsigned int rowPartitionCount = m_rowCountIndexList.getPartitionCount();
            for(;it != itend; it++){
                if(m_rowCountIndexList.where(it.getIndex()) < rowPartitionCount){
                    transformVector.setNewNonzero(it.getIndex(),*it);
                }
            }

            int basisSize = m_basisHead->size();
            Vector* unitRow = new Vector(basisSize);
            unitRow->setNewNonzero(columnindex, 1.0);
            m_lower->at(columnindex) = DoubleETM(new Vector(basisSize), createEta(transformVector,rowindex));
            m_upper->at(rowindex) = DoubleETM(unitRow, new Vector(basisSize));
            m_pivots.emplace_back(rowindex, columnindex);
            m_basisNewHead.at(columnindex) = m_basisHead->at(columnindex);

            //Update the row lists and row counts
            it = transformVector.beginNonzero();
            itend = transformVector.endNonzero();
            for (; it < itend; it++) {
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
//            LPINFO("R PIVOT: "<<rowindex << " - "<<columnindex);
        }
    }
    DEVINFO(D::PFIMAKER, "RPART num: " << rNum);
    LPINFO( "RPART num: " << rNum);
}

void LuBasis::invertM()
{
    DEVINFO(D::PFIMAKER, "Search for the M part and invert it");
    unsigned int mNum = 0;

    //TODO skip everything if the basis is triangular
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
            Vector* kernelRow = new Vector(basisSize);
            Vector::NonzeroIterator it = m_basicRows.at(rowIndex)->beginNonzero();
            Vector::NonzeroIterator itend = m_basicRows.at(rowIndex)->endNonzero();
            for(; it != itend; it++){
                if(m_columnCountIndexList.where(it.getIndex()) < columnPartitionCount){
                    kernelRow->setNewNonzero(it.getIndex(),*it);
                    if(rowAbsMaxs.at(rowIndex) < Numerical::fabs(*it)){
                        rowAbsMaxs.at(rowIndex) = Numerical::fabs(*it);
                    }
                }
            }
            delete m_basicRows.at(rowIndex);
            m_basicRows.at(rowIndex) = kernelRow;
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
        //Compute the row and column Markowitz numbers
        std::vector<int> rowMarkowitzs;
        std::vector<int> rowMarkowitzColumnIndices;
        rowMarkowitzs.resize(basisSize,basisSize*basisSize);
        rowMarkowitzColumnIndices.resize(basisSize);
        std::vector<int> columnMarkowitzs;
        std::vector<int> columnMarkowitzRowIndices;
        columnMarkowitzs.resize(basisSize,basisSize*basisSize);
        columnMarkowitzRowIndices.resize(basisSize);
        const double & pivotThreshold = SimplexParameterHandler::getInstance().getDoubleParameterValue("pivot_threshold");
        std::list<int>::iterator activeRowIt;
        std::list<int>::iterator activeRowItend;
        activeRowIt = activeRows.begin();
        activeRowItend = activeRows.end();
        for(; activeRowIt != activeRowItend; activeRowIt++){
            int currentMarkowitz;
            Vector::NonzeroIterator it = m_basicRows.at(*activeRowIt)->beginNonzero();
            Vector::NonzeroIterator itend = m_basicRows.at(*activeRowIt)->endNonzero();
            for(; it != itend; it++){
                //If the column is singleton, set the only possible choice
                if(m_columnCountIndexList.where(it.getIndex()) == 1){
                    columnMarkowitzs.at(it.getIndex()) = 0;
                    columnMarkowitzRowIndices.at(it.getIndex()) = *activeRowIt;
                } else
                //Check the threshold pivot criterion
                if(Numerical::fabs(*it) >= rowAbsMaxs.at(*activeRowIt)*pivotThreshold){
                    //Update the Markowitz numbers
                    currentMarkowitz = (m_rowCountIndexList.where(*activeRowIt)-1) * (m_columnCountIndexList.where(it.getIndex())-1);
                    if(currentMarkowitz < rowMarkowitzs.at(*activeRowIt)){
                        rowMarkowitzs.at(*activeRowIt) = currentMarkowitz;
                        rowMarkowitzColumnIndices.at(*activeRowIt) = it.getIndex();
                    }
                    if(currentMarkowitz < columnMarkowitzs.at(it.getIndex())){
                        columnMarkowitzs.at(it.getIndex()) = currentMarkowitz;
                        columnMarkowitzRowIndices.at(it.getIndex()) = *activeRowIt;
                    }
                }
            }
        }

        //Procedure find pivot
        int rowindex;
        int columnindex;
        findPivot(rowindex, columnindex, rowMarkowitzs, columnMarkowitzs, rowMarkowitzColumnIndices, columnMarkowitzRowIndices);
        //pivotU first
        Vector * pivotRow = m_basicRows.at(rowindex);
        m_inverseNonzeros += pivotRow->nonZeros();
        m_upper->at(rowindex) = DoubleETM(new Vector(*pivotRow), new Vector(basisSize));
        //Compute the eta column for pivotL and do the elimination
        Vector * etaColumn = new Vector(basisSize);
        //Set the unit element in the eta column
        etaColumn->setNewNonzero(rowindex, 1);
        //Scan the active rows, compute the eta column and do the updates
        activeRowIt = activeRows.begin();
        activeRowItend = activeRows.end();
        //Scatter currentRow here
        pivotRow->setSparsityRatio(DENSE);
        Numerical::Double pivotElement = pivotRow->at(columnindex);
//        LPINFO("PIVOT ELEMENT: rowindex: "<<rowindex<< " - columnindex: "<<columnindex);
        for(; activeRowIt != activeRowItend;){
            //Skip the pivot row from updating and reduce the kernel
            if(*activeRowIt == rowindex){
                //Reduce the kernel here
                Vector::NonzeroIterator rowIt = pivotRow->beginNonzero();
                Vector::NonzeroIterator rowItend = pivotRow->endNonzero();
                for(; rowIt != rowItend; rowIt++){
                    m_columnCountIndexList.move(rowIt.getIndex(),m_columnCountIndexList.where(rowIt.getIndex())-1);
                }
                activeRows.erase(activeRowIt++);
                m_rowCountIndexList.remove(rowindex);
            } else {
                Vector * currentRow = m_basicRows.at(*activeRowIt);
                Numerical::Double pivotColumnValue = currentRow->at(columnindex);
                //If the row should be updated
                if(pivotColumnValue != 0){
                    //Remove the column counts on the active row
                    Vector::NonzeroIterator rowIt = currentRow->beginNonzero();
                    Vector::NonzeroIterator rowItend = currentRow->endNonzero();
                    for(; rowIt != rowItend; rowIt++){
                        m_columnCountIndexList.move(rowIt.getIndex(),m_columnCountIndexList.where(rowIt.getIndex())-1);
                    }
                    //Compute the eta multiplier, store it in the eta column and update the row
                    Numerical::Double eta = - pivotColumnValue / pivotElement;
                    etaColumn->setNewNonzero(*activeRowIt, eta);
                    currentRow->addVector(eta, *pivotRow);

                    //Add the column counts on the updated row and update the max element
                    rowAbsMaxs.at(*activeRowIt) = 0.0;
                    rowIt = currentRow->beginNonzero();
                    rowItend = currentRow->endNonzero();
                    for(; rowIt != rowItend; rowIt++){
                        m_columnCountIndexList.move(rowIt.getIndex(),m_columnCountIndexList.where(rowIt.getIndex())+1);
                        if(rowAbsMaxs.at(*activeRowIt) < *rowIt){
                            rowAbsMaxs.at(*activeRowIt) = *rowIt;
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
        m_lower->at(columnindex) = DoubleETM(new Vector(basisSize), etaColumn);
        m_pivots.emplace_back(rowindex, columnindex);
        m_basisNewHead.at(columnindex) = m_basisHead->at(columnindex);

        mNum++;
    }
    DEVINFO(D::PFIMAKER, "MPART num: " << mNum);
    LPINFO( "MPART num: " << mNum);
}

void LuBasis::checkIndexListValidities(){
    unsigned int rowPartitionCount = m_rowCountIndexList.getPartitionCount();
    unsigned int columnPartitionCount = m_columnCountIndexList.getPartitionCount();
    for(int i=0; i<(int)rowPartitionCount; i++){
        IndexList<>::Iterator it;
        IndexList<>::Iterator itend;
        m_rowCountIndexList.getIterators(&it, &itend, i);
        for(; it != itend; it++){
            int rowindex = it.getData();
            int activeCounter = 0;
            Vector::NonzeroIterator rowit = m_basicRows.at(rowindex)->beginNonzero();
            Vector::NonzeroIterator rowitend = m_basicRows.at(rowindex)->endNonzero();
            for(; rowit != rowitend; rowit++){
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
        for(; it != itend; it++){
            int columnindex = it.getData();
            int activeCounter = 0;
            for(int j = 0; j < (int)m_basisHead->size(); j++){
                if(m_rowCountIndexList.where(j) < rowPartitionCount){
                    if(m_basicRows.at(j)->at(columnindex) != 0){
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
//        LPINFO("SR");
        rowindex = m_rowCountIndexList.firstElement(1);
        //some debug code, can be omitted later
        if(m_basicRows.at(rowindex)->nonZeros() != 1){
            LPERROR("False singleton row!");
        }
        columnindex = m_basicRows.at(rowindex)->beginNonzero().getIndex();
        return;
    }
    if(m_columnCountIndexList.firstElement(1) != -1){
//        LPINFO("SC");
        columnindex = m_columnCountIndexList.firstElement(1);
        rowindex = columnMarkowitzRowIndices.at(columnindex);
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
            for(; it != itend; it++){
                if(columnMarkowitzs.at(it.getData()) < (int)bestMarkowitz){
                    bestMarkowitz = columnMarkowitzs.at(it.getData());
                    columnindex = it.getData();
                    rowindex = columnMarkowitzRowIndices.at(it.getData());
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
            for(; it != itend; it++){
                if(rowMarkowitzs.at(it.getData()) < (int)bestMarkowitz){
                    bestMarkowitz = rowMarkowitzs.at(it.getData());
                    rowindex = it.getData();
                    columnindex = rowMarkowitzColumnIndices.at(it.getData());
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

//void LuBasis::pivotL(const Vector * column, int pivotRow)
//{
////    ETM newETM;
////    if(column != NULL){
////        newETM.eta = createEta(*column, pivotRow);
////        m_inverseNonzeros += newETM.eta->nonZeros();
////    } else {
////        newETM.eta  = NULL;
////    }
////    newETM.index = pivotRow;
////    m_lower->push_back(newETM);
//}

//void LuBasis::pivotU(const Vector * row, int pivotColumn)
//{
////    ETM newETM;
////    if(row != NULL){
////        newETM.eta = new Vector(*row);
////        m_inverseNonzeros += newETM.eta->nonZeros();
////    } else {
////        newETM.eta = NULL;
////    }
////    newETM.index = pivotColumn;
////    m_upper->push_back(newETM);
//}

void LuBasis::append(const Vector &vector, int pivotRow, int incoming, Simplex::VARIABLE_STATE outgoingState)
{
    int outgoing = m_basisHead->at(pivotRow);
    const Variable & outgoingVariable = m_model.getVariable(outgoing);

    if (outgoingState == Simplex::NONBASIC_AT_LB) {
        if(!Numerical::equal(*(m_variableStates->getAttachedData(outgoing)), outgoingVariable.getLowerBound(),1.0e-4)){
//#ifndef NDEBUG
            LPERROR("Outgoing variable is rounded to its lower bound!");
            LPERROR("Current value: " << setw(19) << scientific << setprecision(16) << *(m_variableStates->getAttachedData(outgoing)));
            LPERROR("Lower bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getLowerBound());
            LPERROR("Upper bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getUpperBound());
//#endif
            cerr.unsetf(ios_base::floatfield);
        }
//        pivot(vector, pivotRow);
        m_variableStates->move(outgoing,Simplex::NONBASIC_AT_LB, &(outgoingVariable.getLowerBound()));
    } else if (outgoingState == Simplex::NONBASIC_AT_UB) {
        if(!Numerical::equal(*(m_variableStates->getAttachedData(outgoing)), outgoingVariable.getUpperBound(),1.0e-4)){
//#ifndef NDEBUG
            LPERROR("Outgoing variable is rounded to its upper bound!");
            LPERROR("Current value: " << setw(19) << scientific << setprecision(16) << *(m_variableStates->getAttachedData(outgoing)));
            LPERROR("Lower bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getLowerBound());
            LPERROR("Upper bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getUpperBound());
//#endif
            cerr.unsetf(ios_base::floatfield);
        }
//        pivot(vector, pivotRow);
        m_variableStates->move(outgoing,Simplex::NONBASIC_AT_UB, &(outgoingVariable.getUpperBound()));
    } else if ( outgoingState == Simplex::NONBASIC_FIXED) {
        if(!Numerical::equal(*(m_variableStates->getAttachedData(outgoing)), outgoingVariable.getLowerBound(),1.0e-4)){
//#ifndef NDEBUG
            LPERROR("Outgoing fixed variable is rounded to its bound!");
            LPERROR("Current value: " << setw(19) << scientific << setprecision(16) << *(m_variableStates->getAttachedData(outgoing)));
            LPERROR("Bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getLowerBound());
//#endif
            cerr.unsetf(ios_base::floatfield);
        }
//        pivot(vector, pivotRow);
        m_variableStates->move(outgoing,Simplex::NONBASIC_FIXED, &(outgoingVariable.getLowerBound()));
    } else {
        LPERROR("Invalid outgoing variable state!");
        cerr.unsetf(ios_base::floatfield);
        throw NumericalException(std::string("Invalid outgoing variable state"));
    }
    m_basisHead->at(pivotRow) = incoming;
    m_isFresh = false;
}

void LuBasis::Ftran(Vector &vector, FTRAN_MODE mode) const
{
    __UNUSED(mode);
    //FTRANL
    unsigned int basisSize = m_basisHead->size();

    for (unsigned int i = 0; i < basisSize; i++) {
        int rowindex = m_pivots.at(i).rowindex;
        int columnindex = m_pivots.at(i).columnindex;
        const Numerical::Double pivotValue = vector.at(rowindex);
        if (pivotValue == 0.0) {
            continue;
        }
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = m_lower->at(columnindex).column->at(rowindex);
        //Add the vector
        vector.addVector(pivotValue, *(m_lower->at(columnindex).column));
        //Set the pivot position
        vector.set(rowindex, pivotValue * pivotMultiplier);
    }

    //FTRANU
    Vector alpha(vector.length());
    alpha.clear();
    for (int i = (int)basisSize-1; i >= 0; i--) {

        int rowindex = m_pivots.at(i).rowindex;
        int columnindex = m_pivots.at(i).columnindex;
        const Numerical::Double pivotValue = vector.at(rowindex);
        if (pivotValue == 0.0) {
            continue;
        }
//        LPINFO("CC: "<<*m_upper->at(columnindex).column);
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = m_upper->at(columnindex).column->at(rowindex);
        const Numerical::Double newPivotValue = pivotValue / pivotMultiplier;
        //Add the vector
        vector.addVector(- newPivotValue, *(m_upper->at(columnindex).column));
        //Set the pivot position
        alpha.set(columnindex, newPivotValue);
//        vector.set(rowindex, newPivotValue);
    }
    vector = alpha;
}

void LuBasis::Btran(Vector &vector, BTRAN_MODE mode) const
{
    __UNUSED(mode);
    //BTRANU
    unsigned int basisSize = m_basisHead->size();
    Vector copy(vector);
    Vector alpha(vector.length());
    alpha.clear();
    for (unsigned int i = 0; i < basisSize; i++) {
        int rowindex = m_pivots.at(i).rowindex;
        int columnindex = m_pivots.at(i).columnindex;
        const Numerical::Double pivotValue = vector.at(columnindex);
        if (pivotValue == 0.0) {
            continue;
        }
        //Get the pivot multiplier from the ETM
        const Numerical::Double pivotMultiplier = m_upper->at(rowindex).row->at(columnindex);
        const Numerical::Double newPivotValue = pivotValue / pivotMultiplier;
        //Add the vector
        vector.addVector(- newPivotValue, *(m_upper->at(rowindex).row));
        //Set the pivot position
        alpha.set(rowindex, newPivotValue);
//        vector.set(columnindex, newPivotValue);
    }

    vector = alpha;

//    for (unsigned int i = 0; i < basisSize; i++) {
//        int rowindex = m_pivots.at(i).rowindex;
//        int columnindex = m_pivots.at(i).columnindex;
//        const Numerical::Double pivotValue = vector.at(columnindex);
//        if (pivotValue == 0.0) {
//            continue;
//        }
//        //Get the pivot multiplier from the ETM
//        const Numerical::Double pivotMultiplier = m_upper->at(rowindex).row->at(columnindex);
//        const Numerical::Double newPivotValue = pivotValue / pivotMultiplier;
//        //Add the vector
//        vector.addVector(- newPivotValue, *(m_upper->at(rowindex).row));
//        //Set the pivot position
////        alpha.set(rowindex, newPivotValue);
//        vector.set(columnindex, newPivotValue);
//    }

    //BTRANL
    for (int i = (int)basisSize-1; i >= 0; i--) {
        int rowindex = m_pivots.at(i).rowindex;
        int columnindex = m_pivots.at(i).columnindex;
//        const Numerical::Double pivotValue = vector.at(columnindex);
//        if (pivotValue == 0.0) {
//            continue;
//        }
        Numerical::Summarizer summarizer;
        Vector::NonzeroIterator it = m_lower->at(columnindex).column->beginNonzero();
        Vector::NonzeroIterator itend = m_lower->at(columnindex).column->endNonzero();
        for(; it != itend; it++){
            summarizer.add(*it * vector.at(it.getIndex()));
        }
        //Get the pivot multiplier from the ETM
//        const Numerical::Double pivotMultiplier = m_lower->at(rowindex).row->at(columnindex);
        //Add the vector
//        vector.addVector(pivotValue, *(m_lower->at(rowindex).row));
        //Set the pivot position
//        vector.set(columnindex, pivotValue * pivotMultiplier);
        vector.set(rowindex, summarizer.getResult());
    }

}

void LuBasis::checkSingularity() {
    DEVINFO(D::PFIMAKER, "Checking singularity");
    //TODO: Handle singularity, manage the lower and upper parts
    int singularity = 0;
    for (std::vector<int>::iterator it = m_basisNewHead.begin(); it < m_basisNewHead.end(); it++) {
        if (*it == -1) {
            DEVINFO(D::PFIMAKER, "Given basis column " << it - m_basisNewHead.begin() << " is singular, replacing with unit vector");
            *it = it - m_basisNewHead.begin() + m_model.getColumnCount();
            singularity++;
        }
    }
    if (singularity != 0) {
        LPERROR("Singular basis is not handled in the LU yet");
        m_singularityCounter += singularity;
        LPWARNING("The given basis is singular, the measure of singularity is " << singularity);
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
