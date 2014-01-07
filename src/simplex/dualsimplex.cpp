/**
 * @file dualsimplex.cpp
 */

#include <algorithm>

#include <simplex/dualsimplex.h>
#include <simplex/dualpricingfactory.h>
#include <simplex/pricing/dualdantzigpricingfactory.h>
#include <simplex/pfibasis.h>

#include <utils/thirdparty/prettyprint.h>

const static char * INCOMING_NAME = "Incoming";
const static char * OUTGOING_NAME = "Outgoing";
const static char * PHASE_NAME = "Phase";
const static char * PHASE_1_STRING = "ph-1";
const static char * PHASE_2_STRING = "ph-2";
const static char * PHASE_UNKNOWN_STRING = "unknown";
const static char * PHASE_1_OBJ_VAL_STRING = "Dual infeasibility";
const static char * OBJ_VAL_STRING = "Objective value";
const static char * PRIMAL_REDUCED_COST_STRING = "Reduced cost";
const static char * PRIMAL_THETA_STRING = "Primal theta";
const static char * DUAL_THETA_STRING = "Dual theta";

DualSimplex::DualSimplex():
    m_pricing(0),
    m_updater(0),
    m_feasibilityChecker(0),
    m_ratiotest(0),
    m_feasible(false),
    m_phaseName(PHASE_UNKNOWN_STRING)
{

}

// Interface of the iteration report provider:
std::vector<IterationReportField> DualSimplex::getIterationReportFields(
        enum ITERATION_REPORT_FIELD_TYPE & type) const {
    std::vector<IterationReportField> result = Simplex::getIterationReportFields(type);

    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
    {
        IterationReportField incomingField(INCOMING_NAME, 10, 2, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_INT, *this);
        result.push_back(incomingField);

        IterationReportField outgoingField(OUTGOING_NAME, 10, 2, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_INT, *this);
        result.push_back(outgoingField);

        IterationReportField primalReducedCostField(PRIMAL_REDUCED_COST_STRING, 15, 3, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               -1, IterationReportField::IRF_FIXED);
        result.push_back(primalReducedCostField);

        IterationReportField primalThetaField(PRIMAL_THETA_STRING, 15, 3, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               -1, IterationReportField::IRF_FIXED);
        result.push_back(primalThetaField);

        IterationReportField dualThetaField(DUAL_THETA_STRING, 15, 3, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               -1, IterationReportField::IRF_FIXED);
        result.push_back(dualThetaField);

        IterationReportField phase1ObjValField(PHASE_1_OBJ_VAL_STRING, 19, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               10, IterationReportField::IRF_SCIENTIFIC);
        result.push_back(phase1ObjValField);

        IterationReportField objValField(OBJ_VAL_STRING, 19, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               10, IterationReportField::IRF_SCIENTIFIC);
        result.push_back(objValField);

        IterationReportField phaseField(PHASE_NAME, 6, 1, IterationReportField::IRF_RIGHT,
                                        IterationReportField::IRF_STRING, *this);
        result.push_back(phaseField);
    }
        break;

    case IterationReportProvider::IRF_SOLUTION:
    {
        IterationReportField objValField(OBJ_VAL_STRING, 19, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               10, IterationReportField::IRF_SCIENTIFIC);
        result.push_back(objValField);
        break;
    }

    default:
        break;
    }

    return result;
}

Entry DualSimplex::getIterationEntry(const string &name,
                                                         ITERATION_REPORT_FIELD_TYPE &type) const {
    Entry reply;
    reply.m_integer = 0;

    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
        if (name == PHASE_NAME) {
            reply.m_string = new std::string(m_phaseName);
        } else if (name == INCOMING_NAME) {
            reply.m_integer = m_incomingIndex;
        } else if (name == OUTGOING_NAME) {
            reply.m_integer = m_outgoingIndex;
        } else if (name == PHASE_1_OBJ_VAL_STRING) {
            reply.m_double = m_phaseIObjectiveValue;
        } else if (name == OBJ_VAL_STRING) {
            if(m_simplexModel->getObjectiveType() == MINIMIZE){
                reply.m_double = m_objectiveValue;
            } else {
                reply.m_double = -m_objectiveValue;
            }
        } else if (name == PRIMAL_REDUCED_COST_STRING) {
            reply.m_double = m_primalReducedCost;
        } else if (name == PRIMAL_THETA_STRING) {
            reply.m_double = m_primalTheta;
        } else if (name == DUAL_THETA_STRING) {
            reply.m_double = m_dualTheta;
        } else {
            break;
        }
        return reply;

    case IterationReportProvider::IRF_SOLUTION:
        if (name == OBJ_VAL_STRING) {
            if(m_simplexModel->getObjectiveType() == MINIMIZE){
                reply.m_double = m_objectiveValue;
            } else {
                reply.m_double = -m_objectiveValue;
            }
        } else {
            break;
        }
        return reply;

    default:
        break;
    }

    return Simplex::getIterationEntry(name, type);
}


void DualSimplex::initModules() {
    Simplex::initModules();


    // TODO: ezt majd egy switch-case donti el, amit lehetne
    // kulon fuggvenybe is tenni akar
    DualPricingFactory * pricingFactory = new DualDantzigPricingFactory;

    m_updater = new DualUpdater;

    DualPricingUpdater * pricingUpdater = pricingFactory->createDualPricingUpdater(
                m_basicVariableValues,
                &m_variableFeasibilities,
                m_reducedCostFeasibilities,
                m_basisHead,
                *m_simplexModel,
                *m_basis
                );
    m_updater->setPricingUpdater( pricingUpdater );
    m_pricing = pricingFactory->createDualPricing( *m_simplexModel,
                                                   pricingUpdater,
                                                   m_reducedCosts,
                                                   m_basisHead);

    m_feasibilityChecker=new DualFeasibilityChecker(*m_simplexModel,
                                                    &m_variableStates,
                                                    &m_reducedCostFeasibilities,
                                                    m_reducedCosts,
                                                    *m_basis,
                                                    &m_phaseIObjectiveValue);

    DualRatiotestUpdater * ratiotestUpdater = new DualRatiotestUpdater(&m_reducedCostFeasibilities);
    m_updater->setRatiotestUpdater( ratiotestUpdater );
    m_ratiotest = new DualRatiotest(*m_simplexModel,
                                    m_reducedCosts,
                                    m_reducedCostFeasibilities,
                                    m_variableStates,
                                    *ratiotestUpdater);

    delete pricingFactory;
    pricingFactory = 0;

    //registerIntoIterationReport(*this);
}

void DualSimplex::releaseModules() {
    Simplex::releaseModules();

    if(m_pricing){
        delete m_pricing;
        m_pricing = 0;
    }

    if(m_updater){
        delete m_updater;
        m_updater = 0;
    }

    if(m_feasibilityChecker){
        delete m_feasibilityChecker;
        m_feasibilityChecker = 0;
    }

    if(m_ratiotest){
        delete m_ratiotest;
        m_ratiotest = 0;
    }
}


void DualSimplex::computeFeasibility() throw (NumericalException) {
    m_feasibilityChecker->computeFeasibility(m_expandingTolerance);
}

void DualSimplex::checkFeasibility() throw (OptimizationResultException, NumericalException) {
    bool lastFeasible = m_feasible;
    m_feasible = m_feasibilityChecker->checkFeasibility();
    if(lastFeasible == false && m_feasible == true){
        m_feasibilityChecker->feasiblityCorrection(&m_basicVariableValues);
    }
}

void DualSimplex::price() throw (OptimizationResultException, NumericalException) {
    if(!m_feasible){
        m_phaseName = PHASE_1_STRING;
        //LPINFO("PHASE1 PRICE");
        m_outgoingIndex = m_pricing->performPricingPhase1();
        if(m_outgoingIndex == -1){
            throw DualInfeasibleException("The problem is DUAL INFEASIBLE!");
        }
    } else {
        m_phaseName = PHASE_2_STRING;
        //LPINFO("PHASE2 PRICE");
        m_outgoingIndex = m_pricing->performPricingPhase2();
        if(m_outgoingIndex == -1){
            throw OptimalException("OPTIMAL SOLUTION found!");
        }
    }
}

void DualSimplex::selectPivot() throw (OptimizationResultException, NumericalException) {

    Vector* alpha;
    computeTransformedRow(alpha, m_outgoingIndex);

    if(!m_feasible){
        m_ratiotest->performRatiotestPhase1(*alpha, m_pricing->getReducedCost(), m_phaseIObjectiveValue,m_expandingTolerance);
    } else {
        m_ratiotest->performRatiotestPhase2(m_basisHead[m_outgoingIndex], *alpha, m_objectiveValue);
    }
    m_incomingIndex = m_ratiotest->getIncomingVariableIndex();

    m_dualTheta = m_ratiotest->getDualSteplength();

    delete alpha;
}

void DualSimplex::update()throw (NumericalException) {

    if(m_incomingIndex != -1){
        m_primalReducedCost = m_reducedCosts.at(m_incomingIndex);
    } else {
        //TODO: Bad incoming index, reinvert
        throw NumericalException("Invalid incoming index");
        m_primalReducedCost = std::numeric_limits<double>::signaling_NaN();
    }



    if(m_feasible){
        for(unsigned int varIndex = 0; varIndex<m_reducedCosts.length(); varIndex++){
            const Variable& variable = m_simplexModel->getVariable(varIndex);
            if (variable.getType() == Variable::BOUNDED){
                if(m_variableStates.where(varIndex) == 1 && m_reducedCosts.at(varIndex)<0){
                    LPERROR("ALSO KORLATOS HIBA " << varIndex);
                    LPERROR("m_variableStates.where(variableIndex) "<<m_variableStates.where(varIndex));
                    LPERROR("m_reducedCosts.at(variableIndex) "<<m_reducedCosts.at(varIndex) << " - "<<varIndex);

                    exit(-1);
                }
                if(m_variableStates.where(varIndex) == 2 && m_reducedCosts.at(varIndex)>0){
                    LPERROR("FELSO KORLATOS HIBA " << varIndex);
                    LPERROR("m_variableStates.where(variableIndex) "<<m_variableStates.where(varIndex));
                    LPERROR("m_reducedCosts.at(variableIndex) "<<m_reducedCosts.at(varIndex) << " - "<<varIndex);

                    exit(-1);
                }
            }
        }
    }



    //Todo update the solution properly
    //    Numerical::Double boundflipTheta = 0.0;
    std::vector<unsigned int>::const_iterator it = m_ratiotest->getBoundflips().begin();
    std::vector<unsigned int>::const_iterator itend = m_ratiotest->getBoundflips().end();

    for(; it < itend; it++){
//        LPWARNING("BOUNDFLIPPING at: "<<*it);
        const Variable& variable = m_simplexModel->getVariable(*it);
        Numerical::Double boundDistance = Numerical::fabs(variable.getLowerBound() - variable.getUpperBound());
        //Alpha is not available, since we are in the dual
        Vector alpha = m_simplexModel->getMatrix().column(*it);
        m_basis->Ftran(alpha);
        if(m_variableStates.where(*it) == Simplex::NONBASIC_AT_LB) {
            m_basicVariableValues.addVector(-1 * boundDistance, alpha);
            m_variableStates.move(*it, Simplex::NONBASIC_AT_UB, &(variable.getUpperBound()));
        } else if(m_variableStates.where(*it) == Simplex::NONBASIC_AT_UB){
            m_basicVariableValues.addVector(-1 * boundDistance, alpha);
            m_variableStates.move(*it, Simplex::NONBASIC_AT_LB, &(variable.getLowerBound()));
        } else {
            LPERROR("Boundflipping variable in the basis (or superbasic)!");
            //TODO Throw some exception here
        }
    }

    if(m_outgoingIndex != -1 && m_incomingIndex != -1){
        //Save whether the basis is to be changed
        m_baseChanged = true;

        unsigned int rowCount = m_simplexModel->getRowCount();
        unsigned int columnCount = m_simplexModel->getColumnCount();
        Vector alpha(rowCount);
        if(m_incomingIndex < (int)columnCount){
            alpha = m_simplexModel->getMatrix().column(m_incomingIndex);
        } else {
            alpha.setNewNonzero(m_incomingIndex - columnCount, 1);
        }
        m_basis->Ftran(alpha);



        if(!m_feasible){
            //Phase-I: Bounded variable leaves at lower bound (feasibility correction will handle it)
            m_primalTheta = computePrimalTheta(alpha, m_outgoingIndex);
        }

        if(m_feasible &&
                (m_basicVariableValues.at(m_outgoingIndex) -
                m_simplexModel->getVariable(m_basisHead[m_outgoingIndex]).getLowerBound()) < 0){
            //Phase-II: Bounded variable leaves at lower bound (comes from M)
            m_primalTheta = computePrimalTheta(alpha, m_outgoingIndex, false);
        }else{
            //Phase-II: Bounded variable leaves at upper bound (comes from P)
            m_primalTheta = computePrimalTheta(alpha, m_outgoingIndex, true);
        }

        m_basicVariableValues.addVector(-1 * m_primalTheta, alpha);
        m_objectiveValue += m_primalReducedCost * m_primalTheta;
        //The incoming variable is NONBASIC thus the attached data gives the appropriate bound or zero

        m_basis->append(alpha, m_outgoingIndex, m_incomingIndex);
        m_basicVariableValues.set(m_outgoingIndex, *(m_variableStates.getAttachedData(m_incomingIndex)) + m_primalTheta);
    }


//    computeBasicSolution();

    computeReducedCosts();

    computeFeasibility();

    //Do dual specific using the updater
    m_updater->update(m_feasible ? 2 : 1);
}

void DualSimplex::computeTransformedRow(Vector*& alpha, unsigned int rowIndex) throw(NumericalException) {

    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();

    if (rowIndex > rowCount){
        LPERROR("Invalid row index!");
        throw NumericalException("Invalid row index, the transformed row cannot be computed");
    }


    Vector rho(rowCount); //Row of the inverse of the basis
    rho.setSparsityRatio(DENSE);
    rho.setNewNonzero(rowIndex, 1);
    m_basis->Btran(rho);

//    alpha = new Vector(columnCount);
//    alpha->setSparsityRatio(DENSE);
//    Vector::NonzeroIterator it = rho.beginNonzero();
//    Vector::NonzeroIterator itend = rho.endNonzero();
//    for(; it != itend; it++){
//        alpha->addVector(*it,m_simplexModel->getMatrix().row(it.getIndex()));
//    }
//    alpha->appendVector(rho);

    alpha = new Vector(rowCount + columnCount);
    alpha->setSparsityRatio(DENSE);
    IndexList<const Numerical::Double *>::Iterator it;
    IndexList<const Numerical::Double *>::Iterator itEnd;
    //TODO: A bazisvaltozo egyeset kulon kellene majd bebillenteni hogy gyorsabb legyen
    m_variableStates.getIterators(&it, &itEnd, Simplex::BASIC, 4);
    for(; it != itEnd ; it++){
        unsigned int columnIndex = it.getData();
        if(columnIndex < columnCount){
            alpha->set(columnIndex, rho.dotProduct(m_simplexModel->getMatrix().column(columnIndex)));
        } else {
            alpha->set(columnIndex, rho.at(columnIndex - columnCount));
        }
    }

//    LPINFO("rho: "<<rho);
//    LPINFO("alpha: "<<*alpha);
//    LPINFO("alpha2: "<<alpha2);

//    Numerical::Double x1,x2;
//    for(int i=0; i<alpha->length();i++){
//        if(alpha->at(i) != alpha2.at(i)){
//            checkAlphaValue(rowIndex,i,x1,x2);
//            LPINFO("ALPHA: "<<alpha->at(i)<<" - "<<alpha2.at(i) << " col: "<<x1);
//        }
//    }
}

Numerical::Double DualSimplex::computePrimalTheta(const Vector& alpha, int rowIndex, bool upperBound){
    Variable::VARIABLE_TYPE typeOfIthVariable = m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getType();
    Numerical::Double valueOfOutgoingVariable = m_basicVariableValues.at(rowIndex);

    if (typeOfIthVariable == Variable::FIXED) {
        return (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                alpha.at(rowIndex);
    }
    else if (typeOfIthVariable == Variable::BOUNDED) {
        if(upperBound){
            return (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getUpperBound()) /
                    alpha.at(rowIndex);
        } else {
            return (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                    alpha.at(rowIndex);
        }
    }
    else if (typeOfIthVariable == Variable::PLUS) {
        return (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                alpha.at(rowIndex);
    }
    else if (typeOfIthVariable == Variable::FREE) {
        return valueOfOutgoingVariable / alpha.at(rowIndex);
    }
    else if (typeOfIthVariable == Variable::MINUS) {
        return (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getUpperBound()) /
                alpha.at(rowIndex);
    }
    return 0;
}
