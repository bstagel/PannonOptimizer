#ifndef DUALRATIOTESTUPDATER_H
#define DUALRATIOTESTUPDATER_H

#include <vector>

#include <utils/indexlist.h>
#include <utils/numerical.h>

//class DualRatiotest;

class DualRatiotestUpdater{
    friend class DualRatiotest;

public:
    DualRatiotestUpdater(IndexList<>* reducedcostFeasibilities);
    void updateFeasibilities();

private:
    IndexList<> * m_reducedcostFeasibilities;

    std::vector<int> m_updateVector;
};

#endif // DUALRATIOTESTUPDATER_H
