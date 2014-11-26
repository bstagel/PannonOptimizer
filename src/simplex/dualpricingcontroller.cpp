/**
 * @file dualpricingcontroller.cpp
 */
#include <simplex/dualpricingcontroller.h>
#include <thread>

#include <cstdlib>


DualPricingController::DualPricingController(const DenseVector &basicVariableValues,
                                             IndexList<> *basicVariableFeasibilities,
                                             const IndexList<> &reducedCostFeasibilities,
                                             const std::vector<int> &basisHead,
                                             const SimplexModel &simplexModel,
                                             const Basis &basis)
{
    m_dantzig = new DualDantzigPricing(basicVariableValues,
                                       basicVariableFeasibilities,
                                       reducedCostFeasibilities,
                                       basisHead,
                                       simplexModel,
                                       basis);

    m_devex = new DualDevexPricing(basicVariableValues,
                                   basicVariableFeasibilities,
                                   reducedCostFeasibilities,
                                   basisHead,
                                   simplexModel,
                                   basis);

}

DualPricingController::~DualPricingController()
{
    delete m_dantzig;

    delete m_devex;
}

static void performPhase1(DualPricing * pricing, int * result) {
    *result = pricing->performPricingPhase1();
}

static void performPhase2(DualPricing * pricing, int * result) {
    *result = pricing->performPricingPhase2();
}

int DualPricingController::performPricingPhase1()
{
    int result1, result2;
    std::thread t1 = std::thread(performPhase1, m_dantzig, &result1);
    std::thread t2 = std::thread(performPhase1, m_devex, &result2);

    t1.join();
    t2.join();

    if (rand() % 2 == 0) {
        m_reducedCost = m_dantzig->getReducedCost();
        return result1;
    }
    m_reducedCost = m_devex->getReducedCost();
    return result2;
}

int DualPricingController::performPricingPhase2()
{
    int result1, result2;
    std::thread t1 = std::thread(performPhase2, m_dantzig, &result1);
    std::thread t2 = std::thread(performPhase2, m_devex, &result2);

    t1.join();
    t2.join();

    if (rand() % 2 == 0) {
        m_reducedCost = m_dantzig->getReducedCost();
        return result1;
    }
    m_reducedCost = m_devex->getReducedCost();
    return result2;
}

static void update(DualPricing * pricing, int incomingIndex,
                   int outgoingIndex,
                   const DenseVector &incomingAlpha,
                   const DenseVector &pivotRow,
                   const DenseVector &pivotRowOfBasisInverse) {
    pricing->update(incomingIndex, outgoingIndex, incomingAlpha,
                    pivotRow, pivotRowOfBasisInverse);

}

void DualPricingController::update(int incomingIndex,
                                   int outgoingIndex,
                                   const DenseVector &incomingAlpha,
                                   const DenseVector &pivotRow,
                                   const DenseVector &pivotRowOfBasisInverse)
{
    std::thread t1 = std::thread(::update, m_dantzig, incomingIndex,
                outgoingIndex, incomingAlpha,
                pivotRow, pivotRowOfBasisInverse);

    std::thread t2 = std::thread(::update, m_devex, incomingIndex,
                outgoingIndex, incomingAlpha,
                pivotRow, pivotRowOfBasisInverse);


    t1.join();
    t2.join();
}
