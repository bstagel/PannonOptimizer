/* 
 * File:   dualpricingupdater.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 0:55
 */

#ifndef DUALPRICINGUPDATER_H
#define	DUALPRICINGUPDATER_H

#include <globals.h>
#include <linalg/vector.h>
#include <utils/indexlist.h>
#include <simplex/simplexmodel.h>
#include <vector>

class DualPricingUpdater {
public:
    DualPricingUpdater(const Vector &basicVariableValues,
                       const IndexList<> & variableFeasibilities,
                       const IndexList<> & reducedCostFeasibilities,
                       const std::vector<int> & basisHead,
                       const SimplexModel & simplexModel);
    virtual ~DualPricingUpdater();

    virtual void updatePhase1() = 0;

    virtual void updatePhase2() = 0;
protected:

    const Vector & m_basicVariableValues;

    const IndexList<> & m_variableFeasibilities;

    const IndexList<> & m_reducedCostFeasibilities;

    const std::vector<int> & m_basisHead;

    const SimplexModel & m_simplexModel;
};

#endif	/* DUALPRICINGUPDATER_H */

