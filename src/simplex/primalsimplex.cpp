/**
 * @file primalsimplex.cpp
 */

#include <simplex/primalsimplex.h>
#include <simplex/primalpricingfactory.h>
#include <simplex/pricing/primaldantzigpricingfactory.h>

#include <simplex/simplexparameterhandler.h>

const static char * INCOMING_NAME = "Incoming";
const static char * OUTGOING_NAME = "Outgoing";
const static char * PHASE_NAME = "Phase";
const static char * PHASE_1_STRING = "ph-1";
const static char * PHASE_2_STRING = "ph-2";
const static char * PHASE_UNKNOWN_STRING = "unknown";
const static char * PHASE_1_OBJ_VAL_STRING = "Primal infeasibility";
const static char * OBJ_VAL_STRING = "Objective value";
const static char * PRIMAL_REDUCED_COST_STRING = "Reduced cost";
const static char * PRIMAL_THETA_STRING = "Theta";

PrimalSimplex::PrimalSimplex():
    m_pricing(0),
    m_updater(0),
    m_feasibilityChecker(0),
    m_ratiotest(0),
    m_phaseName(PHASE_UNKNOWN_STRING)
{
    m_workingTolerance = SimplexParameterHandler::getInstance().getDoubleParameterValue("e_feasibility");
}


// Interface of the iteration report provider:
std::vector<IterationReportField> PrimalSimplex::getIterationReportFields(
        enum ITERATION_REPORT_FIELD_TYPE & type) const {
    std::vector<IterationReportField> result = Simplex::getIterationReportFields(type);

    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
    {
        result.push_back(IterationReportField (INCOMING_NAME, 10, 2, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_INT, *this));

        result.push_back(IterationReportField (OUTGOING_NAME, 10, 2, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_INT, *this));

        result.push_back(IterationReportField (PRIMAL_REDUCED_COST_STRING, 15, 3, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               -1, IterationReportField::IRF_FIXED));

        result.push_back(IterationReportField (PRIMAL_THETA_STRING, 15, 3, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               -1, IterationReportField::IRF_FIXED));

        result.push_back(IterationReportField (PHASE_1_OBJ_VAL_STRING, 20, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               10, IterationReportField::IRF_SCIENTIFIC));

        result.push_back(IterationReportField (OBJ_VAL_STRING, 20, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               10, IterationReportField::IRF_SCIENTIFIC));

        result.push_back(IterationReportField (PHASE_NAME, 6, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_STRING, *this));
    }
        break;

    case IterationReportProvider::IRF_SOLUTION:
    {
        result.push_back(IterationReportField (OBJ_VAL_STRING, 20, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               10, IterationReportField::IRF_SCIENTIFIC));
        break;
    }

    default:
        break;
    }

    return result;
}

Entry PrimalSimplex::getIterationEntry(const string &name, ITERATION_REPORT_FIELD_TYPE &type) const {
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
            if(m_incomingIndex != -1){
                reply.m_double = m_primalReducedCost;
            } else {
                reply.m_double = 0;
            }
        } else if (name == PRIMAL_THETA_STRING) {
            reply.m_double = m_primalTheta;
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

void PrimalSimplex::initModules() {
    Simplex::initModules();

    // TODO: ezt majd egy switch-case donti el, amit lehetne
    // kulon fuggvenybe is tenni akar
    PrimalPricingFactory * pricingFactory = new PrimalDantzigPricingFactory;

    m_updater = new PrimalUpdater;

    PrimalPricingUpdater * pricingUpdater = pricingFactory->createPrimalPricingUpdater(
                m_basicVariableValues,
                m_basicVariableFeasibilities,
                &m_reducedCostFeasibilities,
                m_basisHead,
                *m_simplexModel,
                *m_basis
                );
    m_updater->setPricingUpdater( pricingUpdater );

    m_pricing = pricingFactory->createPrimalPricing(*m_simplexModel,
                                                    pricingUpdater,
                                                    m_basisHead);

    m_feasibilityChecker = new PrimalFeasibilityChecker(*m_simplexModel,
                                                        &m_variableStates,
                                                        &m_basicVariableFeasibilities,
                                                        &m_phaseIObjectiveValue);

    PrimalRatiotestUpdater * ratiotestUpdater = new PrimalRatiotestUpdater(&m_reducedCostFeasibilities);
    m_updater->setRatiotestUpdater( ratiotestUpdater );

    //Todo: Add the updater to the ratiotest
    m_ratiotest = new PrimalRatiotest(*m_simplexModel,
                                      m_basicVariableValues,
                                      m_basisHead,
                                      m_basicVariableFeasibilities,
                                      m_variableStates,
                                      m_reducedCosts);

    delete pricingFactory;
    pricingFactory = 0;
}

void PrimalSimplex::releaseModules() {
    Simplex::releaseModules();

    if (m_feasibilityChecker){
        delete m_feasibilityChecker;
        m_feasibilityChecker = 0;
    }
    if (m_pricing) {
        delete m_pricing;
        m_pricing = 0;
    }
    if (m_ratiotest) {
        delete m_ratiotest;
        m_ratiotest = 0;
    }
    if (m_updater) {
        delete m_updater;
        m_updater = 0;
    }
}

void PrimalSimplex::computeFeasibility() {
    m_feasibilityChecker->computeFeasibilities();
}

void PrimalSimplex::checkFeasibility() {

}

void PrimalSimplex::price() {

}

void PrimalSimplex::selectPivot() {

}

void PrimalSimplex::update() {
    //Call this with appropriate parameters:
//    void Simplex::transform(unsigned int incomingIndex,
//      unsigned int outgoingIndex,
//      const std::vector<unsigned int>& boundflips,
//      Numerical::Double reducedCost) {

    //Do primal specific using the updater
    m_updater->update(m_feasible ? 2 : 1);
}

void PrimalSimplex::setReferenceObjective() {

}

void PrimalSimplex::checkReferenceObjective() {

}

void PrimalSimplex::initWorkingTolerance() {

}

void PrimalSimplex::computeWorkingTolerance() {

}

void PrimalSimplex::releaseLocks() {

}
