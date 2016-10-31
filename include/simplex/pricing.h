//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

/**
 * @file pricing.h This file contains the Pricing abstract class.
 */
#ifndef PRICING_H
#define PRICING_H

#include <globals.h>

/**
 * Describes an abstract pricing module.
 * It is necessary for a transparent high level pricing module.
 *
 * @class Pricing
 */
class Pricing
{
public:

    /**
     * Initializes the pricing module.
     */
    virtual void init() = 0;

    /**
     * Returns the number of alternate candidates choosen when combined_objective param is enabled.
     * @return m_alternateCandidateCounter
     */
    unsigned getAlternateCandidateCounter()const {return m_alternateCandidateCounter;}
protected:

    /**
     * Counter of alternate candidates when combined_objective param is enabled.
     */
    unsigned m_alternateCandidateCounter;
};

#endif // PRICING_H
