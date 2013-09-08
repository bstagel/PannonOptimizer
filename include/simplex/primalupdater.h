/* 
 * File:   primalupdater.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 0:54
 */

#ifndef PRIMALUPDATER_H
#define	PRIMALUPDATER_H

#include <simplex/primalreducedcostupdater.h>

class PrimalUpdater {
public:
    PrimalUpdater();

    void setReducedCostUpdater(PrimalReducedCostUpdater * reducedCostUpdater);

    void update();

protected:
    PrimalReducedCostUpdater * m_reducedCostUpdater;
};

#endif	/* PRIMALUPDATER_H */

