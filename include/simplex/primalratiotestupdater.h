/**
 * @file primalratiotestupdater.h
 */

#ifndef PRIMALRATIOTESTUPDATER_H
#define PRIMALRATIOTESTUPDATER_H

#include <vector>

#include <utils/indexlist.h>
#include <utils/numerical.h>

class PrimalRatiotestUpdater{
    friend class PrimalRatiotest;

public:
    PrimalRatiotestUpdater(IndexList<>* basicVariableFeasibilities);
    void updateFeasibilities();

private:
    IndexList<> * m_basicVariableFeasibilities;

    std::vector<int> m_updateVector;
};

#endif // PRIMALRATIOTESTUPDATER_H
