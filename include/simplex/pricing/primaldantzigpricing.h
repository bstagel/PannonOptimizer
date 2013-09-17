/* 
 * File:   primaldantzigpricing.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 0:58
 */

#ifndef PRIMALDANTZIGPRICING_H
#define	PRIMALDANTZIGPRICING_H

#include <globals.h>

#include <simplex/primalpricing.h>

class PrimalDantzigPricing: public PrimalPricing
{
public:
    PrimalDantzigPricing();
    PrimalDantzigPricing(const PrimalDantzigPricing& orig);
    virtual ~PrimalDantzigPricing();
private:

};

#endif	/* PRIMALDANTZIGPRICING_H */

