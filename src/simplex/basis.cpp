
#include <simplex/basis.h>

#include <simplex/simplexmodel.h>

#include <fstream>

Basis::Basis(const SimplexModel& model, IndexList& variableStates, std::vector<int>& basisHead) :
    m_model(model),
    m_variableStates(variableStates),
    m_basisHead(basisHead)
{
    m_isFresh = false;
    m_columns = new std::vector<Vector>();
    m_columnsHash = new std::vector<int>();
    m_rows = new std::vector<Vector>();
    m_rowCounts = new std::vector<int>();
    m_columnCounts = new std::vector<int>();
    m_rowCountIndexList = NULL;
    m_columnCountIndexList = NULL;
}

Basis::~Basis()
{
    delete m_columns;
    delete m_columnsHash;
    delete m_rows;
    delete m_rowCounts;
    delete m_columnCounts;
    if (m_rowCountIndexList != NULL) {
        delete m_rowCountIndexList;
    }
    if (m_columnCountIndexList != NULL) {
        delete m_columnCountIndexList;
    }
}

void Basis::copyBasis(bool buildIndexLists) {
    DEVINFO(D::PFIMAKER, "Copy the basis");
    int columnCount = m_model.getMatrix().columnCount();
    int rowCount = m_model.getMatrix().rowCount();

#define USE_LOGICALS
#ifndef USE_LOGICALS
    std::vector<int> logicals;
#endif
    std::vector<bool> headChecker(rowCount + columnCount, false);
    m_basisNewHead.resize(rowCount, -1);
    //Copy the active submatrix
    for (std::vector<int>::iterator it = m_basisHead.begin(); it < m_basisHead.end(); it++) {
        if (headChecker.at(*it) == false) {
            headChecker.at(*it) = true;
            if (*it >= columnCount) {
                DEVINFO(D::PFIMAKER, "Logical variable found in basis head: y" << *it - columnCount);
                //Collect the logical columns
#ifdef USE_LOGICALS
                Vector logical(rowCount);
                logical.set(*it - columnCount,1);
                m_columns->push_back(logical);
                m_columnsHash->push_back(*it);
                m_basisNonzeros++;
#else
                logicals.push_back(*it - columnCount);
                m_basisNewHead.at(*it - columnCount) = *it;
                m_basisNonzeros++;
                m_inverseNonzeros++;
#endif
            } else {
                DEVINFO(D::PFIMAKER, "Structural variable found in basis head: x" << *it);
                //The submatrix is the active submatrix needed for inversion
                m_columns->push_back(m_model.getMatrix().column(*it));
                m_columnsHash->push_back(*it);
                m_basisNonzeros += m_columns->back().nonZeros();
            }
        } else {
            LPWARNING("Duplicate index in basis head: " << *it);
        }
    }
    //Set up row counts, column counts (r_i, c_i) and the corresponding row lists
    m_rowCounts->resize(rowCount, 0);
    m_columnCounts->resize(m_columns->size(), 0);
    m_rows->resize(rowCount, Vector(m_columns->size()));
    unsigned int maxRowCount = 0;
    unsigned int maxColumnCount = 0;
    for (std::vector<Vector>::iterator it = m_columns->begin(); it < m_columns->end(); it++) {
        m_columnCounts->at(it - m_columns->begin()) = it->nonZeros();
        if (maxColumnCount < it->nonZeros()) {
            maxColumnCount = it->nonZeros();
        }
        for (Vector::NonzeroIterator vectorIt = it->beginNonzero(); vectorIt < it->endNonzero(); vectorIt++) {
            m_rowCounts->at(vectorIt.getIndex())++;
            m_rows->at(vectorIt.getIndex()).setNewNonzero(it - m_columns->begin(), *vectorIt);
            DEVINFO(D::PFIMAKER, "Nonzero at submatrix column " << it - m_columns->begin() << " at index " << vectorIt.getIndex() << " with value " << *vectorIt);
        }
    }
    //Delete logical row counts
#ifndef USE_LOGICALS
    for (std::vector<int>::iterator logicalIt = logicals.begin(); logicalIt < logicals.end(); logicalIt++) {
        m_rowCounts->at(*logicalIt) = -1;
        for (Vector::NonzeroIterator vectorIt = m_rows->at(*logicalIt).beginNonzero(); vectorIt
                < m_rows->at(*logicalIt).endNonzero(); vectorIt++) {
            m_columnCounts->at(vectorIt.getIndex())--;
        }
    }
#endif
    for (std::vector<int>::iterator it = m_rowCounts->begin(); it < m_rowCounts->end(); it++) {
        if (maxRowCount < (unsigned int) *it) {
            maxRowCount = *it;
        }
    }
#ifndef NDEBUG
    printActiveSubmatrix();
#endif

    if(buildIndexLists){
        buildRowCountIndexLists(maxRowCount);
        buildColumnCountIndexLists(maxColumnCount);
    }
}


void Basis::buildRowCountIndexLists(unsigned int maxRowCount) {
    if(m_rowCountIndexList != NULL){
        delete m_rowCountIndexList;
    }
    m_rowCountIndexList = new IndexList(m_rows->size(), maxRowCount+1);
    if(maxRowCount>0){
        for (std::vector<int>::iterator it = m_rowCounts->begin(); it < m_rowCounts->end(); it++) {
            if (*it >= 0) {
                m_rowCountIndexList->insert(*it, it - m_rowCounts->begin());
            }
        }
    }
    DEVINFO(D::PFIMAKER, "Row links built.");
#ifndef NDEBUG
    printRowLinks();
#endif
}

void Basis::buildColumnCountIndexLists(unsigned int maxColumnCount) {
    if(m_columnCountIndexList != NULL){
        delete m_columnCountIndexList;
    }
    m_columnCountIndexList = new IndexList(m_columns->size(), maxColumnCount+1);
    if(maxColumnCount>0){
        for (std::vector<int>::iterator it = m_columnCounts->begin(); it < m_columnCounts->end(); it++) {
            if (*it >= 0) {
                m_columnCountIndexList->insert(*it, it - m_columnCounts->begin());
            }
        }
    }
    DEVINFO(D::PFIMAKER, "Column links built.");
#ifndef NDEBUG
    printColumnLinks();
#endif
}

void Basis::setNewHead() {
    //This vector indicates the pattern of the basis columns
    std::vector<bool> nonbasic(m_model.getMatrix().columnCount() + m_model.getMatrix().rowCount(), false);
    //First mark the positions of the given basis head
    for (std::vector<int>::iterator it = m_basisHead.begin(); it < m_basisHead.end(); it++) {
        nonbasic.at(*it) = true;
    }
    //Update the basis head with the recomputed one
    m_basisHead.assign(m_basisNewHead.begin(), m_basisNewHead.end());
    //Set the basic variables state and remove their traces from the pattern vector
    for (std::vector<int>::iterator it = m_basisHead.begin(); it < m_basisHead.end(); it++) {
        nonbasic.at(*it) = false;
        //TODO
//        m_model.getVariable(*it).setState(Variable::BASIC);
    }
    //If the pattern vector still contains true values then the basis head is modified, thus some variables
    //are aout of the basis, these must be marked as nonbasic and their states must be updated too.
    for (std::vector<bool>::iterator it = nonbasic.begin(); it < nonbasic.end(); it++) {
        if (*it == true) {
            //TODO
//            m_model.getVariable(*it).setState(Variable::NONBASIC_AT_LB);
//            m_model.getVariable(*it).setValue(m_model.getVariable(*it).getLowerBound());
        }
    }
}

void Basis::checkSingularity() {
    DEVINFO(D::PFIMAKER, "Checking singularity");
    int singularity = 0;
    for (std::vector<int>::iterator it = m_basisNewHead.begin(); it < m_basisNewHead.end(); it++) {
        if (*it == -1) {
            DEVINFO(D::PFIMAKER, "Given basis column " << it - m_basisNewHead.begin() << " is singular, replacing with unit vector");
            *it = it - m_basisNewHead.begin() + m_model.getMatrix().columnCount();
            singularity++;
        }
    }
    if (singularity != 0) {
        LPWARNING("The given basis is singular, the measure of singularity is " << singularity);
    }
}

Vector* Basis::createEta(Vector* v, int pivotPosition) throw (NumericalException)
{
    Vector * eta = new Vector(0, 0, 0);
    eta->prepareForData(v->nonZeros(), v->length());
    Numerical::Double atPivot = v->at(pivotPosition);

    if (Numerical::equals(atPivot, 0)) {
        LPERROR("Pivot element is zero at row " << pivotPosition);
        throw NumericalException("NUMERICAL problem");
    } else {
        for (Vector::NonzeroIterator it = v->beginNonzero(); it < v->endNonzero(); it++) {
            if (it.getIndex() == (unsigned int) pivotPosition) {
                eta->newNonZero(1 / atPivot, pivotPosition);
            } else {
                eta->newNonZero(-(*it) / atPivot, it.getIndex());
            }
        }
        DEVINFO(D::PFIMAKER, "Eta vector created with pivot " << pivotPosition);
        return eta;
    }
}

void Basis::printActiveSubmatrix() const
{
#ifndef NDEBUG
    DEVINFO(D::PFIMAKER, "Active submatrix pattern by rows");
    for (std::vector<Vector>::iterator it = m_rows->begin(); it < m_rows->end(); it++) {
        std::string s;
        for (int i = 0; i < (int) it->length(); i++) {
            s += Numerical::equals(it->at(i), 0) ? "-" : "X";
        }
        DEVINFO(D::PFIMAKER, s);
    }
    DEVINFO(D::PFIMAKER, "Active submatrix pattern by columns");
    for (int i = 0; i < (int) m_rows->size(); i++) {
        std::string s;
        for (std::vector<Vector>::iterator it = m_columns->begin(); it < m_columns->end(); it++) {
            s += Numerical::equals(it->at(i), 0) ? "-" : "X";
        }
        DEVINFO(D::PFIMAKER, s);
    }

#endif //!NDEBUG
}

void Basis::printRowLinks() const
{
#ifndef NDEBUG
    DEVINFO(D::PFIMAKER, "Row links partitions: " << m_rowLinks->getPartitionCount());
    DEVINFO(D::PFIMAKER, "Row links indices: " << m_rowLinks->getIndexCount());
    for(unsigned int i=0;i<m_rowLinks->getPartitionCount();i++){
        DEVINFO(D::PFIMAKER, "Row list " << i << ": ");
        for (IndexLinkedList::Iterator it = m_rowLinks->begin(i); it != m_rowLinks->end(i); it++) {
            DEVINFO(D::PFIMAKER, "     " << it.getData());
        }
    }
#endif //!NDEBUG
}

void Basis::printColumnLinks() const
{
#ifndef NDEBUG
    DEVINFO(D::PFIMAKER, "Column links partitions: " << m_columnLinks->getPartitionCount());
    DEVINFO(D::PFIMAKER, "Column links indices: " << m_columnLinks->getIndexCount());
    for(unsigned int i=0;i<m_columnLinks->getPartitionCount();i++){
        DEVINFO(D::PFIMAKER, "Column list " << i << ": ");
        for (IndexLinkedList::Iterator it = m_columnLinks->begin(i); it != m_columnLinks->end(i); it++) {
            DEVINFO(D::PFIMAKER,  "     " << it.getData());
        }
    }
#endif //!NDEBUG
}
