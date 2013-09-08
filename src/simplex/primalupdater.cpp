#include <simplex/primalupdater.h>

PrimalUpdater::PrimalUpdater():
    m_reducedCostUpdater(0)
{

}

void PrimalUpdater::setReducedCostUpdater(PrimalReducedCostUpdater * reducedCostUpdater) {
    m_reducedCostUpdater = reducedCostUpdater;
}

void PrimalUpdater::update() {

}
