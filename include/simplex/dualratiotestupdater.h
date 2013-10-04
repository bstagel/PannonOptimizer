#ifndef DUALRATIOTESTUPDATER_H
#define DUALRATIOTESTUPDATER_H
#include <simplex/dualratiotest.h>
#include <utils/indexlist.h>

class DualRatiotestUpdater{
    IndexList<> * m_reducedcostFeasibilities;
    IndexList<Numerical::Double> * m_variableStates;
public:
    DualRatiotestUpdater(IndexList<>* redcostF,IndexList<Numerical::Double>* varS);
    void updateFeasibilities(const std::vector<int>& updateVector);
};

#endif // DUALRATIOTESTUPDATER_H
