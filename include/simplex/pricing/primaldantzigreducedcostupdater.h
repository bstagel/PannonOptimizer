/* 
 * File:   primaldantzigreducedcostupdater.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 0:59
 */

#ifndef PRIMALDANTZIGREDUCEDCOSTUPDATER_H
#define	PRIMALDANTZIGREDUCEDCOSTUPDATER_H

#include <globals.h>

#include <simplex/primalreducedcostupdater.h>

class PrimalDantzigReducedCostUpdater: public PrimalReducedCostUpdater
{
public:
    PrimalDantzigReducedCostUpdater();
    PrimalDantzigReducedCostUpdater(const PrimalDantzigReducedCostUpdater& orig);
    virtual ~PrimalDantzigReducedCostUpdater();
private:

};

#endif	/* PRIMALDANTZIGREDUCEDCOSTUPDATER_H */

