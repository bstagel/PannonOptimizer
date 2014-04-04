/**
 * @file pfibasis.cpp
 */

#include <string>

#include <simplex/pfibasis.h>
#include <simplex/simplex.h>

#include <simplex/simplexparameterhandler.h>

#include <utils/thirdparty/prettyprint.h>

THREAD_STATIC_DEF int PfiBasis::m_inversionCount = 0;

PfiBasis::PfiBasis(const SimplexModel& model,
                   std::vector<int>* basisHead,
                   IndexList<const Numerical::Double*>* variableStates,
                   const Vector& basicVariableValues) :
    Basis(model, basisHead, variableStates, basicVariableValues) {
    m_basis = new std::vector<ETM>();

    m_transformationCount = 0;
    m_transformationAverage = 0;
    m_mNumAverage = 0;

    m_cColumns = new std::vector<const Vector*>();
    m_cPivotIndexes = new std::vector<int>();

    m_mmRows = new std::vector<Vector>();
    m_mmRowIndices = new std::vector<int>();
    m_mmColumns = new std::vector<Vector>();
    m_mmColumnIndices = new std::vector<int>();
    m_mmGraphOut = new std::vector<std::vector<int> >();
    m_mmGraphIn = new std::vector<std::vector<int> >();
    m_stack = new std::vector<PathNode>();
    m_orderedIndex = new std::vector<char>();
    m_mmBlocks = new std::vector<int>();
    m_rowSwapHash = new std::vector<int>();
    m_columnSwapHash = new std::vector<int>();
    m_columnSwapLog = new std::vector<int>();

    nontriangularMethod = (NONTRIANGULAR_METHOD) SimplexParameterHandler::getInstance().getIntegerParameterValue("nontriangular_method");
    nontriangularPivotRule = (NONTRIANGULAR_PIVOT_RULE) SimplexParameterHandler::getInstance().getIntegerParameterValue("nontriangular_pivot_rule");
    threshold = SimplexParameterHandler::getInstance().getDoubleParameterValue("pivot_threshold");


    cl_inversion = 0;
    cl_copy = 0;
    cl_invertR = 0;
    cl_invertM = 0;
    cl_invertC = 0;
    cl_transversal = 0;
    cl_createBlockTriangular = 0;
    cl_pivot = 0;
    cl_updateColumns = 0;
}

PfiBasis::~PfiBasis() {
    delete m_mmRows;
    delete m_mmRowIndices;
    delete m_mmColumns;
    delete m_mmColumnIndices;
    delete m_mmGraphOut;
    delete m_mmGraphIn;
    delete m_stack;
    delete m_orderedIndex;
    delete m_mmBlocks;
    delete m_rowSwapHash;
    delete m_columnSwapHash;
    delete m_columnSwapLog;
    delete m_cColumns;
    delete m_cPivotIndexes;
    for (std::vector<ETM>::iterator iter = m_basis->begin(); iter < m_basis->end(); iter++) {
        delete iter->eta;
    }
    delete m_basis;
}

void PfiBasis::copyBasis(bool buildIndexLists) {
    DEVINFO(D::PFIMAKER, "Copy the basis");
    unsigned int columnCount = m_model.getColumnCount();
    unsigned int rowCount = m_model.getRowCount();

    //Reinit data structures
    m_columnCounts.clear();
    m_columnCountIndexList.init(0,0);
    m_rowCounts.clear();
    m_rowCountIndexList.init(0,0);

    //Containers to be resized directly
    //NEW//
    m_basicColumns.clear();
    m_basicColumns.reserve(rowCount);
    m_basicColumnCopies.resize(rowCount, NULL);
    m_basicColumnIndices.clear();
    m_basicColumnIndices.reserve(rowCount);
    m_basicNonzeroIndices.clear();
    m_basicNonzeroIndices.resize(rowCount);
    //NEW//

    m_columnCounts.reserve(rowCount);
    m_rowCounts.clear();
    m_rowCounts.resize(rowCount, 0);
    m_basisNewHead.resize(rowCount, -1);

#ifndef NDEBUG
    std::vector<char> headChecker(rowCount + columnCount, 0);
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); it++) {
        if (headChecker.at(*it) == 0) {
            headChecker.at(*it) = 1;
        } else {
            //TODO: throw exception here, DEBUG MODE ONLY
            //TODO: Ennek az esetnek a kezelese vmiert nem jo
            LPWARNING("Duplicate index in basis head: " << *it);
        }
    }
#endif

    //Copy the active submatrix
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); it++) {
        if (*it >= (int) columnCount) {
            DEVINFO(D::PFIMAKER, "Logical variable found in basis head: y" << *it - columnCount);
            //Ignore logical columns from the inverse
            m_rowCounts.at(*it-columnCount) = -1;
            m_basisNewHead.at(*it-columnCount) = *it;
        } else {
            DEVINFO(D::PFIMAKER, "Structural variable found in basis head: x" << *it);
            //The submatrix is the active submatrix needed for inversion
            m_basicColumns.push_back(&(m_model.getMatrix().column(*it)));
            m_basicColumnIndices.push_back(*it);
            //IGNORE//
            m_columnCounts.push_back(m_basicColumns.back()->nonZeros());
            m_basisNonzeros += m_basicColumns.back()->nonZeros();
        }
    }
    //Set up row counts, column counts (r_i, c_i) and the corresponding row lists
    int maxRowCount = 0;
    int maxColumnCount = 0;
    for (std::vector<const Vector*>::iterator it = m_basicColumns.begin(); it < m_basicColumns.end(); it++) {
        int columnIndex = it - m_basicColumns.begin();
        Vector::NonzeroIterator vectorIt = (*it)->beginNonzero();
        Vector::NonzeroIterator vectorItEnd = (*it)->endNonzero();
        for (; vectorIt < vectorItEnd; vectorIt++) {
            int rowIndex = vectorIt.getIndex();
            if(m_rowCounts.at(rowIndex) != -1){
                m_rowCounts.at(rowIndex)++;
                m_basicNonzeroIndices.at(rowIndex).push_back(columnIndex);
            } else {
                m_columnCounts.at(columnIndex)--;
            }
        }
        if (maxColumnCount < m_columnCounts.at(columnIndex)) {
            maxColumnCount = m_columnCounts.at(columnIndex);
        }
    }

    if(buildIndexLists){
        for (std::vector<int>::iterator it = m_rowCounts.begin(); it < m_rowCounts.end(); it++) {
            if (maxRowCount < (int) *it) {
                maxRowCount = *it;
            }
        }
        buildRowCountIndexLists(rowCount, maxRowCount);
        buildColumnCountIndexLists(m_basicColumns.size(), maxColumnCount);
    }
}


void PfiBasis::buildRowCountIndexLists(int size, int maxRowCount) {
    m_rowCountIndexList.init(size, maxRowCount+1);
    if(maxRowCount>0){
        for (std::vector<int>::iterator it = m_rowCounts.begin(); it < m_rowCounts.end(); it++) {
            if (*it >= 0) {
                m_rowCountIndexList.insert(*it, it - m_rowCounts.begin());
            }
        }
    }
    DEVINFO(D::PFIMAKER, "Row links built.");
#ifndef NDEBUG
    DEVINFO(D::PFIMAKER, m_rowCountIndexList);
#endif
}

void PfiBasis::buildColumnCountIndexLists(int size, int maxColumnCount) {
    m_columnCountIndexList.init(size, maxColumnCount+1);
    if(maxColumnCount>0){
        for (std::vector<int>::iterator it = m_columnCounts.begin(); it < m_columnCounts.end(); it++) {
            if (*it >= 0) {
                m_columnCountIndexList.insert(*it, it - m_columnCounts.begin());
            }
        }
    }
    DEVINFO(D::PFIMAKER, "Column links built.");
#ifndef NDEBUG
    DEVINFO(D::PFIMAKER, m_columnCountIndexList);
#endif
}

void PfiBasis::invert() {
    clock_t inversionStart = clock();

    m_transformationCount = 0;
    m_inversionCount++;

    //Reset the data structures
    m_cColumns->clear();
    m_cPivotIndexes->clear();

    for (std::vector<ETM>::iterator iter = m_basis->begin(); iter < m_basis->end(); iter++) {
        delete iter->eta;
    }
    m_basis->clear();
    m_basisNewHead.clear();

    m_basisNonzeros = 0;
    m_inverseNonzeros = 0;

    //Copy the basis for computation
    clock_t copyStart = clock();
    copyBasis();
    clock_t copyStop = clock();

    DEVINFO(D::PFIMAKER, "Basis copied");

//    printActiveSubmatrix();

    //Invert the R, M, C parts separately
    clock_t invertRStart = clock();
//    LPINFO("invertR");
    invertR();
    clock_t invertRStop = clock();

    clock_t findCStart = clock();
    findC();
    clock_t findCStop = clock();

    clock_t invertMStart = clock();
//    LPINFO("invertM");
    invertM();
    clock_t invertMStop = clock();

    clock_t invertCStart = clock();
//    LPINFO("invertC");
    invertC();
    clock_t invertCStop = clock();

    //Free the copied columns
    for(unsigned int i=0; i<m_basicColumnCopies.size(); i++){
        if(m_basicColumnCopies.at(i) != NULL){
            delete m_basicColumnCopies.at(i);
            m_basicColumnCopies.at(i) = NULL;
        }
    }

    //Verify the result of the inversion
    checkSingularity();

    //Update the basis head
    setNewHead();

    m_isFresh = true;

    clock_t inversionStop = clock();
    //Save the inversion times
    cl_inversion += inversionStop - inversionStart;
    cl_copy += copyStop - copyStart;
    cl_invertR += invertRStop - invertRStart;
    cl_invertM += invertMStop - invertMStart;
    cl_invertC += findCStop - findCStart;
    cl_invertC += invertCStop - invertCStart;

    //printStatistics();
    m_transformationAverage += (m_transformationCount - m_transformationAverage) / m_inversionCount;
}
  
void PfiBasis::append(const Vector & vector, int pivotRow, int incoming, Simplex::VARIABLE_STATE outgoingState) {
    //If the alpha vector comes in, then ftran is done already

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
        pivot(vector, pivotRow);
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
        pivot(vector, pivotRow);
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
        pivot(vector, pivotRow);
        m_variableStates->move(outgoing,Simplex::NONBASIC_FIXED, &(outgoingVariable.getLowerBound()));
    } else {
        LPERROR("Invalid outgoing variable state!");
        cerr.unsetf(ios_base::floatfield);
        throw NumericalException(std::string("Invalid outgoing variable state"));
    }
    m_basisHead->at(pivotRow) = incoming;
    m_isFresh = false;
}

void PfiBasis::Ftran(Vector & vector, FTRAN_MODE mode) const {
    __UNUSED(mode);
#ifndef NDEBUG
    //In debug mode the dimensions of the basis and the given vector v are compared.
    //If the dimension mismatches, then the operation cannot be performed.
    //This can't happen in the "normal" case, so in release mode this check is unnecessary.
    if (vector.length() != m_basisHead->size()) {
        LPERROR("FTRAN failed, vector dimension mismatch! ");
        LPERROR("Dimension of the vector to be transformed: " << vector.length());
        LPERROR("Dimension of the basis: " << m_basisHead->size());
    }
#endif //!NDEBUG
    //The ftran operation.

    Numerical::Double * denseVector;

    // 1. lepes: ha kell akkor atvaltjuk dense-re
    if (vector.m_vectorType == Vector::DENSE_VECTOR) {
        denseVector = vector.m_data;
    } else {
        Vector::scatter(Vector::sm_fullLengthVector, Vector::sm_fullLengthVectorLenght, vector);
        denseVector = Vector::sm_fullLengthVector;
    }

    // 2. lepes: vegigmegyunk minden eta vektoron es elvegezzuk a hozzaadast
    std::vector<ETM>::const_iterator iter = m_basis->begin();
    const std::vector<ETM>::const_iterator iterEnd = m_basis->end();

    for (; iter != iterEnd; iter++) {
        const Numerical::Double pivotValue = denseVector[ iter->index ];
        if (pivotValue == 0.0) {
            continue;
        }

        if (iter->eta->m_vectorType == Vector::DENSE_VECTOR) {
            Numerical::Double * ptrValue2 = iter->eta->m_data;
            Numerical::Double * ptrValue1 = denseVector;
            const Numerical::Double * ptrValueEnd = denseVector + vector.m_dimension;
            while (ptrValue1 < ptrValueEnd) {
                const Numerical::Double value = *ptrValue2;
                if (value != 0.0) {
                    const Numerical::Double val = Numerical::stableAdd(*ptrValue1, pivotValue * value);
                    if (*ptrValue1 == 0.0 && val != 0.0) {
                        vector.m_nonZeros++;
                    } else if (*ptrValue1 != 0.0 && val == 0.0) {
                        vector.m_nonZeros--;
                    }
                    *ptrValue1 = val;
                }
                ptrValue1++;
                ptrValue2++;
            }
            //A vegen lerendezzuk a pivot poziciot is:
            const Numerical::Double val = pivotValue * iter->eta->m_data[iter->index];
            if (denseVector[iter->index] == 0.0 && val != 0.0) {
                vector.m_nonZeros++;
            } else if (denseVector[iter->index] != 0.0 && val == 0.0) {
                vector.m_nonZeros--;
            }
            denseVector[iter->index] = val;

        } else {
            Numerical::Double * ptrEta = iter->eta->m_data;
            unsigned int * ptrIndex = iter->eta->m_index;
            const unsigned int * ptrIndexEnd = ptrIndex + iter->eta->m_size;
            const unsigned int pivotPosition = iter->index;
            while (ptrIndex < ptrIndexEnd) {
                Numerical::Double & originalValue = denseVector[*ptrIndex];
                if (*ptrEta != 0.0) {
                    Numerical::Double val;
                    if (*ptrIndex != pivotPosition) {
                        val = Numerical::stableAdd(originalValue, pivotValue * *ptrEta);
                        if (originalValue == 0.0 && val != 0.0) {
                            vector.m_nonZeros++;
                        } else if (originalValue != 0.0 && val == 0.0) {
                            vector.m_nonZeros--;
                        }
                    } else {
                        val = pivotValue * *ptrEta;
                    }
                    originalValue = val;
                }
                ptrIndex++;
                ptrEta++;
            }
        }
    }
    // 3. lepes: ha kell akkor v-t atvaltani, adatokat elmenteni    Vector::VECTOR_TYPE newType;

    Vector::VECTOR_TYPE newType;
    if (vector.m_nonZeros < vector.m_sparsityThreshold) {
        newType = Vector::SPARSE_VECTOR;
    } else {
        newType = Vector::DENSE_VECTOR;
    }

    if (vector.m_vectorType == Vector::DENSE_VECTOR) {
        if (newType == Vector::DENSE_VECTOR) {
            return;
        } else {
            vector.denseToSparse();
        }
    } else {
        vector.prepareForData(vector.m_nonZeros, vector.m_dimension);
        Numerical::Double * ptrValue = denseVector;
        const Numerical::Double * ptrValueEnd = denseVector + vector.m_dimension;
        unsigned int index = 0;
        while (ptrValue < ptrValueEnd) {
            if (*ptrValue != 0.0) {
                vector.newNonZero(*ptrValue, index);
                *ptrValue = 0.0;
            }
            ptrValue++;
            index++;
        }
        if (vector.m_vectorType == Vector::SPARSE_VECTOR) {
            vector.m_sorted = true;
        }
    }
}

void PfiBasis::Btran(Vector & vector, BTRAN_MODE mode) const
{
    __UNUSED(mode);
#ifndef NDEBUG
    //In debug mode the dimensions of the basis and the given vector v are compared.
    //If the dimension mismatches, then the operation cannot be performed.
    //This can't happen in the "normal" case, so in release mode this check is unnecessary.
    if (vector.length() != m_basisHead->size()) {
        LPERROR("BTRAN failed, vector dimension mismatch! ");
        LPERROR("Dimension of the vector to be transformed: " << vector.length());
        LPERROR("Dimension of the basis: " << m_basisHead->size());
    }
#endif //!NDEBUG

    //The btran operation.
    Numerical::Double * denseVector;

    // 1. lepes: ha kell akkor atvaltjuk dense-re
    if (vector.m_vectorType == Vector::DENSE_VECTOR) {
        denseVector = vector.m_data;
    } else {
        Vector::scatter(Vector::sm_fullLengthVector, Vector::sm_fullLengthVectorLenght, vector);
        denseVector = Vector::sm_fullLengthVector;
    }

    // 2. lepes: vegigmegyunk minden eta vektoron es elvegezzuk a skalaris szorzast
    std::vector<ETM>::const_reverse_iterator iter = m_basis->rbegin();
    const std::vector<ETM>::const_reverse_iterator iterEnd = m_basis->rend();

//    const ETM * iter = m_basis->data().data() + m_basis->size() - 1;
//    const ETM * iterEnd = m_basis->data() - 1;
    for (; iter != iterEnd; iter++) {
        unsigned int nonZeros = vector.nonZeros();

        //Numerical::Summarizer summarizer;
        static Numerical::BucketSummarizer summarizer(8); // ez a klasszikus neg-pos-os, minel lejjebb visszuk
                                                           // annal pontosabb, de annal lassabb is

        if (iter->eta->m_vectorType == Vector::DENSE_VECTOR) {
            Numerical::Double * ptrValue2 = iter->eta->m_data;
            Numerical::Double * ptrValue1 = denseVector;
            const Numerical::Double * ptrValueEnd = denseVector + vector.m_dimension;
            while (ptrValue1 < ptrValueEnd && nonZeros) {
                //TODO vajon melyik ritkásbb?
                const Numerical::Double value = *ptrValue1;
                if (value != 0.0) {
                    nonZeros--;
                    summarizer.add(value * *ptrValue2);
                }
                ptrValue1++;
                ptrValue2++;
            }
        } else {
            Numerical::Double * ptrValue = iter->eta->m_data;
            unsigned int * ptrIndex = iter->eta->m_index;
            const unsigned int * ptrIndexEnd = ptrIndex + iter->eta->m_size;
            while (ptrIndex < ptrIndexEnd && nonZeros) {
                const Numerical::Double value = denseVector[*ptrIndex];
                if (value != 0.0) {
                    nonZeros--;
                    summarizer.add(value * *ptrValue);
                }
                ptrIndex++;
                ptrValue++;
            }
        }

        Numerical::Double dotProduct = summarizer.getResult();

        const int pivot = iter->index;
        if (denseVector[pivot] != 0.0 && dotProduct == 0.0) {
            vector.m_nonZeros--;
        }
        if (denseVector[pivot] == 0.0 && dotProduct != 0.0) {
            vector.m_nonZeros++;
        }
        denseVector[pivot] = dotProduct;
    }
    // 3. lepes: ha kell akkor v-t atvaltani, adatokat elmenteni
    Vector::VECTOR_TYPE newType;
    if (vector.m_nonZeros < vector.m_sparsityThreshold) {
        newType = Vector::SPARSE_VECTOR;
    } else {
        newType = Vector::DENSE_VECTOR;
    }

    if (vector.m_vectorType == Vector::DENSE_VECTOR) {
        if (newType == Vector::DENSE_VECTOR) {
            return;
        } else {
            vector.denseToSparse();
        }
    } else {
        vector.prepareForData(vector.m_nonZeros, vector.m_dimension);
        Numerical::Double * ptrValue = denseVector;
        const Numerical::Double * ptrValueEnd = denseVector + vector.m_dimension;
        unsigned int index = 0;
        while (ptrValue < ptrValueEnd) {
            if (*ptrValue != 0.0) {
                vector.newNonZero(*ptrValue, index);
                *ptrValue = 0.0;
            }
            ptrValue++;
            index++;
        }
        if (vector.m_vectorType == Vector::SPARSE_VECTOR) {
            vector.m_sorted = true;
        }
    }

}


void PfiBasis::updateColumns(unsigned int rowindex, unsigned int columnindex) {
    clock_t updateStart = clock();

    std::list<int>::iterator it = m_basicNonzeroIndices.at(rowindex).begin();
    std::list<int>::iterator itend = m_basicNonzeroIndices.at(rowindex).end();

    for (; it != itend; it++) {
        if (*it != (int) columnindex) {
            if(m_basicColumnCopies.at(*it)==NULL){
                m_basicColumnCopies.at(*it) = new Vector(*(m_basicColumns.at(*it)));
                m_basicColumns.at(*it) = m_basicColumnCopies.at(*it);
            }
            m_transformationCount += m_basis->back().eta->nonZeros();
            //Remove everything
            Vector::NonzeroIterator columnIt = m_basicColumnCopies.at(*it)->beginNonzero();
            Vector::NonzeroIterator columnItend = m_basicColumnCopies.at(*it)->endNonzero();
            for (; columnIt < columnItend; columnIt++) {
                if(columnIt.getIndex() != rowindex && m_rowCounts.at(columnIt.getIndex()) > -1){
                    m_basicNonzeroIndices.at(columnIt.getIndex()).remove(*it);
                    m_rowCounts.at(columnIt.getIndex())--;
                }
            }
//            LPINFO("UPDATING COLUMN: "<<*m_basicColumnCopies.at(*it));
            m_basicColumnCopies.at(*it)->elementaryFtran(*(m_basis->back().eta), m_basis->back().index);

//            LPINFO("UPDATED COLUMN: "<<*m_basicColumnCopies.at(*it));
            //Add the changes back
            int newColumnCount = 0;
            columnIt = m_basicColumnCopies.at(*it)->beginNonzero();
            columnItend = m_basicColumnCopies.at(*it)->endNonzero();
            for (; columnIt < columnItend; columnIt++) {
                if(columnIt.getIndex() != rowindex && m_rowCounts.at(columnIt.getIndex()) > -1){
                    m_basicNonzeroIndices.at(columnIt.getIndex()).push_back(*it);
                    m_rowCounts.at(columnIt.getIndex())++;
                    newColumnCount++;
                }
            }

            //Update the column count
            m_columnCounts.at(*it) = newColumnCount;
        }
    }

    clock_t updateStop = clock();
    cl_updateColumns += updateStop - updateStart;
}

void PfiBasis::pivot(const Vector& column, int pivotRow) {
    clock_t pivotStart = clock();

    ETM newEtm;
    newEtm.eta = createEta(column, pivotRow);
    newEtm.index = pivotRow;
    m_inverseNonzeros += newEtm.eta->nonZeros();
    m_basis->push_back(newEtm);

    clock_t pivotStop = clock();
    cl_pivot += pivotStop - pivotStart;
}

void PfiBasis::invertR() {

    //The upper triangular part is called R part
    DEVINFO(D::PFIMAKER, "Search for the R part and invert it");
    unsigned int rNum = 0;

        if (m_rowCountIndexList.getPartitionCount() > 1) {
            while (m_rowCountIndexList.firstElement(1) != -1 ) {
                int rowindex = m_rowCountIndexList.firstElement(1);
                //This part searches for rows with row count 1 and order them to the upper triangular part
                int columnindex = m_basicNonzeroIndices.at(rowindex).front();

                const Vector *currentColumn = m_basicColumns.at(columnindex);
                //Invert the chosen R column
                DEVINFO(D::PFIMAKER, "Inverting R column " << columnindex << " with pivot row " << rowindex);

                pivot(*currentColumn, rowindex);
                m_basisNewHead.at(rowindex) = m_basicColumnIndices.at(columnindex);

                //Update the row lists and row counts
                Vector::NonzeroIterator it = currentColumn->beginNonzero();
                Vector::NonzeroIterator itend = currentColumn->endNonzero();
                for (; it < itend; it++) {
                    int index = it.getIndex();
                    //If the row of the iterated element is still active
                    if (m_rowCounts.at(index) > -1) {
                        m_rowCounts.at(index)--;
                        m_rowCountIndexList.move(index, m_rowCounts.at(index));
                        m_basicNonzeroIndices.at(index).remove(columnindex);
                    }
                }

                //Set the column count to zero to represent that which column has been chosen.
                m_columnCounts.at(columnindex) = -1;
                m_columnCountIndexList.remove(columnindex);
                m_rowCounts.at(rowindex) = -1;
                m_rowCountIndexList.remove(rowindex);

                rNum++;
            }
        }

    DEVINFO(D::PFIMAKER, "RPART num: " << rNum);
}

void PfiBasis::findC() {
    //The lower triangular part is called C part
    DEVINFO(D::PFIMAKER, "Search for C part");
    unsigned int cNum = 0;
    if (m_columnCountIndexList.getPartitionCount() > 1) {
        while (m_columnCountIndexList.firstElement(1) != -1) {
            int columnindex = m_columnCountIndexList.firstElement(1);
            //This part searches for rows with row count 1 and order them to the upper triangular part
            int rowindex = -1 ;
            Vector::NonzeroIterator it = m_basicColumns.at(columnindex)->beginNonzero();
            Vector::NonzeroIterator itend = m_basicColumns.at(columnindex)->endNonzero();
            for (; it < itend; it++) {
                if (m_rowCounts.at(it.getIndex()) > 0) {
                    rowindex = it.getIndex();
                    break;
                }
            }
            const Vector *currentColumn = m_basicColumns.at(columnindex);

            //Store the chosen columns
            m_cColumns->push_back(currentColumn);
            m_cPivotIndexes->push_back(rowindex);
            m_basisNewHead.at(rowindex) = m_basicColumnIndices.at(columnindex);
            DEVINFO(D::PFIMAKER, "Submatrix column " << columnindex << " stored for C part with pivot index " << rowindex);

            //Update the column counts but the columns remains untouched
            std::list<int>::iterator listIt = m_basicNonzeroIndices.at(rowindex).begin();
            std::list<int>::iterator listItend = m_basicNonzeroIndices.at(rowindex).end();
            for (; listIt != listItend; listIt++) {
                int index = *listIt;
                if (m_columnCounts.at(index) > -1) {
                    m_columnCounts.at(index)--;
                    m_columnCountIndexList.move(index, m_columnCounts.at(index));
                }
            }
            m_basicNonzeroIndices.at(rowindex).clear();
            //Set the row count set to zero to represent that which row has been chosen.
            m_rowCounts.at(rowindex) = -1;
            m_rowCountIndexList.remove(rowindex);
            m_columnCounts.at(columnindex) = -1;
            m_columnCountIndexList.remove(columnindex);

            cNum++;
        }
    }

    DEVINFO(D::PFIMAKER, "CPART num: " << cNum);
}

void PfiBasis::invertM() {
    //The middle (non-triangular) part is called M part
    DEVINFO(D::PFIMAKER, "Organize the M part and invert the columns");
    unsigned int mNum = 0;
    switch (nontriangularMethod) {
    case SEARCH:{
            //containsOne represents the exit variable
            bool containsOne;
            containsOne = true;
            while (containsOne) {
                containsOne = false;
                for (std::vector<int>::iterator it = m_rowCounts.begin(); it < m_rowCounts.end(); it++) {
                    if (*it > 0) {
                        DEVINFO(D::PFIMAKER, "Choosing M column with rowcount " << *it);
                        int rowindex = it - m_rowCounts.begin();
                        int columnindex = m_basicNonzeroIndices.at(rowindex).front();
                        const Vector* currentColumn = m_basicColumns.at(columnindex);
                        if (nontriangularCheck(rowindex, currentColumn, -1)) {
                            //Invert the chosen M column
                            DEVINFO(D::PFIMAKER, "Inverting M column " << columnindex << " with pivot row " << rowindex);
                            pivot(*currentColumn, rowindex);
                            containsOne = true;
                            m_basisNewHead.at(rowindex) = m_basicColumnIndices.at(columnindex);
                            //Update the remaining columns
                            updateColumns(rowindex, columnindex);
                            //Update the row lists and row counts
                            Vector::NonzeroIterator vectorIt = currentColumn->beginNonzero();
                            Vector::NonzeroIterator vectorItend = currentColumn->endNonzero();
                            for (; vectorIt < vectorItend; vectorIt++) {
                                if (m_rowCounts.at(vectorIt.getIndex()) >= 0) {
                                    m_rowCounts.at(vectorIt.getIndex())--;
                                    m_basicNonzeroIndices.at(vectorIt.getIndex()).remove(columnindex);
                                }
                            }
                            //Update the column counts too
                            std::list<int>::iterator listIt = m_basicNonzeroIndices.at(rowindex).begin();
                            std::list<int>::iterator listItend = m_basicNonzeroIndices.at(rowindex).end();
                            for (; listIt != listItend; listIt++) {
                                int index = *listIt;
                                if (m_columnCounts.at(index) > -1) {
                                    m_columnCounts.at(index)--;
                                }
                            }
                            //Set the column and row count to zero to represent that which column and row has been chosen.
                            m_rowCounts.at(rowindex) = -1;
                            m_columnCounts.at(columnindex) = -1;
                        } else {
                            LPWARNING("Non-triangular pivot position is numerically unstable, ignoring column:" << columnindex << ")");
                            //Update the row lists and row counts
                            Vector::NonzeroIterator it = currentColumn->beginNonzero();
                            Vector::NonzeroIterator itend = currentColumn->endNonzero();
                            for (; it < itend; it++) {
                                int index = it.getIndex();
                                //If the row of the iterated element is still active
                                if (m_rowCounts.at(index) > -1) {
                                    m_rowCounts.at(index)--;
                                    m_basicNonzeroIndices.at(index).remove(columnindex);
                                }
                            }
                            //Set the column count to zero to represent that which column is unstable.
                            m_columnCounts.at(columnindex) = -1;
                        }
                        mNum++;
                    }
                }
            }
            break;

    }
    case BLOCK_TRIANGULAR:{
        clock_t createBlockTriangularStart = clock();
        createBlockTriangular();
        clock_t createBlockTriangularStop = clock();
        cl_createBlockTriangular += createBlockTriangularStop - createBlockTriangularStart;
        if (m_mmBlocks->size() > 0) {
            int currentBlock = -1;
            int currentBlockSize = 0;
            for (std::vector<int>::iterator rowIt = m_rowSwapHash->begin(); rowIt < m_rowSwapHash->end(); rowIt++) {

                if (currentBlockSize == 0) {
                    currentBlock++;
                    currentBlockSize = m_mmBlocks->at(currentBlock);
                    //LPINFO("Iterating block #"<<currentBlock<<" with block size: "<<currentBlockSize);
                }
                currentBlockSize--;
                //Rowindex: Ki volt eredetileg aki az iteratoradik sor
                int rowindex = m_mmRowIndices->at(*rowIt);
                //Columnindex: Ki volt eredetileg az iteratoradik oszlop
                int columnindex = m_mmColumnIndices->at(m_columnSwapHash->at(rowIt - m_rowSwapHash->begin()));
                const Vector* currentColumn = m_basicColumns.at(columnindex);
                if (nontriangularCheck(rowindex, currentColumn, currentBlock)) {
                    DEVINFO(D::PFIMAKER, "Inverting M column " << columnindex << " with pivot row " << rowindex);
                    //Invert the chosen M column

                    pivot(*currentColumn, rowindex);
                    m_basisNewHead.at(rowindex) = m_basicColumnIndices.at(columnindex);
                    //Update the remaining columns
                    updateColumns(rowindex, columnindex);

                    //Update the row lists and row counts
                    Vector::NonzeroIterator it = currentColumn->beginNonzero();
                    Vector::NonzeroIterator itend = currentColumn->endNonzero();
                    for (; it < itend; it++) {
                        int index = it.getIndex();
                        //If the row of the iterated element is still active
                        if (m_rowCounts.at(index) > -1) {
                            m_rowCounts.at(index)--;
                            m_basicNonzeroIndices.at(index).remove(columnindex);
                        }
                    }
                    //Update the column counts too
                    std::list<int>::iterator listIt = m_basicNonzeroIndices.at(rowindex).begin();
                    std::list<int>::iterator listItend = m_basicNonzeroIndices.at(rowindex).end();
                    for (; listIt != listItend; listIt++) {
                        int index = *listIt;
                        if (m_columnCounts.at(index) > -1) {
                            m_columnCounts.at(index)--;
                        }
                    }
                    m_basicNonzeroIndices.at(rowindex).clear();
                    //Set the column and row count to zero to represent that which column and row has been chosen.
                    m_rowCounts.at(rowindex) = -1;
                    m_columnCounts.at(columnindex) = -1;


                } else {
                    LPWARNING("Non-triangular pivot position is numerically unstable, ignoring column:" << columnindex << ")");
                    //Update the row lists and row counts
                    Vector::NonzeroIterator it = currentColumn->beginNonzero();
                    Vector::NonzeroIterator itend = currentColumn->endNonzero();
                    for (; it < itend; it++) {
                        int index = it.getIndex();
                        //If the row of the iterated element is still active
                        if (m_rowCounts.at(index) > -1) {
                            m_rowCounts.at(index)--;
                            m_basicNonzeroIndices.at(index).remove(columnindex);
                        }
                    }
                    //Set the column count to zero to represent that which column is unstable.
                    m_columnCounts.at(columnindex) = -1;
                }
                mNum++;
            }
        }

        break;
    }
    case BLOCK_ORDERED_TRIANGULAR:{
        clock_t createBlockTriangularStart = clock();
        createBlockTriangular();
        clock_t createBlockTriangularStop = clock();
        cl_createBlockTriangular += createBlockTriangularStop - createBlockTriangularStart;
        {
            int blockStart = 0;
            for (std::vector<int>::iterator blockIt = m_mmBlocks->begin(); blockIt < m_mmBlocks->end(); blockIt++) {
                int currentBlock = blockIt - m_mmBlocks->begin();
                int currentBlockSize = *blockIt;
                std::multimap<int, int> columnOrders;
                int i;
                for (i = blockStart; i < blockStart + currentBlockSize; i++) {
                    columnOrders.insert(
                            std::make_pair((int) m_mmColumns->at(i).nonZeros(),
                                    m_mmColumnIndices->at(m_columnSwapHash->at(i))));
                }
                for (std::multimap<int, int>::iterator colIt = columnOrders.begin(); colIt != columnOrders.end(); colIt++) {
                    blockStart++;
                    int rowindex = -1;
                    int columnindex = colIt->second;
                    const Vector* currentColumn = m_basicColumns.at(columnindex);
                    if (nontriangularCheck(rowindex, currentColumn, currentBlock)) {
                        DEVINFO(D::PFIMAKER, "Inverting M column " << columnindex << " with pivot row " << rowindex);
                        //Invert the chosen M column

                        pivot(*currentColumn, rowindex);
                        m_basisNewHead.at(rowindex) = m_basicColumnIndices.at(columnindex);
                        //Update the remaining columns
                        updateColumns(rowindex, columnindex);

                        //Update the row lists and row counts
                        Vector::NonzeroIterator it = currentColumn->beginNonzero();
                        Vector::NonzeroIterator itend = currentColumn->endNonzero();
                        for (; it < itend; it++) {
                            int index = it.getIndex();
                            //If the row of the iterated element is still active
                            if (m_rowCounts.at(index) > -1) {
                                m_rowCounts.at(index)--;
                                m_basicNonzeroIndices.at(index).remove(columnindex);
                            }
                        }
                        //Update the column counts too
                        std::list<int>::iterator listIt = m_basicNonzeroIndices.at(rowindex).begin();
                        std::list<int>::iterator listItend = m_basicNonzeroIndices.at(rowindex).end();
                        for (; listIt != listItend; listIt++) {
                            int index = *listIt;
                            if (m_columnCounts.at(index) > -1) {
                                m_columnCounts.at(index)--;
                            }
                        }
                        m_basicNonzeroIndices.at(rowindex).clear();
                        //Set the column and row count to zero to represent that which column and row has been chosen.
                        m_rowCounts.at(rowindex) = -1;
                        m_columnCounts.at(columnindex) = -1;
                    } else {
                        LPWARNING("Non-triangular pivot position is numerically unstable, ignoring column:" << columnindex << ")");
                        //Update the row lists and row counts
                        Vector::NonzeroIterator it = currentColumn->beginNonzero();
                        Vector::NonzeroIterator itend = currentColumn->endNonzero();
                        for (; it < itend; it++) {
                            int index = it.getIndex();
                            //If the row of the iterated element is still active
                            if (m_rowCounts.at(index) > -1) {
                                m_rowCounts.at(index)--;
                                m_basicNonzeroIndices.at(index).remove(columnindex);
                            }
                        }
                        //Set the column count to zero to represent that which column is unstable.
                        m_columnCounts.at(columnindex) = -1;
                    }
                    mNum++;
                }
            }
        }

        break;
    }
    default:
        LPWARNING("No triangulatrization method defined!")
        ;
        break;
    }
    DEVINFO(D::PFIMAKER, "MPART num: " << mNum);
}

void PfiBasis::invertC() {
    //The lower triangular part is called C part
    DEVINFO(D::PFIMAKER, "Invert the C part");
    for (std::vector<const Vector*>::reverse_iterator it = m_cColumns->rbegin(); it < m_cColumns->rend(); it++) {
        DEVINFO(D::PFIMAKER, "Inverting C column " << m_cColumns->rend() - 1 - it <<
                " with pivot row " << m_cPivotIndexes->at(m_cColumns->rend() - 1 - it));
        pivot(*(*it), m_cPivotIndexes->at(m_cColumns->rend() - 1 - it));
    }
    return;
}

void PfiBasis::buildMM() {
    //Reset the data structures used for processing the non-triangular part
    m_mmRowIndices->clear();
    m_mmRows->clear();
    m_mmColumnIndices->clear();

    m_mmColumns->clear();
    m_rowSwapHash->clear();
    m_columnSwapHash->clear();
    m_columnSwapLog->clear();
    m_mmGraphOut->clear();
    m_mmGraphIn->clear();
    m_stack->clear();
    m_orderedIndex->clear();
    m_mmBlocks->clear();

    //Collect the indices of the MM part
    std::vector<int> rowMemory = std::vector<int>(m_rowCounts.size(), -1);
    for (std::vector<int>::iterator it = m_rowCounts.begin(); it < m_rowCounts.end(); it++) {
        if (*it >= 0) {
            m_mmRowIndices->push_back(it - m_rowCounts.begin());
            rowMemory.at(it - m_rowCounts.begin()) = m_mmRowIndices->size() - 1;
        }
    }

    for (std::vector<int>::iterator it = m_columnCounts.begin(); it < m_columnCounts.end(); it++) {
        if (*it >= 0) {
            m_mmColumnIndices->push_back(it - m_columnCounts.begin());
        }
    }
    if (m_mmRowIndices->size() != m_mmColumnIndices->size()) {
        LPERROR("Non-triangular part inversion failed!");
        LPERROR("The numbers of rows and columns with nonnegative counters are different!");
        return;
    }
    int mmSize = m_mmRowIndices->size();
    //Compute the average size
    m_mNumAverage += (mmSize - m_mNumAverage) / m_inversionCount;

    m_mmRows->resize(mmSize, Vector(mmSize));
    m_mmColumns->resize(mmSize, Vector(mmSize));
    m_rowSwapHash->resize(mmSize);
    m_columnSwapHash->resize(mmSize);
    m_columnSwapLog->resize(mmSize);
    m_orderedIndex->resize(mmSize, false);

    for (int i = 0; i < mmSize; i++) {
        m_rowSwapHash->at(i) = i;
        m_columnSwapHash->at(i) = i;
        m_columnSwapLog->at(i) = i;
        //     m_mmRows->at(i).setSparsityRatio(0.0);
        //   m_mmColumns->at(i).setSparsityRatio(0.0);
    }
    for (std::vector<int>::iterator it = m_mmColumnIndices->begin(); it < m_mmColumnIndices->end(); it++) {
        Vector::NonzeroIterator vectorIt = m_basicColumns.at(*it)->beginNonzero();
        Vector::NonzeroIterator vectorItend = m_basicColumns.at(*it)->endNonzero();
        for (; vectorIt < vectorItend; vectorIt++) {
            if (rowMemory.at(vectorIt.getIndex()) != -1) {
                int rowIndex = rowMemory.at(vectorIt.getIndex());
                int columnIndex = it - m_mmColumnIndices->begin();
                //TODO: setnewnonzero -> preparefordata
                m_mmRows->at(rowIndex).setNewNonzero(columnIndex, *vectorIt);
                m_mmColumns->at(columnIndex).setNewNonzero(rowIndex, *vectorIt);
            }
        }
    }
    DEVINFO(D::PFIMAKER, "Nonzero pattern in MM built");
}

void PfiBasis::findTransversal() {
    DEVINFO(D::PFIMAKER, "Searching for transversal form");
    for (int i = 0; i < (int) m_rowSwapHash->size(); i++) {
        if (Numerical::equals(m_mmRows->at(i).at(i), 0)) {
            //Find some with bigger index
            bool nextRow = false;
            DEVINFO(D::PFIMAKER, "Searching for diagonal nonzero at row " << i);
            Vector::NonzeroIterator vectorIt = m_mmColumns->at(i).beginNonzero();
            Vector::NonzeroIterator vectorItend = m_mmColumns->at(i).endNonzero();
            for (; vectorIt < vectorItend; vectorIt++) {
                if ((int) vectorIt.getIndex() > i) {
                    nextRow = true;
                    DEVINFO(D::PFIMAKER, "Nonzero found below index " << i);
                    swapRows(vectorIt.getIndex(), i);
                    break;
                } else if (Numerical::equals(m_mmRows->at(vectorIt.getIndex()).at(vectorIt.getIndex()), 0)) {
                    nextRow = true;
                    DEVINFO(D::PFIMAKER, "Nonzero found in singular row below index " << i);
                    swapRows(vectorIt.getIndex(), i);
                    break;
                }
            }
            if (!nextRow) {
                DEVINFO(D::PFIMAKER, "Nonzero not found, search columns");
                std::vector<int> searchedRows;
                vectorIt = m_mmColumns->at(i).beginNonzero();
                vectorItend = m_mmColumns->at(i).endNonzero();
                for (; vectorIt < vectorItend; vectorIt++) {
                    int searchResult;
                    searchedRows.push_back(vectorIt.getIndex());
                    searchResult = searchColumn(vectorIt.getIndex(), i, searchedRows);

                    DEVINFO(D::PFIMAKER, "search result: " << searchResult);
                    if (searchResult > -1) {
                        if (searchResult != i) {
                            swapRows(searchResult, i);
                        }
                        break;
                    }
                }
            }
        }
    }
#ifndef NDEBUG
    DEVINFO(D::PFIMAKER, "Nonzero pattern with transversal ");
//    printMM();
#endif //!NDEBUG
}

void PfiBasis::swapRows(int rowIndex1, int rowIndex2) {
    DEVINFO(D::PFIMAKER, "Swapping row " << rowIndex1 << " and " << rowIndex2);

    int rowHashIndex1 = m_rowSwapHash->at(rowIndex1);
    int rowHashIndex2 = m_rowSwapHash->at(rowIndex2);
    //TODO: gyorsitani
    Vector row1 = m_mmRows->at(rowIndex1);
    Vector row2 = m_mmRows->at(rowIndex2);

    for (Vector::NonzeroIterator vectorIt = row1.beginNonzero(); vectorIt < row1.endNonzero(); vectorIt++) {
        m_mmColumns->at(vectorIt.getIndex()).set(rowIndex1, 0);
    }
    for (Vector::NonzeroIterator vectorIt = row2.beginNonzero(); vectorIt < row2.endNonzero(); vectorIt++) {
        m_mmColumns->at(vectorIt.getIndex()).set(rowIndex2, 0);
    }
    for (Vector::NonzeroIterator vectorIt = row1.beginNonzero(); vectorIt < row1.endNonzero(); vectorIt++) {
        m_mmColumns->at(vectorIt.getIndex()).set(rowIndex2, *vectorIt);
    }
    for (Vector::NonzeroIterator vectorIt = row2.beginNonzero(); vectorIt < row2.endNonzero(); vectorIt++) {
        m_mmColumns->at(vectorIt.getIndex()).set(rowIndex1, *vectorIt);
    }
    //TODO: JOCO Swap fuggveny a vektorba
    m_mmRows->at(rowIndex1) = row2;
    m_mmRows->at(rowIndex2) = row1;

    m_rowSwapHash->at(rowIndex1) = rowHashIndex2;
    m_rowSwapHash->at(rowIndex2) = rowHashIndex1;
    //printSwapHashes();
}

void PfiBasis::swapColumns(int columnIndex1, int columnIndex2) {
    DEVINFO(D::PFIMAKER, "Swapping column " << columnIndex1 << " and " << columnIndex2);

    int columnHashIndex1 = m_columnSwapHash->at(columnIndex1);
    int columnHashIndex2 = m_columnSwapHash->at(columnIndex2);
    Vector column1 = m_mmColumns->at(columnIndex1);
    Vector column2 = m_mmColumns->at(columnIndex2);

    for (Vector::NonzeroIterator vectorIt = column1.beginNonzero(); vectorIt < column1.endNonzero(); vectorIt++) {
        m_mmRows->at(vectorIt.getIndex()).set(columnIndex1, 0);
    }
    for (Vector::NonzeroIterator vectorIt = column2.beginNonzero(); vectorIt < column2.endNonzero(); vectorIt++) {
        m_mmRows->at(vectorIt.getIndex()).set(columnIndex2, 0);
    }
    for (Vector::NonzeroIterator vectorIt = column1.beginNonzero(); vectorIt < column1.endNonzero(); vectorIt++) {
        m_mmRows->at(vectorIt.getIndex()).set(columnIndex2, *vectorIt);
    }
    for (Vector::NonzeroIterator vectorIt = column2.beginNonzero(); vectorIt < column2.endNonzero(); vectorIt++) {
        m_mmRows->at(vectorIt.getIndex()).set(columnIndex1, *vectorIt);
    }
    m_mmColumns->at(columnIndex1) = column2;
    m_mmColumns->at(columnIndex2) = column1;

    m_columnSwapLog->at(columnHashIndex1) = columnIndex2;
    m_columnSwapLog->at(columnHashIndex2) = columnIndex1;

    m_columnSwapHash->at(columnIndex1) = columnHashIndex2;
    m_columnSwapHash->at(columnIndex2) = columnHashIndex1;
    //printSwapHashes();
}

int PfiBasis::searchColumn(int columnIndex, int searchIndex, std::vector<int>& searchedRows) {
    DEVINFO(D::PFIMAKER, "Searching column " << columnIndex << " for nonzero with searchindex " << searchIndex);
    Vector::NonzeroIterator it = m_mmColumns->at(columnIndex).beginNonzero();
    Vector::NonzeroIterator itend = m_mmColumns->at(columnIndex).endNonzero();
    for (; it < itend; it++) {
        if ((int) it.getIndex() >= searchIndex) {
            DEVINFO(D::PFIMAKER, "Nonzero found below index " << searchIndex);
            int searchResult = it.getIndex();
            swapRows(columnIndex, searchResult);
            return searchResult;
        } else if (Numerical::equals(m_mmRows->at(it.getIndex()).at(it.getIndex()), 0)) {
            DEVINFO(D::PFIMAKER, "Nonzero found in singular row below index " << searchIndex);
            int searchResult = it.getIndex();
            swapRows(columnIndex, searchResult);
            return searchResult;
        }
    }
    Vector::NonzeroIterator vectorIt = m_mmColumns->at(columnIndex).beginNonzero();
    Vector::NonzeroIterator vectorItend = m_mmColumns->at(columnIndex).endNonzero();
    for (; vectorIt < vectorItend; vectorIt++) {
        bool contains = false;
        for (std::vector<int>::iterator it = searchedRows.begin(); it < searchedRows.end(); it++) {
            if (*it == (int) vectorIt.getIndex()) {
                contains = true;
            }
        }
        if (contains) {
            continue;
        }
        searchedRows.push_back(vectorIt.getIndex());
        int searchResult = searchColumn(vectorIt.getIndex(), searchIndex, searchedRows);
        if (searchResult > -1) {
            swapRows(columnIndex, searchResult);
            return searchResult;
        }
    }
    DEVINFO(D::PFIMAKER, "Searching ended in column " << columnIndex);
    return -1;
}

void PfiBasis::createGraph() {
    m_mmGraphOut->resize(m_mmRows->size(), std::vector<int>());
    m_mmGraphIn->resize(m_mmRows->size(), std::vector<int>());
    for (std::vector<Vector>::iterator it = m_mmRows->begin(); it < m_mmRows->end(); it++) {
        Vector::NonzeroIterator vectorIt = it->beginNonzero();
        Vector::NonzeroIterator vectorItend = it->endNonzero();
        for (; vectorIt < vectorItend; vectorIt++) {
            if ((int) vectorIt.getIndex() != it - m_mmRows->begin()) {
                m_mmGraphOut->at(it - m_mmRows->begin()).push_back(vectorIt.getIndex());
                m_mmGraphIn->at(vectorIt.getIndex()).push_back(it - m_mmRows->begin());
            }
        }
    }
}

void PfiBasis::tarjan() {
    DEVINFO(D::PFIMAKER, "Tarjan begin");
    for (std::vector<char>::iterator it = m_orderedIndex->begin(); it < m_orderedIndex->end(); it++) {
        DEVINFO(D::PFIMAKER, "Tarjan step" << it - m_orderedIndex->begin());
        if (*it == false) {
            PathNode node;
            node.index = it - m_orderedIndex->begin();
            node.lowest = node.index;
            node.nextEdge = 0;
            m_stack->push_back(node);
            searchNode();
        }
    }
}

int PfiBasis::searchNode() {
    //TODO: std::stack?
    PathNode currentNode = m_stack->back();
    int stackPosition = m_stack->size() - 1;
    int nextLowest = -1;
    DEVINFO(D::PFIMAKER, "Searching node " << currentNode.index);
    std::vector<int> outGoing = m_mmGraphOut->at(currentNode.index);
    while ((int) outGoing.size() > currentNode.nextEdge) {
        if (m_mmGraphIn->at(outGoing.at(currentNode.nextEdge)).size() > 0) {
            int next = outGoing.at(currentNode.nextEdge);
            DEVINFO(D::PFIMAKER, "Searching edge " << next);
            bool newNode = true;
            for (std::vector<PathNode>::iterator it = m_stack->begin(); it < m_stack->end(); it++) {
                if (it->index == next) {
                    newNode = false;
                    break;
                }
            }
            if (newNode) {
                DEVINFO(D::PFIMAKER, "Edge points to a new node, create it on the stack");
                PathNode node;
                node.index = next;
                node.lowest = node.index;
                node.nextEdge = 0;
                m_stack->push_back(node);
                nextLowest = searchNode();
                if (nextLowest != -1) {
                    for (std::vector<PathNode>::iterator it = m_stack->begin(); it < m_stack->end(); it++) {
                        if (it->index == currentNode.lowest) {
                            break;
                        }
                        if (it->index == nextLowest) {
                            currentNode.lowest = nextLowest;
                            m_stack->at(stackPosition).lowest = nextLowest;
                            break;
                        }
                    }
                }
            } else {
                for (std::vector<PathNode>::iterator it = m_stack->begin(); it < m_stack->end(); it++) {
                    if (it->index == next) {
                        DEVINFO(D::PFIMAKER, "Setting current lowest to next " << next);
                        currentNode.lowest = it->lowest;
                        m_stack->at(stackPosition).lowest = it->lowest;
#ifndef NDEBUG
//                        printStack();
#endif //!NDEBUG
                        break;
                    }
                    if (it->index == currentNode.lowest) {
                        break;
                    }
                }
            }
        }
        currentNode.nextEdge++;
        m_stack->at(stackPosition).nextEdge++;
    }
    if (currentNode.index == currentNode.lowest) {
        DEVINFO(D::PFIMAKER, "Creating block #" << m_mmBlocks->size());
        int allBlocks = 0;
        for (std::vector<int>::iterator it = m_mmBlocks->begin(); it < m_mmBlocks->end(); it++) {
            allBlocks += *it;
        }
        m_mmBlocks->push_back(0);
        int lastIndex;
        do {
            lastIndex = m_stack->back().index;
            //TODO Ezt a torlest okosabban kene megoldani
            for (std::vector<int>::iterator nodeIt = m_mmGraphIn->at(lastIndex).begin(); nodeIt < m_mmGraphIn->at(
                    lastIndex).end(); nodeIt++) {
                DEVINFO(D::PFIMAKER, "Searching node " << *nodeIt);
                for (std::vector<int>::iterator searchIt = m_mmGraphOut->at(*nodeIt).begin(); searchIt
                        < m_mmGraphOut->at(*nodeIt).end(); searchIt++) {
                    DEVINFO(D::PFIMAKER, "Outgoing edge " << *searchIt);
                    if (*searchIt == lastIndex) {
                        m_mmGraphOut->at(*nodeIt).erase(searchIt);
                        break;
                    }
                }
            }
            m_mmGraphIn->at(lastIndex).clear();
            DEVINFO(D::PFIMAKER, "Last index in the stack is " << lastIndex);
            m_orderedIndex->at(lastIndex) = true;
            swapRows(m_columnSwapLog->at(lastIndex), allBlocks);
            swapColumns(m_columnSwapLog->at(lastIndex), allBlocks);
            m_mmBlocks->back()++;
            allBlocks++;
            DEVINFO(D::PFIMAKER, "Block " << m_mmBlocks->size() - 1 << " now contains node " << lastIndex);
            m_stack->pop_back();
#ifndef NDEBUG
//            printStack();
#endif //!NDEBUG
        } while (lastIndex != currentNode.index);
    } else {
        DEVINFO(D::PFIMAKER, "Node #" << currentNode.index << " is returning with " << currentNode.lowest);
        return currentNode.lowest;
    }
    return -1;
}

void PfiBasis::createBlockTriangular() {
    buildMM();

    clock_t transversalStart = clock();
    findTransversal();
    clock_t transversalStop = clock();
    cl_transversal += transversalStop - transversalStart;

    createGraph();

    tarjan();

#ifndef NDEBUG
//    printMM();
//    printBlocks();
#endif //!NDEBUG
}

bool PfiBasis::nontriangularCheck(int& rowindex, const Vector* currentColumn, int blockNum) {
    std::vector<int> activeRows;
    std::vector<int> goodRows;
    Numerical::Double nontriangularMax = 0;
    int previousBlocks = 0;
    switch (nontriangularPivotRule) {
    case NONE:
        if (Numerical::isZero(currentColumn->at(rowindex))) {
            rowindex = -1;
            return false;
        } else {
            return true;
        }
        break;
    case THRESHOLD:
        //Ha szamoltunk blokkokat
        if (blockNum != -1) {
            //Megnezzuk hogy korabbi blokkok mennyi sort & oszlopot foglalnak
            for (int i = 0; i < blockNum; i++) {
                previousBlocks += m_mmBlocks->at(i);
            }
//            LPINFO("previousBlocks: "<<previousBlocks);
            //Activerows-ba kigyujtjuk a blokkhoz tartozo nemnullakat
            Vector::NonzeroIterator it = currentColumn->beginNonzero();
            Vector::NonzeroIterator itend = currentColumn->endNonzero();
            for (; it < itend; it++) {
                if (m_rowCounts.at(it.getIndex()) > -1) {
                    //Ha MM-beli nemnullat talaltunk
                    for (int i = previousBlocks; i < previousBlocks + m_mmBlocks->at(blockNum); i++) {
                        //Ha a nemnulla a blokkon belul van
                        if ((int) it.getIndex() == m_mmRowIndices->at(m_rowSwapHash->at(i))) {
                            //TODO: Ezt hatekonyabban is lehetne
                            activeRows.push_back(it.getIndex());
                            break;
                        }
                    }
                }
            }
        }
        //Ha nem szamoltunk blokkokat
        else {
            Vector::NonzeroIterator it = currentColumn->beginNonzero();
            Vector::NonzeroIterator itend = currentColumn->endNonzero();
            for (; it < itend; it++) {
                if (m_rowCounts.at(it.getIndex()) > -1) {
                    activeRows.push_back(it.getIndex());
                }
            }
        }
        //Megnezzuk mi a legnagyobb elem
        //TODO Ezt az elozo lepes is ki tudja szamolni
        for (std::vector<int>::iterator it = activeRows.begin(); it < activeRows.end(); it++) {
            if (currentColumn->at(*it) > 0) {
                if (currentColumn->at(*it) > nontriangularMax) {
                    nontriangularMax = currentColumn->at(*it);
                }
            } else {
                if (-(currentColumn->at(*it)) > nontriangularMax) {
                    nontriangularMax = -(currentColumn->at(*it));
                }
            }
        }

        //Levalogatjuk a thresholdnak megfeleloket
        for (std::vector<int>::iterator it = activeRows.begin(); it < activeRows.end(); it++) {
            if (currentColumn->at(*it) > 0) {
                if (currentColumn->at(*it) > threshold * nontriangularMax) {
                    goodRows.push_back(*it);
                }
            } else {
                if (-(currentColumn->at(*it)) > threshold * nontriangularMax) {
                    goodRows.push_back(*it);
                }
            }
        }
        //Choose the sparsest row among the candidates
        if (goodRows.size() > 0) {
            rowindex = goodRows[0];
            for (std::vector<int>::iterator it = goodRows.begin(); it < goodRows.end(); it++) {
                if (m_rowCounts.at(*it) < m_rowCounts.at(rowindex)) {
                    rowindex = *it;
                }
            }
            //			LPINFO("chosen row: "<<rowindex<<" with value: "<<currentColumn->at(rowindex));
            return true;
        } else {
            rowindex = -1;
            return false;
        }
        break;
    default:
        return true;
    }
    return true;
}



void PfiBasis::printCounts() const {
#ifndef NDEBUG
    DEVINFO(D::PFIMAKER, "Row counts: ");
    for (std::vector<int>::const_iterator it = m_rowCounts.begin(); it < m_rowCounts.end(); it++) {
        DEVINFO(D::PFIMAKER, "Row " << it - m_rowCounts.begin() << " has " << *it << " nonzeros");
    }
    DEVINFO(D::PFIMAKER, "Column counts: ");
    for (std::vector<int>::const_iterator it = m_columnCounts.begin(); it < m_columnCounts.end(); it++) {
        DEVINFO(D::PFIMAKER, "Column " << it - m_columnCounts.begin() << " has " << *it << " nonzeros");
    }
#endif //!NDEBUG
    return;
}

void PfiBasis::printMM() const {
#ifndef NDEBUG
    LPWARNING( "MM pattern by rows");
    for (std::vector<Vector>::iterator it = m_mmRows->begin(); it < m_mmRows->end(); it++) {
        std::string s;
        for (int i = 0; i < (int) it->length(); i++) {
            s += Numerical::equals(it->at(i), 0) ? "-" : "X";
        }
        LPWARNING( s);
    }
//    LPWARNING( "MM pattern by columns");
//    for (int i = 0; i < (int) m_mmColumns->size(); i++) {
//        std::string s;
//        for (std::vector<Vector>::iterator it = m_mmColumns->begin(); it < m_mmColumns->end(); it++) {
//            s += Numerical::equals(it->at(i), 0) ? "-" : "X";
//        }
//        LPWARNING( s);
//    }
//    LPWARNING( "MM pattern with values");
//    for (std::vector<int>::iterator rowIt = m_rowSwapHash->begin(); rowIt < m_rowSwapHash->end(); rowIt++) {
//        int rowindex = m_mmRowIndices->at(*rowIt);
//        std::string s;
//        for (std::vector<int>::iterator columnIt = m_columnSwapHash->begin(); columnIt < m_columnSwapHash->end(); columnIt++) {
//            int columnindex = m_mmColumnIndices->at(*columnIt);
//            s += Numerical::equals(m_basicColumns.at(columnindex)->at(rowindex), 0) ? "-" : "X";
//        }
//        LPWARNING( s);
//    }
//    LPWARNING( "MM pattern with values without equals");
//    for (std::vector<int>::iterator rowIt = m_rowSwapHash->begin(); rowIt < m_rowSwapHash->end(); rowIt++) {
//        int rowindex = m_mmRowIndices->at(*rowIt);
//        std::string s;
//        for (std::vector<int>::iterator columnIt = m_columnSwapHash->begin(); columnIt < m_columnSwapHash->end(); columnIt++) {
//            int columnindex = m_mmColumnIndices->at(*columnIt);
//            s += m_basicColumns.at(columnindex)->at(rowindex) == 0 ? "-" : "X";
//        }
//        LPWARNING( s);
//    }

#endif //!NDEBUG
}

void PfiBasis::printSwapHashes() const {
#ifndef NDEBUG
    DEVINFO(D::PFIMAKER, "Row swap hash:");
    for (std::vector<int>::iterator it = m_rowSwapHash->begin(); it < m_rowSwapHash->end(); it++) {
        DEVINFO(D::PFIMAKER, "    " << *it);
    }
    DEVINFO(D::PFIMAKER, "Column swap hash:");
    for (std::vector<int>::iterator it = m_columnSwapHash->begin(); it < m_columnSwapHash->end(); it++) {
        DEVINFO(D::PFIMAKER, "    " << *it);
    }
#endif //!NDEBUG
}

void PfiBasis::printGraph() const {
#ifndef NDEBUG
    //Print the outgoing edges
    DEVINFO(D::PFIMAKER, "Outgoing graph edges");
    for (std::vector<std::vector<int> >::iterator it = m_mmGraphOut->begin(); it < m_mmGraphOut->end(); it++) {
        DEVINFO(D::PFIMAKER, "Node " << it - m_mmGraphOut->begin() << ":");
        for (std::vector<int>::iterator nodeIt = it->begin(); nodeIt < it->end(); nodeIt++) {
            DEVINFO(D::PFIMAKER, "    " << *nodeIt);
        }
    }
    //Print the incoming edges
    DEVINFO(D::PFIMAKER, "Incoming graph edges");
    for (std::vector<std::vector<int> >::iterator it = m_mmGraphIn->begin(); it < m_mmGraphIn->end(); it++) {
        DEVINFO(D::PFIMAKER, "Node " << it - m_mmGraphIn->begin() << ":");
        for (std::vector<int>::iterator nodeIt = it->begin(); nodeIt < it->end(); nodeIt++) {
            DEVINFO(D::PFIMAKER, "    " << *nodeIt);
        }
    }
#endif //!NDEBUG
}

void PfiBasis::printStack() const {
#ifndef NDEBUG
    //Print the stack
    DEVINFO(D::PFIMAKER, "Tarjan algorithm stack (Top --> Bottom):");
    DEVINFO(D::PFIMAKER, "index \tlowest\t nextEdge");
    for (std::vector<PathNode>::reverse_iterator it = m_stack->rbegin(); it < m_stack->rend(); it++) {
        DEVINFO(D::PFIMAKER, it->index << " \t " << it->lowest << " \t " << it->nextEdge);
    }
#endif //!NDEBUG
}

void PfiBasis::printBlocks() const {
#ifndef NDEBUG
    //Print the outgoing edges
    LPWARNING( "Block sizes:");
    for (std::vector<int>::iterator it = m_mmBlocks->begin(); it < m_mmBlocks->end(); it++) {
        LPWARNING( "Block #" << it - m_mmBlocks->begin() << ": " << *it);
    }
    //printMM();
#endif //!NDEBUG
}

void PfiBasis::printStatistics() const {
    LPINFO("INVERSION FINISHED");
    LPINFO("Run parameters during the inversion:");
    switch (nontriangularMethod) {
    case SEARCH:
        LPINFO("Used non-triangular method: SEARCH")
        ;
        break;
    case BLOCK_TRIANGULAR:
        LPINFO("Used non-triangular method: BLOCK_TRIANGULAR")
        ;
        break;
    case BLOCK_ORDERED_TRIANGULAR:
        LPINFO("Used non-triangular method: BLOCK_ORDERED_TRIANGULAR")
        ;
        break;
    default:
        LPWARNING("No non-triangular method defined!")
        ;
        break;
    }
    switch (nontriangularPivotRule) {
    case NONE:
        LPINFO("Used non-triangular pivot rule: NONE")
        ;
        break;
    case THRESHOLD:
        LPINFO("Used non-triangular pivot rule: THRESHOLD")
        ;
        break;
    default:
        LPWARNING("No non-triangular pivot rule defined!")
        ;
        break;
    }
    LPINFO("Nonzero statistics: ");
    LPINFO("Nonzeros in the basis: " << m_basisNonzeros);
    LPINFO("Nonzeros in the inverse: " << m_inverseNonzeros);
    LPINFO("Fill in amount: " << ((Numerical::Double)m_inverseNonzeros / (Numerical::Double)m_basisNonzeros)*100.0 - 100 << "% )");
    LPINFO("Inversion time statistics: ");
    LPINFO("Total inversion time: " << ((Numerical::Double) (cl_inversion) / (Numerical::Double) CLOCKS_PER_SEC) << " seconds.");
    LPINFO("Active submatrix copy: " << ((Numerical::Double) (cl_copy) / (Numerical::Double) CLOCKS_PER_SEC) << " seconds.");

    LPINFO("Find transversal time:"<<(Numerical::Double) (cl_transversal) / (Numerical::Double) CLOCKS_PER_SEC << " seconds.");
    LPINFO("Create block triangular time:"<<(Numerical::Double) (cl_createBlockTriangular) / (Numerical::Double) CLOCKS_PER_SEC << " seconds.");
    LPINFO("Pivot time:"<<(Numerical::Double) (cl_pivot) / (Numerical::Double) CLOCKS_PER_SEC << " seconds.");
    LPINFO("Update columns time:"<<(Numerical::Double) (cl_updateColumns) / (Numerical::Double) CLOCKS_PER_SEC << " seconds.");

    LPINFO("R part inversion: " << ((Numerical::Double) (cl_invertR) / (Numerical::Double) CLOCKS_PER_SEC) << " seconds.");
    LPINFO("M part inversion: " << ((Numerical::Double) (cl_invertM) / (Numerical::Double) CLOCKS_PER_SEC) << " seconds.");
    LPINFO("C part inversion: " << ((Numerical::Double) (cl_invertC) / (Numerical::Double) CLOCKS_PER_SEC) << " seconds.");
}

void PfiBasis::printTransformationStatistics() const {
    LPINFO("Inversion count: "<<m_inversionCount);
    //		LPINFO("Transformation count in the last inversion: "<<m_transformationCount);
    LPINFO("Transformation count average: "<<m_transformationAverage);
    LPINFO("Average size of the non-triangular part:"<<m_mNumAverage);
    printStatistics();
}
