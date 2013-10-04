#ifndef DUALRATIOTESTUPDATER_H
#define DUALRATIOTESTUPDATER_H
#include <simplex/dualratiotest.h>
#include <utils/indexlist.h>

class DualRatiotestUpdater{
    IndexList<> * m_reducedcostFeasibilities;
    IndexList<const Numerical::Double*> * m_variableStates;
public:
    DualRatiotestUpdater(IndexList<>* reducedcostFeasibilities,IndexList<const Numerical::Double*>* variableStates);
    void updateFeasibilities(const std::vector<int>& updateVector);
};

#endif // DUALRATIOTESTUPDATER_H
