#include <simplex/checker.h>

#include <vector>
#include <algorithm>
#include <simplex/basis.h>

bool Checker::checkPfiWithFtran(const Simplex &simplex) {
    std::vector<int> unitVectors;
    Vector v;
    v.reInit(simplex.m_simplexModel->getRowCount());
    DEVINFO(D::PFIMAKER, "Check inverse with FTRAN");
    std::vector<int>::const_iterator it;
    for (it = simplex.m_basisHead.begin(); it < simplex.m_basisHead.end(); it++) {
        if (*it < (int) simplex.m_simplexModel->getMatrix().columnCount()) {
            v = simplex.m_simplexModel->getMatrix().column(*it);
        } else {
            v = Vector(simplex.m_simplexModel->getMatrix().rowCount());
            v.setNewNonzero((*it) - simplex.m_simplexModel->getMatrix().columnCount(), 1);
        }
        simplex.m_basis->Ftran(v);
        if (v.nonZeros() == 1 && Numerical::equals(*(v.beginNonzero()), 1.0)) {
            DEVINFO(D::PFIMAKER, "Unit vector reinverted with index: " << v.beginNonzero().getIndex());
            unitVectors.push_back(v.beginNonzero().getIndex());
        }
    }
    std::sort(unitVectors.begin(), unitVectors.end());
    bool success = true;
    for (std::vector<int>::const_iterator cit = unitVectors.begin(); cit < unitVectors.end(); cit++) {
        if (*cit == cit - unitVectors.begin() ||
            *cit == cit - unitVectors.begin()+(int) simplex.m_simplexModel->getMatrix().columnCount()) {
            DEVINFO(D::PFIMAKER, *cit << " FTRAN VECTOR PASSED");
        } else {
            DEVINFO(D::PFIMAKER, *cit << " FTRAN VECTOR FAILED");
            success = false;
        }
    }
    if (success && unitVectors.size() == simplex.m_basisHead.size() && simplex.m_basisHead.size() > 0) {
        LPINFO("FTRAN CHECK PASSED");
        return true;
    } else {
        LPWARNING("FTRAN CHECK FAILED");
        return false;
    }
}

bool Checker::checkPfiWithBtran(const Simplex& simplex) {
    std::vector<int> unitVectors;
    DEVINFO(D::PFIMAKER, "Check inverse with BTRAN");
    DEVINFO(D::PFIMAKER, "BTRAN copy");
    //Copy the basis columns
    std::vector<Vector> basisCopy;
    std::vector<int>::const_iterator it;
    for (it = simplex.m_basisHead.begin(); it < simplex.m_basisHead.end(); it++) {
        if (*it >= (int) simplex.m_simplexModel->getMatrix().columnCount()) {
            Vector logical = Vector(simplex.m_simplexModel->getMatrix().rowCount());
            logical.set((*it) - simplex.m_simplexModel->getMatrix().columnCount(), 1);
            basisCopy.push_back(logical);
        } else {
            basisCopy.push_back(simplex.m_simplexModel->getMatrix().column(*it));
        }
    }
    //Prepare the rows
    DEVINFO(D::PFIMAKER, "BTRAN prepare rows");
    std::vector<Vector> rowbasis;
    for (unsigned int i = 0; i < simplex.m_basisHead.size(); i++) {
        Vector row = Vector(simplex.m_simplexModel->getMatrix().rowCount());
        rowbasis.push_back(row);
    }
    std::vector<Vector>::iterator basisIt;
    for (basisIt = basisCopy.begin(); basisIt < basisCopy.end(); basisIt++) {
        for (Vector::NonzeroIterator vectorIt = basisIt->beginNonzero(); vectorIt < basisIt->endNonzero(); vectorIt++) {
            rowbasis.at(vectorIt.getIndex()).set(basisIt - basisCopy.begin(), *vectorIt);
        }
    }
    //Use BTRAN to check the inverse
    DEVINFO(D::PFIMAKER, "Do the BTRAN check on the rows");
    for (basisIt = rowbasis.begin(); basisIt < rowbasis.end(); basisIt++) {
        simplex.m_basis->Btran(*basisIt);
        // DEVINFO(D::PFIMAKER, "V with Vector print form" <<v);
        if (basisIt->nonZeros() == 1 && Numerical::equals(*(basisIt->beginNonzero()), 1)) {
            DEVINFO(D::PFIMAKER, "Unit vector reinverted with index: " << basisIt - rowbasis.begin());
            unitVectors.push_back(basisIt - rowbasis.begin());
        } else {
            Vector::NonzeroIterator badIt = basisIt->beginNonzero();
            double max = 0;
            int maxIndex = 0;
            for(;badIt != basisIt->endNonzero(); badIt++){
                if(!Numerical::equals(*badIt, 1) && Numerical::fabs(*badIt)> max){
                    max = Numerical::fabs(*badIt);
                    maxIndex = badIt.getIndex();
                }
            }
            LPERROR ("Rossz vektor, a legnagyobb elteres: " << basisIt->at(maxIndex));
        }
    }
    std::sort(unitVectors.begin(), unitVectors.end());
    bool success = true;
    for (std::vector<int>::iterator cit = unitVectors.begin(); cit < unitVectors.end(); cit++) {
        DEVINFO(D::PFIMAKER, *cit);
        if (*cit == cit - unitVectors.begin() ||
            *cit == cit - unitVectors.begin()-(int) simplex.m_simplexModel->getMatrix().columnCount()) {
            DEVINFO(D::PFIMAKER, *cit << " BTRAN VECTOR PASSED");
        } else {
            DEVINFO(D::PFIMAKER, *cit << " BTRAN VECTOR FAILED");
            success = false;
        }
    }
    if (success && unitVectors.size() == simplex.m_basisHead.size() && simplex.m_basisHead.size() > 0) {
        LPINFO("BTRAN CHECK PASSED");
        return true;
    } else {
        LPWARNING("BTRAN CHECK FAILED");
        return false;
    }
}

bool Checker::checkPfiWithReducedCost(const Simplex& simplex) {
    Vector pi(simplex.m_simplexModel->getMatrix().rowCount());
    std::vector<int>::const_iterator it;
    for (it = simplex.m_basisHead.begin(); it < simplex.m_basisHead.end(); it++) {
        if (*it < (int) simplex.m_simplexModel->getMatrix().columnCount()) {
            pi.set(it - simplex.m_basisHead.begin(), simplex.m_simplexModel->getCostVector().at(*it));
        }
    }
    simplex.m_basis->Btran(pi);


    int success = true;
    for (it = simplex.m_basisHead.begin(); it < simplex.m_basisHead.end(); it++) {
        //Structural basis vectors are interesting
        if (*it < (int) simplex.m_simplexModel->getMatrix().columnCount()) {
            Numerical::Double d = simplex.m_simplexModel->getCostVector().at(*it) -
                                      pi.dotProduct(simplex.m_simplexModel->getMatrix().column(*it));
            if (!Numerical::isZero(d)) {
                success = false;
                LPWARNING("Nonzero basic reduced cost at basis column " << *it << " with value: " << d);
            }
        }
    }
    if (success && simplex.m_basisHead.size() > 0) {
        LPINFO("REDUCED COST CHECK PASSED");
        return true;
    } else {
        LPWARNING("REDUCED COST CHECK FAILED");
        return false;
    }
}


bool Checker::checkAlphaValue(const Simplex& simplex,
                              int rowIndex, int columnIndex, Numerical::Double & columnAlpha, Numerical::Double & rowAlpha){
    unsigned int rowCount = simplex.m_simplexModel->getRowCount();
    unsigned int columnCount = simplex.m_simplexModel->getColumnCount();
    Vector row(rowCount + columnCount);

    Vector rho(rowCount); //Row of the inverse of the basis
    rho.setSparsityRatio(DENSE);
    rho.setNewNonzero(rowIndex, 1);

    simplex.m_basis->Btran(rho);
    IndexList<const Numerical::Double *>::Iterator it;
    IndexList<const Numerical::Double *>::Iterator itEnd;
    //TODO: A bazisvaltozo egyeset kulon kellene majd bebillenteni hogy gyorsabb legyen
    simplex.m_variableStates.getIterators(&it, &itEnd, Simplex::BASIC, 4);
    for(; it != itEnd ; it++){
        unsigned int columnIndex = it.getData();
        if(columnIndex < columnCount){
            row.set(columnIndex, rho.dotProduct(simplex.m_simplexModel->getMatrix().column(columnIndex)));
        } else {
            row.set(columnIndex, rho.at(columnIndex - columnCount));
        }
    }

    Vector column(rowCount);
    column.setSparsityRatio(DENSE);
    if(columnIndex < (int)columnCount){
        column = simplex.m_simplexModel->getMatrix().column(columnIndex);
    } else {
        column.setNewNonzero(columnIndex - columnCount, 1);
    }
    simplex.m_basis->Ftran(column);

    columnAlpha = column.at(rowIndex);
    rowAlpha = row.at(columnIndex);
    if(!Numerical::isZero(columnAlpha - rowAlpha)){
        LPWARNING("ALPHA ERROR!");
        return false;
    } else {
        return true;
    }
}

bool Checker::checkPrimalTheta(const Simplex& simplex,
                               int rowIndex, int columnIndex,
                               Numerical::Double& thetaFromCol, Numerical::Double& thetaFromRow){
    Numerical::Double pivotFromCol;
    Numerical::Double pivotFromRow;

    checkAlphaValue(simplex, rowIndex, columnIndex, pivotFromCol, pivotFromRow);

    Variable::VARIABLE_TYPE typeOfIthVariable = simplex.m_simplexModel->getVariable(simplex.m_basisHead.at(rowIndex)).getType();
    Numerical::Double valueOfOutgoingVariable = simplex.m_basicVariableValues.at(rowIndex);

    if (typeOfIthVariable == Variable::FIXED) {
        thetaFromCol = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromCol;
        thetaFromRow = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromRow;
    }
    else if (typeOfIthVariable == Variable::BOUNDED) {
        thetaFromCol = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromCol;
        thetaFromRow = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromRow;
    }
    else if (typeOfIthVariable == Variable::PLUS) {
        thetaFromCol = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromCol;
        thetaFromRow = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromRow;
    }
    else if (typeOfIthVariable == Variable::FREE) {
        thetaFromCol = valueOfOutgoingVariable / pivotFromCol;
        thetaFromRow = valueOfOutgoingVariable  / pivotFromRow;
    }
    else if (typeOfIthVariable == Variable::MINUS) {
        thetaFromCol = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead.at(rowIndex)).getUpperBound()) /
                pivotFromCol;
        thetaFromRow = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromRow;
    }

    if(!Numerical::isZero(thetaFromCol - thetaFromRow)){
        LPWARNING("THETA ERROR!");
        return false;
    } else {
        return true;
    }
}
