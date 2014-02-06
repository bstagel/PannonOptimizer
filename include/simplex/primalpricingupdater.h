/**
 * @file primalpricingupdater.h
 */

#ifndef PRIMALPRICINGUPDATER_H
#define	PRIMALPRICINGUPDATER_H

#include <globals.h>
#include <linalg/vector.h>
#include <utils/indexlist.h>
#include <simplex/simplexmodel.h>
#include <simplex/basis.h>
#include <vector>

class PrimalPricingUpdater {
public:
    PrimalPricingUpdater(const Vector &basicVariableValues,
                         const IndexList<> & basicVariableFeasibilities,
                         IndexList<> * reducedCostFeasibilities,
                         const std::vector<int> & basisHead,
                         const SimplexModel & simplexModel,
                         const Basis & basis);
    virtual ~PrimalPricingUpdater();

    virtual void updatePhase1() = 0;

    virtual void updatePhase2() = 0;
protected:

    const Vector & m_basicVariableValues;

    const IndexList<> & m_basicVariableFeasibilities;

    IndexList<> * m_reducedCostFeasibilities;

    const std::vector<int> & m_basisHead;

    const SimplexModel & m_simplexModel;

    const Basis & m_basis;
};

#endif	/* PRIMALPRICINGUPDATER_H */

