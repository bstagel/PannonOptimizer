/**
 * @file basis.cpp
 */

#include <simplex/basis.h>

#include <simplex/simplex.h>
#include <simplex/simplexmodel.h>

#include <fstream>

Basis::Basis(const SimplexModel& model,
             std::vector<int>* basisHead,
             IndexList<const Numerical::Double*>* variableStates,
             const Vector &basicVariableValues) :
    m_model(model),
    m_basisHead(basisHead),
    m_variableStates(variableStates),
    m_basicVariableValues(basicVariableValues)
{
    m_isFresh = false;
}

Basis::~Basis()
{

}

void Basis::copyBasis(bool buildIndexLists) {
    DEVINFO(D::PFIMAKER, "Copy the basis");
    unsigned int columnCount = m_model.getColumnCount();
    unsigned int rowCount = m_model.getRowCount();

    //Reinit data structures
    m_columns.clear();
    m_columnCountIndexList.init(0,0);
    m_columnsHash.clear();
    m_rows.clear();
    m_rowCountIndexList.init(0,0);

    if(m_columns.capacity() != rowCount){
        m_columns.reserve(rowCount);
        m_columnCounts.resize(rowCount,0);
        m_columnsHash.reserve(rowCount);

        m_rows.resize(rowCount, Vector(rowCount));
        m_rowCounts.resize(rowCount, 0);
    }

    std::vector<bool> headChecker(rowCount + columnCount, false);
    m_basisNewHead.resize(rowCount, -1);
    //Copy the active submatrix
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); it++) {
        if (headChecker.at(*it) == false) {
            headChecker.at(*it) = true;
            if (*it >= (int) columnCount) {
                DEVINFO(D::PFIMAKER, "Logical variable found in basis head: y" << *it - columnCount);
                //Collect the logical columns
                Vector logical(rowCount);
                logical.setNewNonzero(*it - columnCount,1);
                m_columns.push_back(logical);
                m_columnsHash.push_back(*it);
                m_basisNonzeros++;
            } else {
                DEVINFO(D::PFIMAKER, "Structural variable found in basis head: x" << *it);
                //The submatrix is the active submatrix needed for inversion
                m_columns.push_back(m_model.getMatrix().column(*it));
                m_columnsHash.push_back(*it);
                m_basisNonzeros += m_columns.back().nonZeros();
            }
        } else {
            //TODO: throw exception here
            LPWARNING("Duplicate index in basis head: " << *it);
        }
    }
    //Set up row counts, column counts (r_i, c_i) and the corresponding row lists
    unsigned int maxRowCount = 0;
    unsigned int maxColumnCount = 0;
    for (std::vector<Vector>::iterator it = m_columns.begin(); it < m_columns.end(); it++) {
        m_columnCounts.at(it - m_columns.begin()) = it->nonZeros();
        if (maxColumnCount < it->nonZeros()) {
            maxColumnCount = it->nonZeros();
        }
        for (Vector::NonzeroIterator vectorIt = it->beginNonzero(); vectorIt < it->endNonzero(); vectorIt++) {
            m_rowCounts.at(vectorIt.getIndex())++;
            m_rows.at(vectorIt.getIndex()).setNewNonzero(it - m_columns.begin(), *vectorIt);
        }
    }

    if(buildIndexLists){
        for (std::vector<int>::iterator it = m_rowCounts.begin(); it < m_rowCounts.end(); it++) {
            if (maxRowCount < (unsigned int) *it) {
                maxRowCount = *it;
            }
        }
        buildRowCountIndexLists(maxRowCount);
        buildColumnCountIndexLists(maxColumnCount);
    }

#ifndef NDEBUG
    printActiveSubmatrix();
#endif
}


void Basis::buildRowCountIndexLists(unsigned int maxRowCount) {
    m_rowCountIndexList.init(m_rows.size(), maxRowCount+1);
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

void Basis::buildColumnCountIndexLists(unsigned int maxColumnCount) {
    m_columnCountIndexList.init(m_columns.size(), maxColumnCount+1);
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

void Basis::setNewHead() {
    //This vector indicates the pattern of the basis columns
    std::vector<bool> nonbasic(m_model.getColumnCount() + m_model.getRowCount(), false);
    //First mark the positions of the given basis head
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); it++) {
        nonbasic.at(*it) = true;
    }
    //Update the basis head with the recomputed one
    m_basisHead->assign(m_basisNewHead.begin(), m_basisNewHead.end());
    //Set the basic variables state and remove their traces from the pattern vector
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); it++) {
        nonbasic.at(*it) = false;
        if (m_variableStates->where(*it) >= Simplex::VARIABLE_STATE_ENUM_LENGTH){
            LPERROR("A varible is not in variablestates!")
            //TODO exception
        } else if(m_variableStates->where(*it) != Simplex::BASIC) {
            m_variableStates->move(*it, Simplex::BASIC, &(m_basicVariableValues.at(it - m_basisHead->begin())));
        }
    }
    //If the pattern vector still contains true values then the basis head is modified, thus some variables
    //are aout of the basis, these must be marked as nonbasic and their states must be updated too.
    for (std::vector<bool>::iterator it = nonbasic.begin(); it < nonbasic.end(); it++) {
        if (*it == true) {
            const Variable& variable = m_model.getVariable(*it);
            if (variable.getType() == Variable::FREE) {
                m_variableStates->insert(Simplex::NONBASIC_FREE, *it, &ZERO);
            } else if (variable.getType() == Variable::MINUS) {
                m_variableStates->insert(Simplex::NONBASIC_AT_UB, *it, &(variable.getUpperBound()));
                return;
            } else if (variable.getType() == Variable::PLUS) {
                m_variableStates->insert(Simplex::NONBASIC_AT_LB, *it, &(variable.getLowerBound()));
                return;
            } else {
                m_variableStates->insert(Simplex::NONBASIC_AT_LB, *it, &(variable.getLowerBound()));
                return;
            }
        }
    }
}

void Basis::checkSingularity() {
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
        LPWARNING("The given basis is singular, the measure of singularity is " << singularity);
    }
}

Vector* Basis::createEta(const Vector& vector, int pivotPosition) throw (NumericalException)
{
    Vector* eta = new Vector();
    eta->prepareForData(vector.nonZeros(), vector.length());
    //TODO Ezt vajon lehet gyorsabban?
    Numerical::Double atPivot = vector.at(pivotPosition);

    if (Numerical::equals(atPivot, 0)) {
        throw NumericalException("NUMERICAL problem: Pivot element is zero at row " + pivotPosition );
    } else {
        Vector::NonzeroIterator it = vector.beginNonzero();
        Vector::NonzeroIterator endit = vector.endNonzero();
        for (; it < endit; it++) {
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
    for (std::vector<Vector>::const_iterator it = m_rows.begin(); it < m_rows.end(); it++) {
        std::string s;
        for (int i = 0; i < (int) it->length(); i++) {
            s += Numerical::equals(it->at(i), 0) ? "-" : "X";
        }
        DEVINFO(D::PFIMAKER, s);
    }
    DEVINFO(D::PFIMAKER, "Active submatrix pattern by columns");
    for (int i = 0; i < (int) m_rows.size(); i++) {
        std::string s;
        for (std::vector<Vector>::const_iterator it = m_columns.begin(); it < m_columns.end(); it++) {
            s += Numerical::equals(it->at(i), 0) ? "-" : "X";
        }
        DEVINFO(D::PFIMAKER, s);
    }

#endif //!NDEBUG
}

