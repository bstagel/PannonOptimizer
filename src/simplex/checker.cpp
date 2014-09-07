/**
 * @file checker.cpp
 */
#include <simplex/checker.h>
#include <vector>
#include <algorithm>
#include <simplex/basis.h>
#include <linalg/linalgparameterhandler.h>
#include <simplex/simplexparameterhandler.h>

bool Checker::checkBasisWithFtran(const Simplex &simplex) {
    unsigned int basisSize = simplex.m_basisHead.size();
    Vector v(basisSize);

    bool success = true;
    std::vector<int>::const_iterator it;
    for (it = simplex.m_basisHead.begin(); it < simplex.m_basisHead.end(); it++) {
        Vector resultVector(basisSize);
        resultVector.setNewNonzero(it - simplex.m_basisHead.begin(), 1.0);
        if (*it < (int) simplex.m_simplexModel->getMatrix().columnCount()) {
            v = simplex.m_simplexModel->getMatrix().column(*it);
        } else {
            v = Vector(simplex.m_simplexModel->getMatrix().rowCount());
            v.setNewNonzero((*it) - simplex.m_simplexModel->getMatrix().columnCount(), 1);
        }
        simplex.m_basis->Ftran(v);
        if (!(v.nonZeros() == 1 &&
              v.beginNonzero().getIndex() == resultVector.beginNonzero().getIndex() &&
              Numerical::equals(*v.beginNonzero(), *resultVector.beginNonzero()))) {
            LPWARNING("FTRAN check error at basis column "<< it-simplex.m_basisHead.begin());
            LPWARNING("Result vector: "<< v  );
            LPWARNING("Instead of: "<< resultVector );
            success = false;
        }
    }

    if (success) {
        LPINFO("FTRAN CHECK PASSED");
        return true;
    } else {
        LPWARNING("FTRAN CHECK FAILED");
        return false;
    }
}

bool Checker::checkBasisWithBtran(const Simplex& simplex) {
//    std::vector<int> unitVectors;
    unsigned int basisSize = simplex.m_basisHead.size();
    bool success = true;
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
    std::vector<Vector> rowbasis;
    for (unsigned int i = 0; i < basisSize; i++) {
        Vector row = Vector(basisSize);
        rowbasis.push_back(row);
    }
    std::vector<Vector>::iterator basisIt;
    for (basisIt = basisCopy.begin(); basisIt < basisCopy.end(); basisIt++) {
        for (Vector::NonzeroIterator vectorIt = basisIt->beginNonzero(); vectorIt < basisIt->endNonzero(); vectorIt++) {
            rowbasis[vectorIt.getIndex()].setNewNonzero(basisIt - basisCopy.begin(), *vectorIt);
        }
    }
    //Use BTRAN to check the inverse
    for (basisIt = rowbasis.begin(); basisIt < rowbasis.end(); basisIt++) {
        Vector resultVector(basisSize);
        resultVector.setNewNonzero(basisIt - rowbasis.begin(), 1.0);
        simplex.m_basis->Btran(*basisIt);

        if (!((basisIt->nonZeros() == 1 &&
              basisIt->beginNonzero().getIndex() == resultVector.beginNonzero().getIndex() &&
              Numerical::equals(*basisIt->beginNonzero(), *resultVector.beginNonzero())))) {
            LPWARNING("BTRAN check error at basis column "<< it-simplex.m_basisHead.begin());
            LPWARNING("Result vector: "<< *basisIt );
            LPWARNING("Instead of: "<< resultVector );
            success = false;
            Vector::NonzeroIterator badIt = basisIt->beginNonzero();
            double max = 0;
            int maxIndex = 0;
            for(;badIt != basisIt->endNonzero(); badIt++){
                if(!Numerical::equals(*badIt, 1) && Numerical::fabs(*badIt)> max){
                    max = Numerical::fabs(*badIt);
                    maxIndex = badIt.getIndex();
                }
            }
            LPERROR ("Bad vector, the max difference: " << basisIt->at(maxIndex));
        }
    }

    if (success) {
        LPINFO("BTRAN CHECK PASSED");
        return true;
    } else {
        LPWARNING("BTRAN CHECK FAILED");
        return false;
    }
}

bool Checker::checkBasisWithReducedCost(const Simplex& simplex) {
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

bool Checker::checkBasisWithNonbasicReducedCost(const Simplex& simplex) {
    unsigned int rowCount = simplex.m_simplexModel->getRowCount();
    unsigned int columnCount = simplex.m_simplexModel->getColumnCount();

    //Get the c_B vector
    Vector cB(rowCount);
    cB.setSparsityRatio(DENSE);
    const Vector& costVector = simplex.m_simplexModel->getCostVector();
    for(unsigned int i = 0; i<simplex.m_basisHead.size(); i++){
        if(simplex.m_basisHead[i] < (int) columnCount && costVector.at(simplex.m_basisHead[i]) != 0.0){
            cB.setNewNonzero(i, costVector.at(simplex.m_basisHead[i]));
        }
    }
    Vector simplexMultiplier(cB);
    //Compute simplex multiplier
    simplex.m_basis->Btran(simplexMultiplier);

    //For each variable
    bool success = true;
    for(unsigned int i = 0; i < rowCount + columnCount; i++) {
        if(simplex.m_variableStates.where(i) == Simplex::BASIC){
            continue;
        }
        //Compute the dot product and the reduced cost
        Numerical::Double btranreducedCost;
        Numerical::Double ftranreducedCost;
        if(i < columnCount){
            //c_j-(c_B^T*B^-1)*a_j
            btranreducedCost = Numerical::stableAdd(costVector.at(i),
                               - simplexMultiplier.dotProduct(simplex.m_simplexModel->getMatrix().column(i),true,true));
            Vector column(simplex.m_simplexModel->getMatrix().column(i));
            simplex.m_basis->Ftran(column);
            //c_j-c_B^T*(B^-1*a_j)
            ftranreducedCost = Numerical::stableAdd(costVector.at(i), - cB.dotProduct(column));
        } else {
            btranreducedCost = -1 * simplexMultiplier.at(i - columnCount);
            Vector column(rowCount);
            column.setNewNonzero(i-columnCount, 1);
            simplex.m_basis->Ftran(column);
            ftranreducedCost = -1 * cB.dotProduct(column);
        }
        if(!Numerical::equals(btranreducedCost, ftranreducedCost)){
            LPERROR("REDUCED COST ERROR AT COLUMN "<<i);
            LPERROR("btranreducedCost "<<btranreducedCost);
            LPERROR("ftranreducedCost "<<ftranreducedCost);
            success = false;
        }
    }

    if (success && simplex.m_basisHead.size() > 0) {
        LPINFO("NONZERO REDUCED COST CHECK PASSED");
        return true;
    } else {
        LPWARNING("NONZERO REDUCED COST CHECK FAILED");
        return false;
    }
}

bool Checker::checkVariableStateAttachedValues(const Simplex& simplex){
    int rowCount = simplex.m_simplexModel->getRowCount();
    int columnCount = simplex.m_simplexModel->getColumnCount();
    bool success = true;
    for(int i=0; i<rowCount + columnCount; i++){
        if(simplex.m_variableStates.where(i) == Simplex::BASIC){
            int basisindex = -1;
            for(int j=0; j<(int)simplex.m_basisHead.size(); j++){
                if(simplex.m_basisHead[j] == i){
                    basisindex = j;
                    break;
                }
            }
            if(*simplex.m_variableStates.getAttachedData(i) != simplex.m_basicVariableValues.at(basisindex)){
                LPERROR("Wrong attached value of variable state BASIC");
                LPERROR("Attached value "<< *simplex.m_variableStates.getAttachedData(i));
                LPERROR("It should be " << simplex.m_basicVariableValues.at(basisindex));
                success = false;
            }
        } else if(simplex.m_variableStates.where(i) == Simplex::NONBASIC_AT_LB){
            if(*simplex.m_variableStates.getAttachedData(i) != simplex.m_simplexModel->getVariable(i).getLowerBound()){
                LPERROR("Wrong attached value of variable state NONBASIC_AT_LB");
                LPERROR("Attached value "<< *simplex.m_variableStates.getAttachedData(i));
                LPERROR("It should be " << simplex.m_simplexModel->getVariable(i).getLowerBound());
                success = false;
            }
        } else if(simplex.m_variableStates.where(i) == Simplex::NONBASIC_AT_UB){
            if(*simplex.m_variableStates.getAttachedData(i) != simplex.m_simplexModel->getVariable(i).getUpperBound()){
                LPERROR("Wrong attached value of variable state NONBASIC_AT_UB");
                LPERROR("Attached value " << *simplex.m_variableStates.getAttachedData(i));
                LPERROR("It should be " << simplex.m_simplexModel->getVariable(i).getUpperBound());
                success = false;
            }
        } else if(simplex.m_variableStates.where(i) == Simplex::NONBASIC_FIXED){
            if(*simplex.m_variableStates.getAttachedData(i) != simplex.m_simplexModel->getVariable(i).getLowerBound() &&
                    *simplex.m_variableStates.getAttachedData(i) != simplex.m_simplexModel->getVariable(i).getUpperBound()){
                LPERROR("Wrong attached value of variable state NONBASIC_FIXED");
                LPERROR("Attached value "<< *simplex.m_variableStates.getAttachedData(i));
                LPERROR("It should be "<< simplex.m_simplexModel->getVariable(i).getLowerBound()
                        << " and "<< simplex.m_simplexModel->getVariable(i).getUpperBound());
                success = false;
            }

        } else if(simplex.m_variableStates.where(i) == Simplex::NONBASIC_FREE){
            if(*simplex.m_variableStates.getAttachedData(i) != ZERO){
                LPERROR("Wrong attached value of variable state NONBASIC_FREE");
                LPERROR("Attached value "<< *simplex.m_variableStates.getAttachedData(i));
                LPERROR("It should be "<< ZERO);
                success = false;
            }
        }
    }
    if (success && simplex.m_basisHead.size() > 0) {
        LPINFO("VARIABLE STATE ATTACHED VALUE CHECK PASSED");
        return true;
    } else {
        LPWARNING("VARIABLE STATE ATTACHED VALUE CHECK FAILED");
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

    Variable::VARIABLE_TYPE typeOfIthVariable = simplex.m_simplexModel->getVariable(simplex.m_basisHead[rowIndex]).getType();
    Numerical::Double valueOfOutgoingVariable = simplex.m_basicVariableValues.at(rowIndex);

    if (typeOfIthVariable == Variable::FIXED) {
        thetaFromCol = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead[rowIndex]).getLowerBound()) /
                pivotFromCol;
        thetaFromRow = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead[rowIndex]).getLowerBound()) /
                pivotFromRow;
    }
    else if (typeOfIthVariable == Variable::BOUNDED) {
        thetaFromCol = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead[rowIndex]).getLowerBound()) /
                pivotFromCol;
        thetaFromRow = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead[rowIndex]).getLowerBound()) /
                pivotFromRow;
    }
    else if (typeOfIthVariable == Variable::PLUS) {
        thetaFromCol = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead[rowIndex]).getLowerBound()) /
                pivotFromCol;
        thetaFromRow = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead[rowIndex]).getLowerBound()) /
                pivotFromRow;
    }
    else if (typeOfIthVariable == Variable::FREE) {
        thetaFromCol = valueOfOutgoingVariable / pivotFromCol;
        thetaFromRow = valueOfOutgoingVariable  / pivotFromRow;
    }
    else if (typeOfIthVariable == Variable::MINUS) {
        thetaFromCol = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead[rowIndex]).getUpperBound()) /
                pivotFromCol;
        thetaFromRow = (valueOfOutgoingVariable - simplex.m_simplexModel->getVariable(simplex.m_basisHead[rowIndex]).getLowerBound()) /
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
        unsigned int variableIndex = simplex.m_basisHead[basisIndex];
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

bool Checker::checkFeasibilitySets(const Simplex &simplex, bool print, Numerical::Double tolerance)
{
    bool okay = true;
    Numerical::Double reducedCost = 0;
    unsigned int variableState;

    for(unsigned i = 0; i < simplex.m_reducedCosts.length(); i++){
        reducedCost = simplex.m_reducedCosts.at(i);
        variableState = simplex.m_variableStates.where(i);
        //Bounded variables are left out from the feasibility sets because of the feasibilityCorrection
        if(simplex.m_simplexModel->getVariable(i).getType() != Variable::BOUNDED &&
                simplex.m_simplexModel->getVariable(i).getType() != Variable::FIXED){
            if( (variableState == Simplex::NONBASIC_FREE || variableState == Simplex::NONBASIC_AT_LB) &&
                    reducedCost < -tolerance && simplex.m_reducedCostFeasibilities.where(i) != Simplex::MINUS){
                if(print){
                    LPINFO("FEASIBILITY SET M VIOLATED: "
                           << " variableIndex: " << i
                           << " d_j: " << std::scientific<<reducedCost
                           << " reducedCostFeasibility: " << simplex.m_reducedCostFeasibilities.where(i)
                           << " variableState: " << variableState);
                }
                okay = false;
            }else if( (variableState == Simplex::NONBASIC_FREE || variableState == Simplex::NONBASIC_AT_UB) &&
                     reducedCost > tolerance && simplex.m_reducedCostFeasibilities.where(i) != Simplex::PLUS){
                if(print){
                    LPINFO("FEASIBILITY SET P VIOLATED: "
                           << " variableIndex: " << i
                           << " d_j: " << std::scientific<<reducedCost
                           << " reducedCostFeasibility: " << simplex.m_reducedCostFeasibilities.where(i)
                           << " variableState: " << variableState);
                }
                okay = false;
            }else if ( ((variableState == Simplex::NONBASIC_FREE && Numerical::fabs(reducedCost) < tolerance) ||
                        (variableState == Simplex::NONBASIC_AT_LB && reducedCost > -tolerance) ||
                        (variableState == Simplex::NONBASIC_AT_UB && reducedCost < tolerance) ) &&
                       simplex.m_reducedCostFeasibilities.where(i) != Simplex::FEASIBLE){
                if(print){
                    LPINFO("FEASIBILITY SET F VIOLATED: "
                           << " variableIndex: " << i
                           << " d_j: " << std::scientific<<reducedCost
                           << " reducedCostFeasibility: " << simplex.m_reducedCostFeasibilities.where(i)
                           << " variableState: " << variableState);
                }
                okay = false;
            }
        }
    }

    if(okay){
//        LPINFO("checkFeasibilitySets PASSED");
    }else{
        LPERROR("checkFeasibilitySets FAILED");
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
        if(simplex.m_variableStates.where(simplex.m_basisHead[basisIndex])!=Simplex::BASIC){
            if(print){
                LPINFO("UNSATISFIED BASIC STATE: "
                       << " state:  " << simplex.m_variableStates.getAttachedData(simplex.m_basisHead[basisIndex])
                       << " val: " << *simplex.m_variableStates.getAttachedData(simplex.m_basisHead[basisIndex]));
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
        const Variable & variable = simplex.m_simplexModel->getVariable(simplex.m_basisHead[basisIndex]);
        unsigned int state = simplex.m_basicVariableFeasibilities.where(basisIndex);
        Numerical::Double lb = variable.getLowerBound();
        Numerical::Double ub = variable.getUpperBound();
        Numerical::Double value = simplex.m_basicVariableValues.at(basisIndex);
        if((Numerical::lessthan(value, lb, tolerance) &&  state != Simplex::MINUS)){
            if(print){
                LPINFO("UNSATISFIED BASIC VARIABLE FEASIBILITY (var: " << simplex.m_basisHead[basisIndex] << ") << : "
                       << " state : " << state << " - NOT MINUS"
                       << " lb    : " << lb
                       << " value : " << value
                       << " ub    : " << ub
                       << " (tol: " << tolerance << " )");
            }
            okay = false;
        } else if((Numerical::lessthan(ub, value, tolerance) &&  state != Simplex::PLUS)){
            if(print){
                LPINFO("UNSATISFIED BASIC VARIABLE FEASIBILITY (var: " << simplex.m_basisHead[basisIndex] << ") << : "
                       << " state : " << state << " - NOT PLUS"
                       << " lb    : " << lb
                       << " value : " << value
                       << " ub    : " << ub
                       << " (tol: " << tolerance << " )");
            }
            okay = false;
        } else if(Numerical::lessthan(lb, value, tolerance) && Numerical::lessthan(value, ub, tolerance) &&  state != Simplex::FEASIBLE){
            if(print){
                LPINFO("UNSATISFIED BASIC VARIABLE FEASIBILITY (var: " << simplex.m_basisHead[basisIndex] << ") << : "
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
