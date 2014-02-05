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
    PrimalRatiotestUpdater(IndexList<>* variableFeasibilities);
    void updateFeasibilities();

private:
    IndexList<> * m_variableFeasibilities;

    std::vector<int> m_updateVector;
};

#endif // PRIMALRATIOTESTUPDATER_H
