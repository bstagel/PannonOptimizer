#include <simplex/checker.h>

#include <vector>
#include <algorithm>
#include <simplex/basis.h>
#include <linalg/linalgparameterhandler.h>
#include <simplex/simplexparameterhandler.h>

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
    simplex.m_variableStates.getIterators(&it, &itEnd, 0, Simplex::VARIABLE_STATE_ENUM_LENGTH);
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

bool Checker::checkFeasibilityConditions(const Simplex& simplex, bool print, Numerical::Double tolerance)
{
    bool okay = true;
    for(unsigned int basisIndex=0; basisIndex<simplex.m_simplexModel->getRowCount(); basisIndex++){
        unsigned int variableIndex = simplex.m_basisHead.at(basisIndex);
        if(Numerical::lessthan(simplex.m_basicVariableValues.at(basisIndex),simplex.m_simplexModel->getVariable(variableIndex).getLowerBound(),tolerance) ||
           Numerical::lessthan(simplex.m_simplexModel->getVariable(variableIndex).getUpperBound(),simplex.m_basicVariableValues.at(basisIndex),tolerance)){
            if(print){
                LPINFO("UNSATISFIED FEASIBILITY (basisIndex:" << basisIndex << " ; variableIndex: " << variableIndex<< "): "
                   << " LB:   " << simplex.m_simplexModel->getVariable(variableIndex).getLowerBound()
                   << " val:  " << simplex.m_basicVariableValues.at(basisIndex)
                   << " UB:   " << simplex.m_simplexModel->getVariable(variableIndex).getUpperBound()
                   << " (tol: " << tolerance << " )");
            }
            okay = false;
        }
    }
    if(okay){
        LPINFO("checkFeasibilityConditions PASSED");
    } else {
        LPERROR("checkFeasibilityConditions FAILED");
    }
    return okay;
}

bool Checker::checkOptimalityConditions(const Simplex& simplex, bool print, Numerical::Double tolerance)
{
    bool okay = true;
    for(unsigned int variableIndex=0; variableIndex<simplex.m_simplexModel->getColumnCount() + simplex.m_simplexModel->getRowCount(); variableIndex++){
        if(simplex.m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_LB &&
           Numerical::lessthan(simplex.m_reducedCosts.at(variableIndex),0,tolerance)){
            if(print){
                LPINFO("UNSATISFIED OPTIMALITY: "
                       << " d_j:  " << simplex.m_reducedCosts.at(variableIndex)
                       << " variable index: " << variableIndex
                       << " type: " << "NONBASIC_AT_LB"
                       << " (tol: " << tolerance << " )");
            }
            okay = false;
        } else if(simplex.m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_UB &&
                  Numerical::lessthan(0,simplex.m_reducedCosts.at(variableIndex),tolerance)){
            if(print){
                LPINFO("UNSATISFIED OPTIMALITY: "
                       << " d_j:  " << simplex.m_reducedCosts.at(variableIndex)
                       << " type: " << "NONBASIC_AT_UB"
                       << " (tol: " << tolerance << " )");
            }
            okay = false;
        } else if(simplex.m_variableStates.where(variableIndex) == Simplex::NONBASIC_FREE &&
                  (Numerical::lessthan(0,simplex.m_reducedCosts.at(variableIndex),tolerance) ||
                   Numerical::lessthan(simplex.m_reducedCosts.at(variableIndex),0,tolerance))){
            if(print){
                LPINFO("UNSATISFIED OPTIMALITY: "
                       << " d_j:  " << simplex.m_reducedCosts.at(variableIndex)
                       << " type: " << "NONBASIC_FREE"
                       << " (tol: " << tolerance << " )");
            }
            okay = false;
        }
    }
    if(okay){
        LPINFO("checkOptimalityConditions PASSED");
    } else {
        LPERROR("checkOptimalityConditions FAILED");
    }
    return okay;
}

bool Checker::checkAllConstraints(const Simplex& simplex, bool print, Numerical::Double tolerance)
{
    bool okay = true;
    for(unsigned int rowIndex=0; rowIndex<simplex.m_simplexModel->getRowCount(); rowIndex++){
        const Vector & row = simplex.m_simplexModel->getMatrix().row(rowIndex);
        Vector::NonzeroIterator rowIter = row.beginNonzero();
        Vector::NonzeroIterator rowIterEnd = row.endNonzero();
        Numerical::Double minus = 0, plus = 0;
        for(; rowIter != rowIterEnd; rowIter++){
            Numerical::Double product = (*rowIter) * (*simplex.m_variableStates.getAttachedData(rowIter.getIndex()));
            if(product > 0){
                plus += product;
            } else {
                minus += product;
            }
        }

        Numerical::Double logicalValue = *simplex.m_variableStates.getAttachedData(simplex.m_simplexModel->getColumnCount() + rowIndex);
        if(logicalValue > 0){
            plus += logicalValue;
        } else {
            minus += logicalValue;
        }

        Numerical::Double sum = plus + minus;
        if(!Numerical::equal(sum, simplex.m_simplexModel->getRhs().at(rowIndex), tolerance)){
            if(print){
                LPINFO("UNSATISFIED CONSTRAINT: "
                       << " sum:  " << sum
                       << " rhs: "  << simplex.m_simplexModel->getRhs().at(rowIndex)
                       << " (tol: " << tolerance << " )");
            }
            okay = false;
        }
    }
    if(okay){
        LPINFO("checkAllConstraints PASSED");
    } else {
        LPERROR("checkAllConstraints FAILED");
    }
    return okay;
}

bool Checker::checkNonbasicVariableStates(const Simplex &simplex, bool print)
{
    bool okay = true;
    for(unsigned int variableIndex=0; variableIndex<simplex.m_simplexModel->getColumnCount() + simplex.m_simplexModel->getRowCount(); variableIndex++){
        unsigned int state = simplex.m_variableStates.where(variableIndex);
        const Variable & variable = simplex.m_simplexModel->getVariable(variableIndex);
        if(state == Simplex::NONBASIC_AT_LB && *simplex.m_variableStates.getAttachedData(variableIndex)!=variable.getLowerBound()){
            if(print){
                LPINFO("UNSATISFIED VARIABLE STATE: "
                       << " state:  " << "NONBASIC_AT_LB"
                       << " LB: "  << variable.getLowerBound()
                       << " val: " << *simplex.m_variableStates.getAttachedData(variableIndex));
            }
            okay = false;
        } else if(state == Simplex::NONBASIC_AT_UB && *simplex.m_variableStates.getAttachedData(variableIndex) != variable.getUpperBound()){
            if(print){
                LPINFO("UNSATISFIED VARIABLE STATE: "
                       << " state:  " << "NONBASIC_AT_UB"
                       << " LB: "  << variable.getUpperBound()
                       << " val: " << *simplex.m_variableStates.getAttachedData(variableIndex));
            }
            okay = false;
        } else if(state == Simplex::NONBASIC_FIXED && *simplex.m_variableStates.getAttachedData(variableIndex) != variable.getLowerBound()
                                                   && *simplex.m_variableStates.getAttachedData(variableIndex) != variable.getUpperBound()){
            if(print){
                LPINFO("UNSATISFIED VARIABLE STATE: "
                       << " state:  " << "NONBASIC_FIXED"
                       << " LB: "  << variable.getLowerBound()
                       << " UB: "  << variable.getUpperBound()
                       << " val: " << *simplex.m_variableStates.getAttachedData(variableIndex));
            }
            okay = false;
        } else if(state == Simplex::NONBASIC_FREE && *simplex.m_variableStates.getAttachedData(variableIndex) != 0){
            if(print){
                LPINFO("UNSATISFIED VARIABLE STATE: "
                       << " state:  " << "NONBASIC_FREE"
                       << " val: " << *simplex.m_variableStates.getAttachedData(variableIndex));
            }
            okay = false;
        } else if(state > Simplex::NONBASIC_FREE){
            if(print){
                LPINFO("UNSATISFIED VARIABLE STATE: "
                       << " state:  " << "INVALID STATE");
            }
            okay = false;
        }
    }
    if(okay){
        LPINFO("checkNonbasicVariableStates PASSED");
    } else {
        LPERROR("checkNonbasicVariableStates FAILED");
    }
    return okay;
}

bool Checker::checkBasicVariableStates(const Simplex &simplex, bool print)
{
    bool okay = true;
    for(unsigned int basisIndex=0; basisIndex<simplex.m_basisHead.size(); basisIndex++){
        if(simplex.m_variableStates.where(simplex.m_basisHead.at(basisIndex))!=Simplex::BASIC){
            if(print){
                LPINFO("UNSATISFIED BASIC STATE: "
                       << " state:  " << simplex.m_variableStates.getAttachedData(simplex.m_basisHead.at(basisIndex))
                       << " val: " << *simplex.m_variableStates.getAttachedData(simplex.m_basisHead.at(basisIndex)));
            }
            okay = false;
        }
    }
    if(okay){
        LPINFO("checkBasicVariableStates PASSED");
    } else {
        LPERROR("checkBasicVariableStates FAILED");
    }
    return okay;
}

bool Checker::checkBasicVariableFeasibilityStates(const Simplex &simplex, bool print, Numerical::Double tolerance)
{
    bool okay = true;
    for(unsigned int basisIndex=0; basisIndex<simplex.m_basisHead.size(); basisIndex++){
        const Variable & variable = simplex.m_simplexModel->getVariable(simplex.m_basisHead.at(basisIndex));
        unsigned int state = simplex.m_basicVariableFeasibilities.where(basisIndex);
        Numerical::Double lb = variable.getLowerBound();
        Numerical::Double ub = variable.getUpperBound();
        Numerical::Double value = simplex.m_basicVariableValues.at(basisIndex);
        if((Numerical::lessthan(value, lb, tolerance) &&  state != Simplex::MINUS)){
            if(print){
                LPINFO("UNSATISFIED BASIC VARIABLE FEASIBILITY: "
                       << " state : " << state << " - NOT MINUS"
                       << " lb    : " << lb
                       << " value : " << value
                       << " ub    : " << ub
                       << " (tol: " << tolerance << " )");
            }
            okay = false;
        } else if((Numerical::lessthan(ub, value, tolerance) &&  state != Simplex::PLUS)){
            if(print){
                LPINFO("UNSATISFIED BASIC VARIABLE FEASIBILITY: "
                       << " state : " << state << " - NOT PLUS"
                       << " lb    : " << lb
                       << " value : " << value
                       << " ub    : " << ub
                       << " (tol: " << tolerance << " )");
            }
            okay = false;
        } else if(Numerical::lessthan(lb, value, tolerance) && Numerical::lessthan(value, ub, tolerance) &&  state != Simplex::FEASIBLE){
            if(print){
                LPINFO("UNSATISFIED BASIC VARIABLE FEASIBILITY: "
                       << " state : " << state << " - NOT FEASIBLE"
                       << " lb    : " << lb
                       << " value : " << value
                       << " ub    : " << ub
                       << " (tol: " << tolerance << " )");
            }
            okay = false;
        }
    }
    if(okay){
        LPINFO("checkBasicVariableFeasibilities PASSED");
    } else {
        LPERROR("checkBasicVariableFeasibilities FAILED");
    }
    return okay;
}
