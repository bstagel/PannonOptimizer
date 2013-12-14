/**
 * @file simplex.cpp
 */

#include <simplex/simplex.h>
#include <utils/parameterhandler.h>
#include <utils/exceptions.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/startingbasisfinder.h>
#include <simplex/pfibasis.h>
#include <globals.h>
#include <simplex/basisheadbas.h>

#include <utils/thirdparty/prettyprint.h>
//#include <qd/qd_real.h>
//#include <qd/fpu.h>

#include <algorithm>

const static char * ITERATION_INDEX_NAME = "iteration";
const static char * ITERATION_TIME_NAME = "time";
const static char * ITERATION_INVERSION_NAME = "inv";

const static char * SOLUTION_ITERATION_NAME = "Total iterations";
const static char * SOLUTION_SOLVE_TIMER_NAME = "Total solution time";
const static char * SOLUTION_INVERSION_TIMER_NAME = "Inversion time";
const static char * SOLUTION_COMPUTE_BASIC_SOLUTION_TIMER_NAME = "Compute basic solution time";
const static char * SOLUTION_COMPUTE_REDUCED_COSTS_TIMER_NAME = "Compute reduced costs time";
const static char * SOLUTION_COMPUTE_FEASIBILITY_TIMER_NAME = "Compute feasibility time";
const static char * SOLUTION_CHECK_FEASIBILITY_TIMER_NAME = "Check feasibility time";
const static char * SOLUTION_PRICE_TIMER_NAME = "Pricing time";
const static char * SOLUTION_SELECT_PIVOT_TIMER_NAME = "Pivot selection time";
const static char * SOLUTION_UPDATE_TIMER_NAME = "Update time";

bool Simplex::checkPfiWithFtran(){
    std::vector<int> unitVectors;
    Vector v;
    DEVINFO(D::PFIMAKER, "Check inverse with FTRAN");
    std::vector<int>::iterator it;
    for (it = m_basisHead.begin(); it < m_basisHead.end(); it++) {
        if (*it < (int) m_simplexModel->getMatrix().columnCount()) {
            v = m_simplexModel->getMatrix().column(*it);
        } else {
            v = Vector(m_simplexModel->getMatrix().rowCount());
            v.setNewNonzero((*it) - m_simplexModel->getMatrix().columnCount(), 1);
        }
        m_basis->Ftran(v);
        if (v.nonZeros() == 1 && Numerical::equals(*(v.beginNonzero()), 1.0)) {
            DEVINFO(D::PFIMAKER, "Unit vector reinverted with index: " << v.beginNonzero().getIndex());
            unitVectors.push_back(v.beginNonzero().getIndex());
        }
    }
    std::sort(unitVectors.begin(), unitVectors.end());
    bool success = true;
    for (std::vector<int>::iterator cit = unitVectors.begin(); cit < unitVectors.end(); cit++) {
        if (*cit == cit - unitVectors.begin() || *cit == cit - unitVectors.begin()+(int) m_simplexModel->getMatrix().columnCount()) {
            DEVINFO(D::PFIMAKER, *cit << " FTRAN VECTOR PASSED");
        } else {
            DEVINFO(D::PFIMAKER, *cit << " FTRAN VECTOR FAILED");
            success = false;
        }
    }
    if (success && unitVectors.size() == m_basisHead.size() && m_basisHead.size() > 0) {
//        LPINFO("FTRAN CHECK PASSED");
        return true;
    } else {
//        LPWARNING("FTRAN CHECK FAILED");
        return false;
    }
}

bool Simplex::checkPfiWithBtran() {
    std::vector<int> unitVectors;
    DEVINFO(D::PFIMAKER, "Check inverse with BTRAN");
    DEVINFO(D::PFIMAKER, "BTRAN copy");
    //Copy the basis columns
    std::vector<Vector> basisCopy;
    for (std::vector<int>::iterator it = m_basisHead.begin(); it < m_basisHead.end(); it++) {
        if (*it >= (int) m_simplexModel->getMatrix().columnCount()) {
            Vector logical = Vector(m_simplexModel->getMatrix().rowCount());
            logical.set((*it) - m_simplexModel->getMatrix().columnCount(), 1);
            basisCopy.push_back(logical);
        } else {
            basisCopy.push_back(m_simplexModel->getMatrix().column(*it));
        }
    }
    //Prepare the rows
    DEVINFO(D::PFIMAKER, "BTRAN prepare rows");
    std::vector<Vector> rowbasis;
    for (unsigned int i = 0; i < m_basisHead.size(); i++) {
        Vector row = Vector(m_simplexModel->getMatrix().rowCount());
        rowbasis.push_back(row);
    }
    for (std::vector<Vector>::iterator it = basisCopy.begin(); it < basisCopy.end(); it++) {
        for (Vector::NonzeroIterator vectorIt = it->beginNonzero(); vectorIt < it->endNonzero(); vectorIt++) {
            rowbasis.at(vectorIt.getIndex()).set(it - basisCopy.begin(), *vectorIt);
        }
    }
    //Use BTRAN to check the inverse
    DEVINFO(D::PFIMAKER, "Do the BTRAN check on the rows");
    for (std::vector<Vector>::iterator it = rowbasis.begin(); it < rowbasis.end(); it++) {
        m_basis->Btran(*it);
        // DEVINFO(D::PFIMAKER, "V with Vector print form" <<v);
        if (it->nonZeros() == 1 && Numerical::equals(*(it->beginNonzero()), 1)) {
            DEVINFO(D::PFIMAKER, "Unit vector reinverted with index: " << it - rowbasis.begin());
            unitVectors.push_back(it - rowbasis.begin());
        } else {
            LPERROR ("Rossz vektor: "<<*it);
        }
    }
    std::sort(unitVectors.begin(), unitVectors.end());
    bool success = true;
    for (std::vector<int>::iterator cit = unitVectors.begin(); cit < unitVectors.end(); cit++) {
        DEVINFO(D::PFIMAKER, *cit);
        if (*cit == cit - unitVectors.begin() || *cit == cit - unitVectors.begin()-(int) m_simplexModel->getMatrix().columnCount()) {
            DEVINFO(D::PFIMAKER, *cit << " BTRAN VECTOR PASSED");
        } else {
            DEVINFO(D::PFIMAKER, *cit << " BTRAN VECTOR FAILED");
            success = false;
        }
    }
    if (success && unitVectors.size() == m_basisHead.size() && m_basisHead.size() > 0) {
//        LPINFO("BTRAN CHECK PASSED");
        return true;
    } else {
        LPWARNING("BTRAN CHECK FAILED");
        return false;
    }
}

bool Simplex::checkPfiWithReducedCost() {
    Vector pi(m_simplexModel->getMatrix().rowCount());
    for (std::vector<int>::iterator it = m_basisHead.begin(); it < m_basisHead.end(); it++) {
        if (*it < (int) m_simplexModel->getMatrix().columnCount()) {
            pi.set(it - m_basisHead.begin(), m_simplexModel->getCostVector().at(*it));
        }
    }
    m_basis->Btran(pi);


    int success = true;
    for (std::vector<int>::iterator it = m_basisHead.begin(); it < m_basisHead.end(); it++) {
        //Structural basis vectors are interesting
        if (*it < (int) m_simplexModel->getMatrix().columnCount()) {
            Numerical::Double d = m_simplexModel->getCostVector().at(*it) - pi.dotProduct(m_simplexModel->getMatrix().column(*it));
            if (!Numerical::isZero(d)) {
                success = false;
                LPWARNING("Nonzero basic reduced cost at basis column " << *it << " with value: " << d);
            }
        }
    }
    if (success && m_basisHead.size() > 0) {
        LPINFO("REDUCED COST CHECK PASSED");
        return true;
    } else {
        LPWARNING("REDUCED COST CHECK FAILED");
        return false;
    }
}

bool Simplex::checkAlphaValue(int rowIndex, int columnIndex, double& columnAlpha, double& rowAlpha){
    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();
    Vector row(rowCount + columnCount);

    Vector rho(rowCount); //Row of the inverse of the basis
    rho.setSparsityRatio(DENSE);
    rho.setNewNonzero(rowIndex, 1);

    m_basis->Btran(rho);
    IndexList<const Numerical::Double *>::Iterator it;
    IndexList<const Numerical::Double *>::Iterator itEnd;
    //TODO: A bazisvaltozo egyeset kulon kellene majd bebillenteni hogy gyorsabb legyen
    m_variableStates.getIterators(&it, &itEnd, Simplex::BASIC, 4);
    for(; it != itEnd ; it++){
        unsigned int columnIndex = it.getData();
        if(columnIndex < columnCount){
            row.set(columnIndex, rho.dotProduct(m_simplexModel->getMatrix().column(columnIndex)));
        } else {
            row.set(columnIndex, rho.at(columnIndex - columnCount));
        }
    }

    Vector column(rowCount);
    column.setSparsityRatio(DENSE);
    if(columnIndex < (int)columnCount){
        column = m_simplexModel->getMatrix().column(columnIndex);
    } else {
        column.setNewNonzero(columnIndex - columnCount, 1);
    }
    m_basis->Ftran(column);

    columnAlpha = column.at(rowIndex);
    rowAlpha = row.at(columnIndex);
    LPINFO("Alpha value: row based: "<<row.at(columnIndex));
    LPINFO("Alpha value: column based: "<<column.at(rowIndex));
    LPINFO("Alpha value diff: "<<Numerical::fabs(column.at(rowIndex)- row.at(columnIndex)));
    if(!Numerical::isZero(column.at(rowIndex)- row.at(columnIndex))){
        LPWARNING("ALPHA ERROR!");
        return false;
    } else {
        return true;
    }
}

bool Simplex::checkPrimalTheta(int rowIndex, int columnIndex,
                               Numerical::Double& thetaFromCol, Numerical::Double& thetaFromRow){
    Numerical::Double pivotFromCol;
    Numerical::Double pivotFromRow;

    checkAlphaValue(rowIndex, columnIndex, pivotFromCol, pivotFromRow);

    Variable::VARIABLE_TYPE typeOfIthVariable = m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getType();
    Numerical::Double valueOfOutgoingVariable = m_basicVariableValues.at(rowIndex);

    Numerical::Double computedFromCol = 0;
    Numerical::Double computedFromRow = 0;
    if (typeOfIthVariable == Variable::FIXED) {
        computedFromCol = (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromCol;
        computedFromRow = (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromRow;
    }
    else if (typeOfIthVariable == Variable::BOUNDED) {
        computedFromCol = (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromCol;
        computedFromRow = (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromRow;
    }
    else if (typeOfIthVariable == Variable::PLUS) {
        computedFromCol = (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromCol;
        computedFromRow = (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromRow;
    }
    else if (typeOfIthVariable == Variable::FREE) {
        computedFromCol = valueOfOutgoingVariable / pivotFromCol;
        computedFromRow = valueOfOutgoingVariable  / pivotFromRow;
    }
    else if (typeOfIthVariable == Variable::MINUS) {
        computedFromCol = (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getUpperBound()) /
                pivotFromCol;
        computedFromRow = (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                pivotFromRow;
    }

//    LPINFO("Theta value: get: "<<theta);
//    LPINFO("Theta value: computedFromCol: "<<computedFromCol);
//    LPINFO("Theta value: computedFromRow: "<<computedFromRow);
//    LPINFO("Theta value diffFromCol: "<<Numerical::fabs(theta - computedFromCol));
//    LPINFO("Theta value diffFromRow: "<<Numerical::fabs(theta - computedFromRow));
    thetaFromCol = computedFromCol;
    thetaFromRow = computedFromRow;
    if(!Numerical::isZero(computedFromCol - computedFromRow)){
        LPWARNING("THETA ERROR!");
        return false;
    } else {
        return true;
    }
}

//---------------------------------------------------------

Simplex::Simplex():
    m_simplexModel(NULL),
    m_variableStates(0,0),
    m_variableFeasibilities(0,0),
    m_reducedCostFeasibilities(0,0),
    m_basicVariableValues(0),
    m_reducedCosts(0),
    m_objectiveValue(0),
    m_phaseIObjectiveValue(0),
    m_baseChanged(false),
    m_freshBasis(false),
    m_startingBasisFinder(NULL),
    m_basis(NULL),
    m_iterationIndex(0)
{
    m_basicVariableValues.setSparsityRatio(DENSE);
    m_reducedCosts.setSparsityRatio(DENSE);

//    fpu_fix_start(&m_old_cw);
}

Simplex::~Simplex() {
    releaseModules();

    if(m_simplexModel){
        delete m_simplexModel;
        m_simplexModel = 0;
    }
//    fpu_fix_end(&m_old_cw);
}

std::vector<IterationReportField> Simplex::getIterationReportFields(
        enum ITERATION_REPORT_FIELD_TYPE & type) const {
    std::vector<IterationReportField> result;

    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
    {
        IterationReportField inversionField(ITERATION_INVERSION_NAME, 3, 1, IterationReportField::IRF_CENTER,
                                    IterationReportField::IRF_STRING, *this);
        result.push_back(inversionField);
        IterationReportField iterationField(ITERATION_INDEX_NAME, 9, 1, IterationReportField::IRF_CENTER,
                                    IterationReportField::IRF_INT, *this);
        result.push_back(iterationField);
        IterationReportField timeField(ITERATION_TIME_NAME, 10, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(timeField);
    }
        break;

    case IterationReportProvider::IRF_SOLUTION:
        IterationReportField iterationField(SOLUTION_ITERATION_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_INT, *this);
        result.push_back(iterationField);
        IterationReportField solveTimerField(SOLUTION_SOLVE_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(solveTimerField);
        IterationReportField inversionTimerField(SOLUTION_INVERSION_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(inversionTimerField);
        IterationReportField computeBasicSolutionTimerField(SOLUTION_COMPUTE_BASIC_SOLUTION_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(computeBasicSolutionTimerField);
        IterationReportField computeReducedCostsField(SOLUTION_COMPUTE_REDUCED_COSTS_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(computeReducedCostsField);
        IterationReportField computeFeasibilityTimerField(SOLUTION_COMPUTE_FEASIBILITY_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(computeFeasibilityTimerField);
        IterationReportField checkFeasibilityTimerField(SOLUTION_CHECK_FEASIBILITY_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(checkFeasibilityTimerField);
        IterationReportField priceTimerField(SOLUTION_PRICE_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(priceTimerField);
        IterationReportField selectpivotTimerField(SOLUTION_SELECT_PIVOT_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(selectpivotTimerField);
        IterationReportField updateTimerField(SOLUTION_UPDATE_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(updateTimerField);
        break;

    }

    return result;
}

Entry Simplex::getIterationEntry(const string &name,
                                                     ITERATION_REPORT_FIELD_TYPE &type) const {
    Entry reply;
    reply.m_integer = 0;
    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
        if (name == ITERATION_INDEX_NAME) {
            reply.m_integer = m_iterationIndex;
        } else if (name == ITERATION_TIME_NAME) {
            reply.m_float = m_solveTimer.getRunningTime()/1000000;
        } else if (name == ITERATION_INVERSION_NAME) {
            if(m_freshBasis){
                reply.m_string = new std::string("*I*");
            } else {
                reply.m_string = new std::string("");
            }
        }
        break;

    case IterationReportProvider::IRF_SOLUTION:
        if (name == SOLUTION_ITERATION_NAME) {
            reply.m_integer = m_iterationIndex;
        } else if (name == SOLUTION_SOLVE_TIMER_NAME) {
            reply.m_float = m_solveTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_INVERSION_TIMER_NAME) {
            reply.m_float = m_inversionTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_COMPUTE_BASIC_SOLUTION_TIMER_NAME) {
            reply.m_float = m_computeBasicSolutionTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_COMPUTE_REDUCED_COSTS_TIMER_NAME) {
            reply.m_float = m_computeReducedCostsTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_COMPUTE_FEASIBILITY_TIMER_NAME) {
            reply.m_float = m_computeFeasibilityTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_CHECK_FEASIBILITY_TIMER_NAME) {
            reply.m_float = m_checkFeasibilityTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_PRICE_TIMER_NAME) {
            reply.m_float = m_priceTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_SELECT_PIVOT_TIMER_NAME) {
            reply.m_float = m_selectPivotTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_UPDATE_TIMER_NAME) {
            reply.m_float = m_updateTimer.getTotalElapsed()/1000000;
        }
        break;

    }

    return reply;
}

void Simplex::setModel(const Model &model) {
    if(m_simplexModel){
        delete m_simplexModel;
    }
    unregisterMethodWithModel(this, model);
    m_simplexModel = new SimplexModel(model);
    registerMethodWithModel(this, model);

    //Init the data structures
    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();
    m_basisHead.resize(rowCount, INVALID_POSITION);
    m_variableStates.init(rowCount + columnCount, VARIABLE_STATE_ENUM_LENGTH);
    m_variableFeasibilities.init(rowCount + columnCount, FEASIBILITY_ENUM_LENGTH);
    m_reducedCostFeasibilities.init(rowCount + columnCount, FEASIBILITY_ENUM_LENGTH);
    m_basicVariableValues.reInit(rowCount);
    m_reducedCosts.reInit(rowCount + columnCount);
    //TODO: Ezt ne kelljen ujra beallitani egy init miatt
    m_basicVariableValues.setSparsityRatio(DENSE);
    m_reducedCosts.setSparsityRatio(DENSE);
}

void Simplex::constraintAdded()
{

}

void Simplex::variableAdded()
{

}

void Simplex::saveBasis(const char * fileName, BasisHeadIO * basisWriter, bool releaseWriter) {
    basisWriter->startWrting(fileName);
    int variableIndex;
    unsigned int position = 0;
    STL_FOREACH(std::vector<int>, m_basisHead, basisHeadIter) {
        variableIndex = *basisHeadIter;
        const Variable * variable = &m_simplexModel->getVariable(variableIndex);
        basisWriter->addBasicVariable( variable, position, m_basicVariableValues.at(position) );
        position++;
    }

    IndexList<const Numerical::Double *>::Iterator iter, iterEnd;
    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_LB, 1);
    for (; iter != iterEnd; iter++) {
        variableIndex = iter.getData();
        const Variable * variable = &m_simplexModel->getVariable(variableIndex);
        // TODO: itt az erteket nem kell majd atadni
        basisWriter->addNonbasicVariable(variable, Simplex::NONBASIC_AT_LB, variable->getLowerBound());
    }

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_UB, 1);
    for (; iter != iterEnd; iter++) {
        variableIndex = iter.getData();
        const Variable * variable = &m_simplexModel->getVariable(variableIndex);
        // TODO: itt az erteket nem kell majd atadni
        basisWriter->addNonbasicVariable(variable, Simplex::NONBASIC_AT_UB, variable->getUpperBound());
    }

    basisWriter->finishWriting();

    if (releaseWriter == true) {
        delete basisWriter;
    }
}

void Simplex::loadBasis(const char * fileName, BasisHeadIO * basisReader, bool releaseReader) {
    const unsigned int variableCount = m_simplexModel->getColumnCount() + m_simplexModel->getRowCount();
    basisReader->startReading(fileName, variableCount);
    unsigned int index;
    for (index = 0; index < variableCount; index++) {
        basisReader->addVariable(m_simplexModel->getVariable(index));
    }

    basisReader->finishReading(&m_basisHead, &m_variableStates);

    if (releaseReader) {
        delete basisReader;
    }
}


void Simplex::solve() {
    initModules();

    ParameterHandler & simplexParameters = SimplexParameterHandler::getInstance();

    const unsigned int iterationLimit = simplexParameters.getParameterValue("iteration_limit");
    const double timeLimit = simplexParameters.getParameterValue("time_limit");
    StartingBasisFinder::STARTING_BASIS_STRATEGY startingBasisStratgy =
            (StartingBasisFinder::STARTING_BASIS_STRATEGY)simplexParameters.getParameterValue("starting_basis");
    unsigned int reinversionFrequency = simplexParameters.getParameterValue("reinversion_frequency");
    unsigned int reinversionCounter = reinversionFrequency;

    m_iterationReport.addProviderForStart(*this);
    m_iterationReport.addProviderForIteration(*this);
    m_iterationReport.addProviderForSolution(*this);

    m_debugLevel = SimplexParameterHandler::getInstance().getParameterValue("debug_level");
    m_iterationReport.setDebugLevel(m_debugLevel);

    m_iterationReport.createStartReport();
    m_iterationReport.writeStartReport();

    LPINFO("-----------------------------------");

    try {
        m_solveTimer.start();
        m_basicVariableValues.prepareForData(m_simplexModel->getRhs().nonZeros(), m_basicVariableValues.length(), DENSE);
        Vector::NonzeroIterator it = m_simplexModel->getRhs().beginNonzero();
        Vector::NonzeroIterator itend = m_simplexModel->getRhs().endNonzero();
        for(; it < itend; it++){
            m_basicVariableValues.newNonZero(*it, it.getIndex());
        }
        m_startingBasisFinder->findStartingBasis(startingBasisStratgy);
//        loadBasis("basis.bas", new BasisHeadBAS, true);
        //exit(1);
//        saveBasis("basis2.bas", new BasisHeadBAS, true);
        for (m_iterationIndex = 1; m_iterationIndex <= iterationLimit && (m_solveTimer.getRunningTime()/1000000) < timeLimit; m_iterationIndex++) {
            // ITTEN MENTJUK KI A BAZIST:
//            if(m_iterationIndex == 10)
      //        saveBasis("basis.bas", new BasisHeadBAS, true);

            if(reinversionCounter == reinversionFrequency){
                m_freshBasis = true;
                reinversionCounter = 0;
                m_inversionTimer.start();
                reinvert();
                m_inversionTimer.stop();
                m_computeBasicSolutionTimer.start();
                computeBasicSolution();
                m_computeBasicSolutionTimer.stop();
                m_computeReducedCostsTimer.start();
                computeReducedCosts();
                m_computeReducedCostsTimer.stop();
                m_computeFeasibilityTimer.start();
                computeFeasibility();
                m_computeFeasibilityTimer.stop();
            }
            try{
//                if(m_iterationIndex>820){
//                    checkPfiWithFtran();
//                    checkPfiWithBtran();
//                    checkPfiWithReducedCost();
//                }

                m_checkFeasibilityTimer.start();
                checkFeasibility();
                m_checkFeasibilityTimer.stop();
                m_priceTimer.start();
                price();
                m_priceTimer.stop();
                m_selectPivotTimer.start();
                selectPivot();
                m_selectPivotTimer.stop();
                m_updateTimer.start();
                //TODO: Do not use the update formulas before reinversion
                //TODO: UGY NEM JO AHOGY VOLT, az append is az update-be van!
//                if(reinversionCounter+1 < reinversionFrequency){
                update();
//                }
                m_updateTimer.stop();

                if(m_debugLevel>1 || (m_debugLevel==1 && m_freshBasis)){
                    m_iterationReport.createIterationReport();
                    m_iterationReport.writeIterationReport();
                }
                m_freshBasis = false;
                reinversionCounter++;
            }
            catch ( const OptimalException & exception ) {
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    m_solveTimer.stop();
                    throw exception;
                } else {
//                    LPINFO("TRIGGERING REINVERSION TO CHECK OPTIMALITY! ");
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            }
            catch ( const PrimalInfeasibleException & exception ) {
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    m_solveTimer.stop();
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            }
            catch ( const DualInfeasibleException & exception ) {
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    m_solveTimer.stop();
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            }
            catch ( const PrimalUnboundedException & exception ) {
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    m_solveTimer.stop();
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            }
            catch ( const DualUnboundedException & exception ) {
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    m_solveTimer.stop();
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            }
            catch ( const NumericalException & exception ) {
                //Check the result with triggering reinversion
                LPINFO("TRIGGERING REINVERSION TO HANDLE NUMERICAL ERROR! ");
                if(m_freshBasis){
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            }
        }

    } catch ( const OptimalException & exception ) {
        LPINFO("OPTIMAL SOLUTION found! ");
        LPINFO("The objective value: " << m_objectiveValue);
        // TODO: postsovle, post scaling
        // TODO: Save optimal basis if necessary
    } catch ( const PrimalInfeasibleException & exception ) {
        LPERROR("The problem is PRIMAL INFEASIBLE.");
    } catch ( const DualInfeasibleException & exception ) {
        LPERROR("The problem is DUAL INFEASIBLE.");
    } catch ( const PrimalUnboundedException & exception ) {
        LPERROR("The problem is PRIMAL UNBOUNDED.");
    } catch ( const DualUnboundedException & exception ) {
        LPERROR("The problem is DUAL UNBOUNDED.");
    } catch ( const NumericalException & exception ) {
        LPERROR("Numerical error!");
    } catch ( const std::bad_alloc & exception ) {
        LPERROR("STL bad alloc exception: " << exception.what() );
    } catch ( const std::bad_cast & exception ) {
        LPERROR("STL bad cast exception: " << exception.what() );
    } catch ( const std::bad_exception & exception ) {
        LPERROR("STL bad exception exception: " << exception.what() );
    } catch ( const std::bad_typeid & exception ) {
        LPERROR("STL bad typeid exception: " << exception.what() );
    } catch (const std::ios_base::failure & exception ) {
        LPERROR("STL ios_base::failure exception: " << exception.what() );
    } catch (const std::logic_error & exception ) {
        LPERROR("STL logic error exception: " << exception.what() );
    } catch ( const std::range_error & exception ) {
        LPERROR("STL range error: " << exception.what() );
    } catch ( const std::overflow_error & exception ) {
        LPERROR("STL arithmetic overflow error: " << exception.what() );
    } catch ( const std::underflow_error & exception ) {
        LPERROR("STL arithmetic underflow error: " << exception.what() );
#if __cplusplus > 199711L
    } catch ( const std::system_error & exception ) {
        //LPERROR("STL system error: \"" << exception.code().message() << "\" " << exception.what() );
        LPERROR("STL system error: " << std::endl);
        LPERROR("\tError: " << exception.what() << std::endl);
        LPERROR("\tCode: " << exception.code().value() << std::endl);
        LPERROR("\tCategory: " << exception.code().category().name() << std::endl);
        LPERROR("\tMessage: " << exception.code().message() << std::endl);
    } catch ( const std::bad_function_call & exception ) {
        LPERROR("STL bad function call exception: " << exception.what() );
#endif
    } catch (const std::runtime_error & exception ) {
        LPERROR("STL runtime error: " << exception.what() );
    }
    catch (const std::exception & exception) {
        LPERROR("General STL exception: " << exception.what());
    } catch (...) {
        LPERROR("Unknown exception");
    }

    m_iterationReport.createSolutionReport();
    m_iterationReport.writeSolutionReport();

    releaseModules();
}

void Simplex::initModules() {
    m_startingBasisFinder = new StartingBasisFinder(*m_simplexModel, &m_basisHead, &m_variableStates, &m_basicVariableValues);
    double factorizationType = SimplexParameterHandler::getInstance().getParameterValue("factorization_type");
    if (factorizationType == 0){
        m_basis = new PfiBasis(*m_simplexModel, &m_basisHead, &m_variableStates, m_basicVariableValues);
    } else {
        LPERROR("Wrong parameter: factorization_type");
        //TODO: Throw parameter exception
    }
}

void Simplex::releaseModules() {
    if(m_startingBasisFinder){
        delete m_startingBasisFinder;
        m_startingBasisFinder = 0;
    }
    if(m_basis){
        delete m_basis;
        m_basis = 0;
    }
}

const std::vector<Numerical::Double> Simplex::getPrimalSolution() const {
    std::vector<Numerical::Double> result;
    unsigned int totalVariableCount = m_simplexModel->getColumnCount() + m_simplexModel->getRowCount();
    result.reserve(totalVariableCount);
    for(unsigned int i = 0; i<totalVariableCount; i++) {
        result[i] = *(m_variableStates.getAttachedData(i));
    }
    return result;
}


const std::vector<Numerical::Double> Simplex::getDualSolution() const {
    return std::vector<Numerical::Double>();
    //TODO
}


void Simplex::reinvert() throw (NumericalException) {
    m_basis->invert();
}

void Simplex::computeBasicSolution() throw (NumericalException) {
    m_basicVariableValues = m_simplexModel->getRhs();
    m_objectiveValue = - m_simplexModel->getCostConstant();


    const unsigned int columnCount = m_simplexModel->getColumnCount();
    //x_B=B^{-1}*(b-\SUM{U*x_U}-\SUM{L*x_L})
    IndexList<const Numerical::Double *>::Iterator it;
    IndexList<const Numerical::Double *>::Iterator itend;
    //This iterates through Simplex::NONBASIC_AT_LB and Simplex::NONBASIC_AT_UB too -- BUG with 2 partitions
    m_variableStates.getIterators(&it, &itend, Simplex::NONBASIC_AT_LB,2);

    for(; it != itend; it++) {
        if(*(it.getAttached()) != 0){
            if(it.getData() < columnCount){
                m_basicVariableValues.addVector(-1 * *(it.getAttached()), m_simplexModel->getMatrix().column(it.getData()));
                m_objectiveValue += m_simplexModel->getCostVector().at(it.getData()) * *(it.getAttached());
            } else {
                m_basicVariableValues.set(it.getData() - columnCount,
                                          Numerical::stableSub(m_basicVariableValues.at(it.getData() - columnCount), *(it.getAttached())));
            }
        }
    }

    //    This also sets the basic solution since the pointers of the basic variables point to the basic variable values vector
    m_basis->Ftran(m_basicVariableValues);

    m_variableStates.getIterators(&it, &itend, Simplex::BASIC);
    for(; it != itend; it++) {
        if(it.getData() < columnCount){
            m_objectiveValue += m_simplexModel->getCostVector().at(it.getData()) * *(it.getAttached());
        }
    }
}

void Simplex::computeReducedCosts() throw (NumericalException) {
    m_reducedCosts.clear();
    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();

    //Get the c_B vector
    Vector simplexMultiplier(rowCount);
    simplexMultiplier.setSparsityRatio(DENSE);
    const Vector& costVector = m_simplexModel->getCostVector();
    for(unsigned int i = 0; i<m_basisHead.size(); i++){
        if(m_basisHead[i] < (int) columnCount && costVector.at(m_basisHead[i]) != 0.0){
            simplexMultiplier.setNewNonzero(i, costVector.at(m_basisHead[i]));
        }
    }
    //Compute simplex multiplier
    m_basis->Btran(simplexMultiplier);

    //For each variable
    for(unsigned int i = 0; i < rowCount + columnCount; i++) {
        //Compute the dot product and the reduced cost
        Numerical::Double reducedCost;
        if(i < columnCount){
            reducedCost = Numerical::stableSub(costVector.at(i), simplexMultiplier.dotProduct(m_simplexModel->getMatrix().column(i)));
        } else {
            reducedCost = -1 * simplexMultiplier.at(i - columnCount);
        }
        if(reducedCost != 0.0){
            m_reducedCosts.setNewNonzero(i, reducedCost);
        }
    }
}
