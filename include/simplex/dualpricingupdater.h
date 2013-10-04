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
#include <vector>

class DualPricingUpdater {
public:
    DualPricingUpdater(const Vector & basicVariableValues,
                       IndexList<> variableFeasibilities,
                       std::vector<int> basisHead);
    virtual ~DualPricingUpdater();
public:

    const Vector & m_basicVariableValues;

    const IndexList<> & m_variableFeasibilities;

    const std::vector<int> & m_basisHead;
};

#endif	/* DUALPRICINGUPDATER_H */

