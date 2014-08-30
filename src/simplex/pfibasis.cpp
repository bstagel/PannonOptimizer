/**
 * @file pfibasis.cpp
 */

#include <string>

#include <simplex/pfibasis.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>

#include <utils/thirdparty/prettyprint.h>
#include <utils/architecture.h>

double expDiffSquareSum = 0;
double expDiffSum = 0;
int expCounter = 0;

double aprExpDiffSquareSum = 0;
double aprExpDiffSum = 0;
int aprExpCounter = 0;

thread_local int PfiBasis::m_inversionCount = 0;

PfiBasis::PfiBasis(const SimplexModel& model,
                   std::vector<int>* basisHead,
                   IndexList<const Numerical::Double*>* variableStates,
                   const Vector& basicVariableValues) :
    Basis(model, basisHead, variableStates, basicVariableValues),
    m_nontriangularMethod(static_cast<NONTRIANGULAR_METHOD>
                          (SimplexParameterHandler::getInstance().getIntegerParameterValue("nontriangular_method"))),
    m_nontriangularPivotRule(static_cast<NONTRIANGULAR_PIVOT_RULE>
                             (SimplexParameterHandler::getInstance().getIntegerParameterValue("nontriangular_pivot_rule"))),
    m_threshold(SimplexParameterHandler::getInstance().getDoubleParameterValue("pivot_threshold"))
{
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
    m_rowNonzeroIndices.clear();
    m_rowNonzeroIndices.resize(rowCount);
    m_basicColumnIndices.clear();
    m_basicColumnIndices.reserve(rowCount);
    //NEW//

    m_columnCounts.reserve(rowCount);
    m_rowCounts.resize(rowCount, 0);
    m_basisNewHead.resize(rowCount, -1);

#ifndef NDEBUG
    std::vector<char> headChecker(rowCount + columnCount, 0);
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); it++) {
        if (headChecker[*it] == 0) {
            headChecker[*it] = 1;
        } else {
            LPWARNING("Duplicate index in basis head: " << *it);
            throw PanOptException("Duplicate index in the basis head!");
        }
    }
#endif

    //Copy the active submatrix
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); it++) {
        if (*it >= (int) columnCount) {
            DEVINFO(D::PFIMAKER, "Logical variable found in basis head: y" << *it - columnCount);
            //Ignore logical columns from the inverse
            m_rowCounts[*it-columnCount] = -1;
            m_basisNewHead[*it-columnCount] = *it;
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
            if(m_rowCounts[rowIndex] != -1){
                m_rowCounts[rowIndex]++;
                m_rowNonzeroIndices[rowIndex].push_back(columnIndex);
            } else {
                m_columnCounts[columnIndex]--;
            }
        }
        if (maxColumnCount < m_columnCounts[columnIndex]) {
            maxColumnCount = m_columnCounts[columnIndex];
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
    expDiffSquareSum = 0;
    expDiffSum = 0;
    expCounter = 0;

    aprExpDiffSquareSum = 0;
    aprExpDiffSum = 0;
    aprExpCounter = 0;

    etaExpCount = 0;
    etaExpSquareSum = 0;
    etaExpSum = 0;

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
    copyBasis();

    DEVINFO(D::PFIMAKER, "Basis copied");

    //Invert the R, M, C parts separately
    invertR();
    findC();
    invertM();
    invertC();

    //Free the copied columns
    for(unsigned int i=0; i<m_basicColumnCopies.size(); i++){
        if(m_basicColumnCopies[i] != NULL){
            delete m_basicColumnCopies[i];
            m_basicColumnCopies[i] = NULL;
        }
    }

    //Verify the result of the inversion
    checkSingularity();

    //Update the basis head
    setNewHead();

    m_isFresh = true;

    //printStatistics();
    m_transformationAverage += (m_transformationCount - m_transformationAverage) / m_inversionCount;
}

void PfiBasis::append(const Vector & vector, int pivotRow, int incoming, Simplex::VARIABLE_STATE outgoingState) {
    //If the alpha vector comes in, then ftran is done already

    int outgoing = (*m_basisHead)[pivotRow];
    const Variable & outgoingVariable = m_model.getVariable(outgoing);

    if (outgoingState == Simplex::NONBASIC_AT_LB) {
        if(!Numerical::equal(*(m_variableStates->getAttachedData(outgoing)), outgoingVariable.getLowerBound(),1.0e-4)){
            LPERROR("Outgoing variable is rounded to its lower bound! "<<outgoing);
            LPERROR("Current value: " << setw(19) << scientific << setprecision(16) << *(m_variableStates->getAttachedData(outgoing)));
            LPERROR("Lower bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getLowerBound());
            LPERROR("Upper bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getUpperBound());
            cerr.unsetf(ios_base::floatfield);
        }
        pivot(vector, pivotRow);
        m_variableStates->move(outgoing,Simplex::NONBASIC_AT_LB, &(outgoingVariable.getLowerBound()));
    } else if (outgoingState == Simplex::NONBASIC_AT_UB) {
        if(!Numerical::equal(*(m_variableStates->getAttachedData(outgoing)), outgoingVariable.getUpperBound(),1.0e-4)){
            LPERROR("Outgoing variable is rounded to its upper bound! "<<outgoing);
            LPERROR("Current value: " << setw(19) << scientific << setprecision(16) << *(m_variableStates->getAttachedData(outgoing)));
            LPERROR("Lower bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getLowerBound());
            LPERROR("Upper bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getUpperBound());
            cerr.unsetf(ios_base::floatfield);
        }
        pivot(vector, pivotRow);
        m_variableStates->move(outgoing,Simplex::NONBASIC_AT_UB, &(outgoingVariable.getUpperBound()));
    } else if ( outgoingState == Simplex::NONBASIC_FIXED) {
        if(!Numerical::equal(*(m_variableStates->getAttachedData(outgoing)), outgoingVariable.getLowerBound(),1.0e-4)){
            LPERROR("Outgoing fixed variable is rounded to its bound! "<<outgoing);
            LPERROR("Current value: " << setw(19) << scientific << setprecision(16) << *(m_variableStates->getAttachedData(outgoing)));
            LPERROR("Bound: " << setw(19) << scientific << setprecision(16) << outgoingVariable.getLowerBound());
            cerr.unsetf(ios_base::floatfield);
        }
        pivot(vector, pivotRow);
        m_variableStates->move(outgoing,Simplex::NONBASIC_FIXED, &(outgoingVariable.getLowerBound()));
    } else {
        LPERROR("Invalid outgoing variable state!");
        cerr.unsetf(ios_base::floatfield);
        throw NumericalException(std::string("Invalid outgoing variable state"));
    }
    (*m_basisHead)[pivotRow] = incoming;
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
        Vector::scatter(Vector::sm_fullLengthVector, Vector::sm_fullLengthVectorLength, vector);
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

        if (iter->eta->m_vectorType == Vector::SPARSE_VECTOR) {
            Numerical::Double * ptrEta = iter->eta->m_data;
            unsigned int * ptrIndex = iter->eta->m_index;
            const unsigned int * ptrIndexEnd = ptrIndex + iter->eta->m_size;
            const unsigned int pivotPosition = iter->index;
            while (ptrIndex < ptrIndexEnd) {
                Numerical::Double & originalValue = denseVector[*ptrIndex];
                if (*ptrEta != 0.0) {
                    Numerical::Double val;
                    if (*ptrIndex != pivotPosition) {
                        val = Numerical::stableAddAbs(originalValue, pivotValue * *ptrEta);
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

        } else {
            Numerical::Double * ptrValue2 = iter->eta->m_data;
            Numerical::Double * ptrValue1 = denseVector;
            const Numerical::Double * ptrValueEnd = denseVector + vector.m_dimension;
            while (ptrValue1 < ptrValueEnd) {
                const Numerical::Double value = *ptrValue2;
                if (value != 0.0) {
                    const Numerical::Double val = Numerical::stableAddAbs(*ptrValue1, pivotValue * value);
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

Numerical::Double lostValueAdd(Numerical::Double a, Numerical::Double b) {
    Numerical::Double sum = a + b;
    // levonjuk a nagyobb abszolut erteku elem eredeti erteket
    Numerical::Double lost = sum - (Numerical::fabs(a) > Numerical::fabs(b) ? a : b);
    // ezt kivonjuk a kisebb abszolut erteku elem eredeti ertekebol
    return (Numerical::fabs(a) > Numerical::fabs(b) ? b : a) - lost;
}

void PfiBasis::FtranCheck(Vector & vector, Vector & checkVector, FTRAN_MODE mode) const
{
    __UNUSED(mode);
    checkVector = vector;
    //The ftran operation.

    Numerical::Double * denseVector;
    Numerical::Double * checkDenseVector;

    // 1. lepes: ha kell akkor atvaltjuk dense-re
    if (vector.m_vectorType == Vector::DENSE_VECTOR) {
        denseVector = vector.m_data;
        checkDenseVector = checkVector.m_data;
    } else {
        Vector::scatter(Vector::sm_fullLengthVector, Vector::sm_fullLengthVectorLength, vector);
        denseVector = Vector::sm_fullLengthVector;
    }
    checkDenseVector = alloc<Numerical::Double, 16>( vector.length() );
    unsigned int index;
    for (index = 0; index < vector.length(); index++) {
        double mul = ((rand() % 10000) / (double)1e15)*pow(1, index);
        checkDenseVector[index] = mul;

    }
    //panOptMemcpy(checkDenseVector, denseVector, vector.length() * sizeof(Numerical::Double));

    // 2. lepes: vegigmegyunk minden eta vektoron es elvegezzuk a hozzaadast
    std::vector<ETM>::const_iterator iter = m_basis->begin();
    const std::vector<ETM>::const_iterator iterEnd = m_basis->end();

    Numerical::Double plusMinus = -1.0;

    for (; iter != iterEnd; iter++) {
        plusMinus *= -1.0;
        const Numerical::Double pivotValue = denseVector[ iter->index ];
        const Numerical::Double checkPivotValue = checkDenseVector[ iter->index ];

        if (pivotValue == 0.0) {
            continue;
        }

        if (iter->eta->m_vectorType == Vector::DENSE_VECTOR) {
            Numerical::Double * ptrValue2 = iter->eta->m_data;
            Numerical::Double * ptrValue1 = denseVector;
            Numerical::Double * checkValue = checkDenseVector;
            const Numerical::Double * ptrValueEnd = denseVector + vector.m_dimension;
            while (ptrValue1 < ptrValueEnd) {
                const Numerical::Double value = *ptrValue2;
                if (value != 0.0) {
                    Numerical::Double val = Numerical::stableAddAbs(*ptrValue1, pivotValue * value);
                    //LPINFO(*ptrValue1 << " + " << (pivotValue * value) << " = " << val);
                    if (*ptrValue1 == 0.0 && val != 0.0) {
                        vector.m_nonZeros++;
                    } else if (*ptrValue1 != 0.0 && val == 0.0) {
                        vector.m_nonZeros--;
                    }
                    *ptrValue1 = val;

                    Numerical::Double mul = 1.0 - ((rand() % 10000) / (double)1e12)*plusMinus;
                    mul = 1.0;
                    val = Numerical::stableAddAbs(*checkValue, checkPivotValue * value * mul);
                    //cout << val << "   " << lostValueAdd(*checkValue, checkPivotValue * value) << endl;
                    mul = 1.0 - ((rand() % 10000) / (double)1e12)*plusMinus;
                    //mul = 1.0;
                    *checkValue = val + ((rand() % 10000) / (double)1e11)*pow(-1, plusMinus);;
                }
                ptrValue1++;
                ptrValue2++;
                checkValue++;
            }
            // std::cin.get();
            //A vegen lerendezzuk a pivot poziciot is:
            Numerical::Double val = pivotValue * iter->eta->m_data[iter->index];
            if (denseVector[iter->index] == 0.0 && val != 0.0) {
                vector.m_nonZeros++;
            } else if (denseVector[iter->index] != 0.0 && val == 0.0) {
                vector.m_nonZeros--;
            }
            denseVector[iter->index] = val;

            Numerical::Double mul = 1.0 - ((rand() % 10000) / (double)1e11)*plusMinus;
            val = checkPivotValue * iter->eta->m_data[iter->index];
            checkDenseVector[iter->index] = val * mul;

        } else {
            Numerical::Double * ptrEta = iter->eta->m_data;
            unsigned int * ptrIndex = iter->eta->m_index;
            const unsigned int * ptrIndexEnd = ptrIndex + iter->eta->m_size;
            const unsigned int pivotPosition = iter->index;
            while (ptrIndex < ptrIndexEnd) {
                Numerical::Double & originalValue = denseVector[*ptrIndex];
                Numerical::Double & originalCheckValue = checkDenseVector[*ptrIndex];
                if (*ptrEta != 0.0) {
                    Numerical::Double val;
                    Numerical::Double checkVal;
                    if (*ptrIndex != pivotPosition) {
                        val = Numerical::stableAddAbs(originalValue, pivotValue * *ptrEta);
                        Numerical::Double mul = 1.0 - ((rand() % 10000) / (double)1e12)*plusMinus;
                        mul = 1.0;
                        checkVal = Numerical::stableAddAbs(originalCheckValue, checkPivotValue * *ptrEta * mul);
                        if (originalValue == 0.0 && val != 0.0) {
                            vector.m_nonZeros++;
                        } else if (originalValue != 0.0 && val == 0.0) {
                            vector.m_nonZeros--;
                        }
                    } else {
                        val = pivotValue * *ptrEta;
                        checkVal = checkPivotValue * *ptrEta;
                    }

                    //                    Numerical::Double mul = 1.0 - ((rand() % 10000) / (double)1e11)*plusMinus;
                    //                    mul = 1.0;
                    originalValue = val;
                    originalCheckValue = checkVal + ((rand() % 10000) / (double)1e11)*pow(-1, plusMinus);

                }
                ptrIndex++;
                ptrEta++;
            }
        }
    }

    // 3. elmentjuk az ellenorzo vektort
    unsigned int checkNonzCounter = 0;
    for (index = 0; index < vector.length(); index++) {
        if (checkDenseVector[index] != 0.0) {
            checkNonzCounter++;
        }
    }
    checkVector.prepareForData(checkNonzCounter, vector.length());
    for (index = 0; index < vector.length(); index++) {
        if (checkDenseVector[index] != 0.0) {
            checkVector.newNonZero(checkDenseVector[index], index);
        }
    }

    // 4. lepes: ha kell akkor v-t atvaltani, adatokat elmenteni    Vector::VECTOR_TYPE newType;

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

    // 1. convert the input vector to dense form if necessary
    if (vector.m_vectorType == Vector::DENSE_VECTOR) {
        denseVector = vector.m_data;
    } else {
        Vector::scatter(Vector::sm_fullLengthVector, Vector::sm_fullLengthVectorLength, vector);
        denseVector = Vector::sm_fullLengthVector;
    }

    // 2. perform the dot products
    //std::vector<ETM>::const_reverse_iterator iter = m_basis->rbegin();
    //const std::vector<ETM>::const_reverse_iterator iterEnd = m_basis->rend();
    ETM * iterEnd = m_basis->data() - 1;
    ETM * iter = iterEnd + m_basis->size();

    unsigned int etaIndex = 0;
    for (; iter != iterEnd; iter--, etaIndex++) {
        unsigned int nonZeros = vector.nonZeros();

        Numerical::Summarizer summarizer;
        Numerical::Double dotProduct = 0;

        if (iter->eta->m_vectorType == Vector::SPARSE_VECTOR) {
            Numerical::Double * ptrValue = iter->eta->m_data;
            unsigned int * ptrIndex = iter->eta->m_index;
            const unsigned int * ptrIndexEnd = ptrIndex + iter->eta->m_size;
            // if the input vector has less nonzeros than the eta vector,
            // this implementation can be faster
            if (nonZeros < iter->eta->m_size) {
                while (ptrIndex < ptrIndexEnd && nonZeros) {
                    const Numerical::Double value = denseVector[*ptrIndex];
                    if (value != 0.0) {
                        nonZeros--;
                        summarizer.add(value * *ptrValue);

                    }
                    ptrIndex++;
                    ptrValue++;
                }
            } else {
                while (ptrIndex < ptrIndexEnd) {
                    const Numerical::Double value = denseVector[*ptrIndex];
                    if (value != 0.0) {
                        summarizer.add(value * *ptrValue);
                    }
                    ptrIndex++;
                    ptrValue++;
                }
            }

            dotProduct = summarizer.getResult();
        } else {
            Numerical::Double * ptrValue2 = iter->eta->m_data;
            Numerical::Double * ptrValue1 = denseVector;
            const Numerical::Double * ptrValueEnd = denseVector + vector.m_dimension;

            while (ptrValue1 < ptrValueEnd && nonZeros) {
                // if the input vector has less nonzeros than the eta vector,
                // this implementation can be faster
                const Numerical::Double value = *ptrValue1;
                if (value != 0.0) {
                    nonZeros--;

                    summarizer.add(value * *ptrValue2);
                }
                ptrValue1++;
                ptrValue2++;
            }
            dotProduct = summarizer.getResult();

        }

        // store the dot product, and update the nonzero counter
        const int pivot = iter->index;
        if (denseVector[pivot] != 0.0 && dotProduct == 0.0) {
            vector.m_nonZeros--;
        }
        if (denseVector[pivot] == 0.0 && dotProduct != 0.0) {
            vector.m_nonZeros++;
        }
        denseVector[pivot] = dotProduct;
    }

    // 3. store the result in the output vector
    Vector::VECTOR_TYPE newType;
    // operating vector and output are the same
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
        // build the result vector, if the original vector was sparse
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
    std::list<int>::iterator it = m_rowNonzeroIndices[rowindex].begin();
    std::list<int>::iterator itend = m_rowNonzeroIndices[rowindex].end();

    for (; it != itend; it++) {
        if (*it != (int) columnindex) {
            if(m_basicColumnCopies[*it]==NULL){
                m_basicColumnCopies[*it] = new Vector(*(m_basicColumns[*it]));
                m_basicColumns[*it] = m_basicColumnCopies[*it];
            }
            m_transformationCount += m_basis->back().eta->nonZeros();
            //Remove everything
            Vector::NonzeroIterator columnIt = m_basicColumnCopies[*it]->beginNonzero();
            Vector::NonzeroIterator columnItend = m_basicColumnCopies[*it]->endNonzero();
            for (; columnIt < columnItend; columnIt++) {
                if(columnIt.getIndex() != rowindex && m_rowCounts[columnIt.getIndex()] > -1){
                    m_rowNonzeroIndices[columnIt.getIndex()].remove(*it);
                    m_rowCounts[columnIt.getIndex()]--;
                }
            }
            //            LPINFO("UPDATING COLUMN: "<<*m_basicColumnCopies.at(*it));
            m_basicColumnCopies[*it]->elementaryFtran(*(m_basis->back().eta), m_basis->back().index);

            //            LPINFO("UPDATED COLUMN: "<<*m_basicColumnCopies.at(*it));
            //Add the changes back
            int newColumnCount = 0;
            columnIt = m_basicColumnCopies[*it]->beginNonzero();
            columnItend = m_basicColumnCopies[*it]->endNonzero();
            for (; columnIt < columnItend; columnIt++) {
                if(columnIt.getIndex() != rowindex && m_rowCounts[columnIt.getIndex()] > -1){
                    m_rowNonzeroIndices[columnIt.getIndex()].push_back(*it);
                    m_rowCounts[columnIt.getIndex()]++;
                    newColumnCount++;
                }
            }

            //Update the column count
            m_columnCounts[*it] = newColumnCount;
        }
    }
}

void PfiBasis::pivot(const Vector& column, int pivotRow) {
    ETM newETM;
    newETM.eta = createEta(column, pivotRow);
    newETM.index = pivotRow;
    m_inverseNonzeros += newETM.eta->nonZeros();
    m_basis->push_back(newETM);
}

void PfiBasis::invertR() {

    //The upper triangular part is called R part
    DEVINFO(D::PFIMAKER, "Search for the R part and invert it");
    unsigned int rNum = 0;

    if (m_rowCountIndexList.getPartitionCount() > 1) {
        while (m_rowCountIndexList.firstElement(1) != -1 ) {
            int rowindex = m_rowCountIndexList.firstElement(1);
            //This part searches for rows with row count 1 and order them to the upper triangular part
            int columnindex = m_rowNonzeroIndices[rowindex].front();

            const Vector *currentColumn = m_basicColumns[columnindex];
            //Invert the chosen R column
            DEVINFO(D::PFIMAKER, "Inverting R column " << columnindex << " with pivot row " << rowindex);

            pivot(*currentColumn, rowindex);
            m_basisNewHead[rowindex] = m_basicColumnIndices[columnindex];

            //Update the row lists and row counts
            Vector::NonzeroIterator it = currentColumn->beginNonzero();
            Vector::NonzeroIterator itend = currentColumn->endNonzero();
            for (; it < itend; it++) {
                int index = it.getIndex();
                //If the row of the iterated element is still active
                if (m_rowCounts[index] > -1) {
                    m_rowCounts[index]--;
                    m_rowCountIndexList.move(index, m_rowCounts[index]);
                    m_rowNonzeroIndices[index].remove(columnindex);
                }
            }

            //Set the column count to zero to represent that which column has been chosen.
            m_columnCounts[columnindex] = -1;
            m_columnCountIndexList.remove(columnindex);
            m_rowCounts[rowindex] = -1;
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
            Vector::NonzeroIterator it = m_basicColumns[columnindex]->beginNonzero();
            Vector::NonzeroIterator itend = m_basicColumns[columnindex]->endNonzero();
            for (; it < itend; it++) {
                if (m_rowCounts[it.getIndex()] > 0) {
                    rowindex = it.getIndex();
                    break;
                }
            }
            const Vector *currentColumn = m_basicColumns[columnindex];

            //Store the chosen columns
            m_cColumns->push_back(currentColumn);
            m_cPivotIndexes->push_back(rowindex);
            m_basisNewHead[rowindex] = m_basicColumnIndices[columnindex];
            DEVINFO(D::PFIMAKER, "Submatrix column " << columnindex << " stored for C part with pivot index " << rowindex);

            //Update the column counts but the columns remains untouched
            std::list<int>::iterator listIt = m_rowNonzeroIndices[rowindex].begin();
            std::list<int>::iterator listItend = m_rowNonzeroIndices[rowindex].end();
            for (; listIt != listItend; listIt++) {
                int index = *listIt;
                if (m_columnCounts[index] > -1) {
                    m_columnCounts[index]--;
                    m_columnCountIndexList.move(index, m_columnCounts[index]);
                }
            }
            m_rowNonzeroIndices[rowindex].clear();
            //Set the row count set to zero to represent that which row has been chosen.
            m_rowCounts[rowindex] = -1;
            m_rowCountIndexList.remove(rowindex);
            m_columnCounts[columnindex] = -1;
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
    switch (m_nontriangularMethod) {
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
                    int columnindex = m_rowNonzeroIndices[rowindex].front();
                    const Vector* currentColumn = m_basicColumns[columnindex];
                    if (nontriangularCheck(rowindex, currentColumn, -1)) {
                        //Invert the chosen M column
                        DEVINFO(D::PFIMAKER, "Inverting M column " << columnindex << " with pivot row " << rowindex);
                        pivot(*currentColumn, rowindex);
                        containsOne = true;
                        m_basisNewHead[rowindex] = m_basicColumnIndices[columnindex];
                        //Update the remaining columns
                        updateColumns(rowindex, columnindex);
                        //Update the row lists and row counts
                        Vector::NonzeroIterator vectorIt = currentColumn->beginNonzero();
                        Vector::NonzeroIterator vectorItend = currentColumn->endNonzero();
                        for (; vectorIt < vectorItend; vectorIt++) {
                            if (m_rowCounts[vectorIt.getIndex()] >= 0) {
                                m_rowCounts[vectorIt.getIndex()]--;
                                m_rowNonzeroIndices[vectorIt.getIndex()].remove(columnindex);
                            }
                        }
                        //Update the column counts too
                        std::list<int>::iterator listIt = m_rowNonzeroIndices[rowindex].begin();
                        std::list<int>::iterator listItend = m_rowNonzeroIndices[rowindex].end();
                        for (; listIt != listItend; listIt++) {
                            int index = *listIt;
                            if (m_columnCounts[index] > -1) {
                                m_columnCounts[index]--;
                            }
                        }
                        //Set the column and row count to zero to represent that which column and row has been chosen.
                        m_rowCounts[rowindex] = -1;
                        m_columnCounts[columnindex] = -1;
                    } else {
                        LPWARNING("Non-triangular pivot position is numerically unstable, ignoring column:" << columnindex << ")");
                        //Update the row lists and row counts
                        Vector::NonzeroIterator it = currentColumn->beginNonzero();
                        Vector::NonzeroIterator itend = currentColumn->endNonzero();
                        for (; it < itend; it++) {
                            int index = it.getIndex();
                            //If the row of the iterated element is still active
                            if (m_rowCounts[index] > -1) {
                                m_rowCounts[index]--;
                                m_rowNonzeroIndices[index].remove(columnindex);
                            }
                        }
                        //Set the column count to zero to represent that which column is unstable.
                        m_columnCounts[columnindex] = -1;
                    }
                    mNum++;
                }
            }
        }
        break;

    }
    case BLOCK_TRIANGULAR:{
        createBlockTriangular();
        if (m_mmBlocks->size() > 0) {
            int currentBlock = -1;
            int currentBlockSize = 0;
            for (std::vector<int>::iterator rowIt = m_rowSwapHash->begin(); rowIt < m_rowSwapHash->end(); rowIt++) {

                if (currentBlockSize == 0) {
                    currentBlock++;
                    currentBlockSize = (*m_mmBlocks)[currentBlock];
                    //LPINFO("Iterating block #"<<currentBlock<<" with block size: "<<currentBlockSize);
                }
                currentBlockSize--;
                //Rowindex: Ki volt eredetileg aki az iteratoradik sor
                int rowindex = (*m_mmRowIndices)[*rowIt];
                //Columnindex: Ki volt eredetileg az iteratoradik oszlop
                int columnindex = (*m_mmColumnIndices)[(*m_columnSwapHash)[rowIt - m_rowSwapHash->begin()]];
                const Vector* currentColumn = m_basicColumns[columnindex];
                if (nontriangularCheck(rowindex, currentColumn, currentBlock)) {
                    DEVINFO(D::PFIMAKER, "Inverting M column " << columnindex << " with pivot row " << rowindex);
                    //Invert the chosen M column

                    pivot(*currentColumn, rowindex);
                    m_basisNewHead[rowindex] = m_basicColumnIndices[columnindex];
                    //Update the remaining columns
                    updateColumns(rowindex, columnindex);

                    //Update the row lists and row counts
                    Vector::NonzeroIterator it = currentColumn->beginNonzero();
                    Vector::NonzeroIterator itend = currentColumn->endNonzero();
                    for (; it < itend; it++) {
                        int index = it.getIndex();
                        //If the row of the iterated element is still active
                        if (m_rowCounts[index] > -1) {
                            m_rowCounts[index]--;
                            m_rowNonzeroIndices[index].remove(columnindex);
                        }
                    }
                    //Update the column counts too
                    std::list<int>::iterator listIt = m_rowNonzeroIndices[rowindex].begin();
                    std::list<int>::iterator listItend = m_rowNonzeroIndices[rowindex].end();
                    for (; listIt != listItend; listIt++) {
                        int index = *listIt;
                        if (m_columnCounts[index] > -1) {
                            m_columnCounts[index]--;
                        }
                    }
                    m_rowNonzeroIndices[rowindex].clear();
                    //Set the column and row count to zero to represent that which column and row has been chosen.
                    m_rowCounts[rowindex] = -1;
                    m_columnCounts[columnindex] = -1;


                } else {
                    LPWARNING("Non-triangular pivot position is numerically unstable, ignoring column:" << columnindex << ")");
                    //Update the row lists and row counts
                    Vector::NonzeroIterator it = currentColumn->beginNonzero();
                    Vector::NonzeroIterator itend = currentColumn->endNonzero();
                    for (; it < itend; it++) {
                        int index = it.getIndex();
                        //If the row of the iterated element is still active
                        if (m_rowCounts[index] > -1) {
                            m_rowCounts[index]--;
                            m_rowNonzeroIndices[index].remove(columnindex);
                        }
                    }
                    //Set the column count to zero to represent that which column is unstable.
                    m_columnCounts[columnindex] = -1;
                }
                mNum++;
            }
        }

        break;
    }
    case BLOCK_ORDERED_TRIANGULAR:{
        createBlockTriangular();
        {
            int blockStart = 0;
            for (std::vector<int>::iterator blockIt = m_mmBlocks->begin(); blockIt < m_mmBlocks->end(); blockIt++) {
                int currentBlock = blockIt - m_mmBlocks->begin();
                int currentBlockSize = *blockIt;
                std::multimap<int, int> columnOrders;
                int i;
                for (i = blockStart; i < blockStart + currentBlockSize; i++) {
                    columnOrders.insert(
                                std::make_pair((int) (*m_mmColumns)[i].nonZeros(),
                                               (*m_mmColumnIndices)[(*m_columnSwapHash)[i]]));
                }
                for (std::multimap<int, int>::iterator colIt = columnOrders.begin(); colIt != columnOrders.end(); colIt++) {
                    blockStart++;
                    int rowindex = -1;
                    int columnindex = colIt->second;
                    const Vector* currentColumn = m_basicColumns[columnindex];
                    if (nontriangularCheck(rowindex, currentColumn, currentBlock)) {
                        DEVINFO(D::PFIMAKER, "Inverting M column " << columnindex << " with pivot row " << rowindex);
                        //Invert the chosen M column

                        pivot(*currentColumn, rowindex);
                        m_basisNewHead[rowindex] = m_basicColumnIndices[columnindex];
                        //Update the remaining columns
                        updateColumns(rowindex, columnindex);

                        //Update the row lists and row counts
                        Vector::NonzeroIterator it = currentColumn->beginNonzero();
                        Vector::NonzeroIterator itend = currentColumn->endNonzero();
                        for (; it < itend; it++) {
                            int index = it.getIndex();
                            //If the row of the iterated element is still active
                            if (m_rowCounts[index] > -1) {
                                m_rowCounts[index]--;
                                m_rowNonzeroIndices[index].remove(columnindex);
                            }
                        }
                        //Update the column counts too
                        std::list<int>::iterator listIt = m_rowNonzeroIndices[rowindex].begin();
                        std::list<int>::iterator listItend = m_rowNonzeroIndices[rowindex].end();
                        for (; listIt != listItend; listIt++) {
                            int index = *listIt;
                            if (m_columnCounts[index] > -1) {
                                m_columnCounts[index]--;
                            }
                        }
                        m_rowNonzeroIndices[rowindex].clear();
                        //Set the column and row count to zero to represent that which column and row has been chosen.
                        m_rowCounts[rowindex] = -1;
                        m_columnCounts[columnindex] = -1;
                    } else {
                        LPWARNING("Non-triangular pivot position is numerically unstable, ignoring column:" << columnindex << ")");
                        //Update the row lists and row counts
                        Vector::NonzeroIterator it = currentColumn->beginNonzero();
                        Vector::NonzeroIterator itend = currentColumn->endNonzero();
                        for (; it < itend; it++) {
                            int index = it.getIndex();
                            //If the row of the iterated element is still active
                            if (m_rowCounts[index] > -1) {
                                m_rowCounts[index]--;
                                m_rowNonzeroIndices[index].remove(columnindex);
                            }
                        }
                        //Set the column count to zero to represent that which column is unstable.
                        m_columnCounts[columnindex] = -1;
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
                " with pivot row " << (*m_cPivotIndexes)[m_cColumns->rend() - 1 - it]);
        pivot(*(*it), (*m_cPivotIndexes)[m_cColumns->rend() - 1 - it]);
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
            rowMemory[it - m_rowCounts.begin()] = m_mmRowIndices->size() - 1;
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
        (*m_rowSwapHash)[i] = i;
        (*m_columnSwapHash)[i] = i;
        (*m_columnSwapLog)[i] = i;
        //     m_mmRows->at(i).setSparsityRatio(0.0);
        //   m_mmColumns->at(i).setSparsityRatio(0.0);
    }
    for (std::vector<int>::iterator it = m_mmColumnIndices->begin(); it < m_mmColumnIndices->end(); it++) {
        Vector::NonzeroIterator vectorIt = m_basicColumns[*it]->beginNonzero();
        Vector::NonzeroIterator vectorItend = m_basicColumns[*it]->endNonzero();
        for (; vectorIt < vectorItend; vectorIt++) {
            if (rowMemory[vectorIt.getIndex()] != -1) {
                int rowIndex = rowMemory[vectorIt.getIndex()];
                int columnIndex = it - m_mmColumnIndices->begin();
                //TODO: setnewnonzero -> preparefordata
                (*m_mmRows)[rowIndex].setNewNonzero(columnIndex, *vectorIt);
                (*m_mmColumns)[columnIndex].setNewNonzero(rowIndex, *vectorIt);
            }
        }
    }
    DEVINFO(D::PFIMAKER, "Nonzero pattern in MM built");
}

void PfiBasis::findTransversal() {
    DEVINFO(D::PFIMAKER, "Searching for transversal form");
    for (int i = 0; i < (int) m_rowSwapHash->size(); i++) {
        if ((*m_mmRows)[i].at(i) == 0) {
            //Find some with bigger index
            bool nextRow = false;
            DEVINFO(D::PFIMAKER, "Searching for diagonal nonzero at row " << i);
            Vector::NonzeroIterator vectorIt = (*m_mmColumns)[i].beginNonzero();
            Vector::NonzeroIterator vectorItend = (*m_mmColumns)[i].endNonzero();
            for (; vectorIt < vectorItend; vectorIt++) {
                if ((int) vectorIt.getIndex() > i) {
                    nextRow = true;
                    DEVINFO(D::PFIMAKER, "Nonzero found below index " << i);
                    swapRows(vectorIt.getIndex(), i);
                    break;
                } else if ((*m_mmRows)[vectorIt.getIndex()].at(vectorIt.getIndex()) == 0) {
                    nextRow = true;
                    DEVINFO(D::PFIMAKER, "Nonzero found in singular row below index " << i);
                    swapRows(vectorIt.getIndex(), i);
                    break;
                }
            }
            if (!nextRow) {
                DEVINFO(D::PFIMAKER, "Nonzero not found, search columns");
                std::vector<int> searchedRows;
                vectorIt = (*m_mmColumns)[i].beginNonzero();
                vectorItend = (*m_mmColumns)[i].endNonzero();
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

    int rowHashIndex1 = (*m_rowSwapHash)[rowIndex1];
    int rowHashIndex2 = (*m_rowSwapHash)[rowIndex2];
    //TODO: gyorsitani
    Vector row1 = (*m_mmRows)[rowIndex1];
    Vector row2 = (*m_mmRows)[rowIndex2];

    for (Vector::NonzeroIterator vectorIt = row1.beginNonzero(); vectorIt < row1.endNonzero(); vectorIt++) {
        (*m_mmColumns)[vectorIt.getIndex()].set(rowIndex1, 0);
    }
    for (Vector::NonzeroIterator vectorIt = row2.beginNonzero(); vectorIt < row2.endNonzero(); vectorIt++) {
        (*m_mmColumns)[vectorIt.getIndex()].set(rowIndex2, 0);
    }
    for (Vector::NonzeroIterator vectorIt = row1.beginNonzero(); vectorIt < row1.endNonzero(); vectorIt++) {
        (*m_mmColumns)[vectorIt.getIndex()].set(rowIndex2, *vectorIt);
    }
    for (Vector::NonzeroIterator vectorIt = row2.beginNonzero(); vectorIt < row2.endNonzero(); vectorIt++) {
        (*m_mmColumns)[vectorIt.getIndex()].set(rowIndex1, *vectorIt);
    }
    //TODO: JOCO Swap fuggveny a vektorba
    (*m_mmRows)[rowIndex1] = row2;
    (*m_mmRows)[rowIndex2] = row1;

    (*m_rowSwapHash)[rowIndex1] = rowHashIndex2;
    (*m_rowSwapHash)[rowIndex2] = rowHashIndex1;
    //printSwapHashes();
}

void PfiBasis::swapColumns(int columnIndex1, int columnIndex2) {
    DEVINFO(D::PFIMAKER, "Swapping column " << columnIndex1 << " and " << columnIndex2);

    int columnHashIndex1 = (*m_columnSwapHash)[columnIndex1];
    int columnHashIndex2 = (*m_columnSwapHash)[columnIndex2];
    Vector column1 = (*m_mmColumns)[columnIndex1];
    Vector column2 = (*m_mmColumns)[columnIndex2];

    for (Vector::NonzeroIterator vectorIt = column1.beginNonzero(); vectorIt < column1.endNonzero(); vectorIt++) {
        (*m_mmRows)[vectorIt.getIndex()].set(columnIndex1, 0);
    }
    for (Vector::NonzeroIterator vectorIt = column2.beginNonzero(); vectorIt < column2.endNonzero(); vectorIt++) {
        (*m_mmRows)[vectorIt.getIndex()].set(columnIndex2, 0);
    }
    for (Vector::NonzeroIterator vectorIt = column1.beginNonzero(); vectorIt < column1.endNonzero(); vectorIt++) {
        (*m_mmRows)[vectorIt.getIndex()].set(columnIndex2, *vectorIt);
    }
    for (Vector::NonzeroIterator vectorIt = column2.beginNonzero(); vectorIt < column2.endNonzero(); vectorIt++) {
        (*m_mmRows)[vectorIt.getIndex()].set(columnIndex1, *vectorIt);
    }
    (*m_mmColumns)[columnIndex1] = column2;
    (*m_mmColumns)[columnIndex2] = column1;

    (*m_columnSwapLog)[columnHashIndex1] = columnIndex2;
    (*m_columnSwapLog)[columnHashIndex2] = columnIndex1;

    (*m_columnSwapHash)[columnIndex1] = columnHashIndex2;
    (*m_columnSwapHash)[columnIndex2] = columnHashIndex1;
    //printSwapHashes();
}

int PfiBasis::searchColumn(int columnIndex, int searchIndex, std::vector<int>& searchedRows) {
    DEVINFO(D::PFIMAKER, "Searching column " << columnIndex << " for nonzero with searchindex " << searchIndex);
    Vector::NonzeroIterator it = (*m_mmColumns)[columnIndex].beginNonzero();
    Vector::NonzeroIterator itend = (*m_mmColumns)[columnIndex].endNonzero();
    for (; it < itend; it++) {
        if ((int) it.getIndex() >= searchIndex) {
            DEVINFO(D::PFIMAKER, "Nonzero found below index " << searchIndex);
            int searchResult = it.getIndex();
            swapRows(columnIndex, searchResult);
            return searchResult;
        } else if ((*m_mmRows)[it.getIndex()].at(it.getIndex()) == 0) {
            DEVINFO(D::PFIMAKER, "Nonzero found in singular row below index " << searchIndex);
            int searchResult = it.getIndex();
            swapRows(columnIndex, searchResult);
            return searchResult;
        }
    }
    Vector::NonzeroIterator vectorIt = (*m_mmColumns)[columnIndex].beginNonzero();
    Vector::NonzeroIterator vectorItend = (*m_mmColumns)[columnIndex].endNonzero();
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
                (*m_mmGraphOut)[it - m_mmRows->begin()].push_back(vectorIt.getIndex());
                (*m_mmGraphIn)[vectorIt.getIndex()].push_back(it - m_mmRows->begin());
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
    std::vector<int> outGoing = (*m_mmGraphOut)[currentNode.index];
    while ((int) outGoing.size() > currentNode.nextEdge) {
        if ((*m_mmGraphIn)[outGoing[currentNode.nextEdge]].size() > 0) {
            int next = outGoing[currentNode.nextEdge];
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
                            (*m_stack)[stackPosition].lowest = nextLowest;
                            break;
                        }
                    }
                }
            } else {
                for (std::vector<PathNode>::iterator it = m_stack->begin(); it < m_stack->end(); it++) {
                    if (it->index == next) {
                        DEVINFO(D::PFIMAKER, "Setting current lowest to next " << next);
                        currentNode.lowest = it->lowest;
                        (*m_stack)[stackPosition].lowest = it->lowest;
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
        (*m_stack)[stackPosition].nextEdge++;
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
            for (std::vector<int>::iterator nodeIt = (*m_mmGraphIn)[lastIndex].begin(); nodeIt < (*m_mmGraphIn)[lastIndex].end(); nodeIt++) {
                DEVINFO(D::PFIMAKER, "Searching node " << *nodeIt);
                for (std::vector<int>::iterator searchIt = (*m_mmGraphOut)[*nodeIt].begin(); searchIt < (*m_mmGraphOut)[*nodeIt].end(); searchIt++) {
                    DEVINFO(D::PFIMAKER, "Outgoing edge " << *searchIt);
                    if (*searchIt == lastIndex) {
                        (*m_mmGraphOut)[*nodeIt].erase(searchIt);
                        break;
                    }
                }
            }
            (*m_mmGraphIn)[lastIndex].clear();
            DEVINFO(D::PFIMAKER, "Last index in the stack is " << lastIndex);
            (*m_orderedIndex)[lastIndex] = true;
            swapRows((*m_columnSwapLog)[lastIndex], allBlocks);
            swapColumns((*m_columnSwapLog)[lastIndex], allBlocks);
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
    findTransversal();
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
    switch (m_nontriangularPivotRule) {
    case NONE:
        if (currentColumn->at(rowindex) == 0) {
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
                previousBlocks += (*m_mmBlocks)[i];
            }
            //            LPINFO("previousBlocks: "<<previousBlocks);
            //Activerows-ba kigyujtjuk a blokkhoz tartozo nemnullakat
            Vector::NonzeroIterator it = currentColumn->beginNonzero();
            Vector::NonzeroIterator itend = currentColumn->endNonzero();
            for (; it < itend; it++) {
                if (m_rowCounts[it.getIndex()] > -1) {
                    //Ha MM-beli nemnullat talaltunk
                    for (int i = previousBlocks; i < previousBlocks + (*m_mmBlocks)[blockNum]; i++) {
                        //Ha a nemnulla a blokkon belul van
                        if ((int) it.getIndex() == (*m_mmRowIndices)[(*m_rowSwapHash)[i]]) {
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
                if (m_rowCounts[it.getIndex()] > -1) {
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
                if (currentColumn->at(*it) > m_threshold * nontriangularMax) {
                    goodRows.push_back(*it);
                }
            } else {
                if (-(currentColumn->at(*it)) > m_threshold * nontriangularMax) {
                    goodRows.push_back(*it);
                }
            }
        }
        //Choose the sparsest row among the candidates
        if (goodRows.size() > 0) {
            rowindex = goodRows[0];
            for (std::vector<int>::iterator it = goodRows.begin(); it < goodRows.end(); it++) {
                if (m_rowCounts[*it] < m_rowCounts[rowindex]) {
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

void PfiBasis::checkSingularity() {
    DEVINFO(D::PFIMAKER, "Checking singularity");
    int singularity = 0;
    for (std::vector<int>::iterator it = m_basisNewHead.begin(); it < m_basisNewHead.end(); it++) {
        if (*it == -1) {
            DEVINFO(D::PFIMAKER, "Given basis column " << it - m_basisNewHead.begin() << " is singular, replacing with unit vector");
            *it = it - m_basisNewHead.begin() + m_model.getColumnCount();
            singularity++;
        }
    }
    if (singularity != 0) {
        m_singularityCounter += singularity;
        LPWARNING("The given basis is singular, the measure of singularity is " << singularity);
    }
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
    switch (m_nontriangularMethod) {
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
    switch (m_nontriangularPivotRule) {
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
}

void PfiBasis::printTransformationStatistics() const {
    LPINFO("Inversion count: "<<m_inversionCount);
    //		LPINFO("Transformation count in the last inversion: "<<m_transformationCount);
    LPINFO("Transformation count average: "<<m_transformationAverage);
    LPINFO("Average size of the non-triangular part:"<<m_mNumAverage);
    printStatistics();
}
