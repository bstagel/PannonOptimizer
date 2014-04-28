/**
 * @file dualratiotestupdater.h
 */

#ifndef DUALRATIOTESTUPDATER_H
#define DUALRATIOTESTUPDATER_H

#include <vector>

#include <utils/indexlist.h>
#include <utils/numerical.h>

class DualRatiotestUpdater{
    friend class DualRatiotest;

public:
    DualRatiotestUpdater(IndexList<>* reducedcostFeasibilities);
    void updateFeasibilities();

private:
    IndexList<> * m_reducedcostFeasibilities;

};

#endif // DUALRATIOTESTUPDATER_H
