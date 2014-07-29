/**
 * @file pricing.h This file contains the Pricing abstract class.
 */
#ifndef PRICING_H
#define PRICING_H

#include <globals.h>

/**
 * Describes an abstract pricing module.
 * It is necessary for a transparent high level pricing module.
 * @class Pricing
 */
class Pricing
{
public:

    /**
     * Initializing function, should be implemented in the derived classes.
     */
    virtual void init() = 0;
};

#endif // PRICING_H
