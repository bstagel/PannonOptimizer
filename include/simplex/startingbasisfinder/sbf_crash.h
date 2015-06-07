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

#ifndef SBF_CRASH_H
#define SBF_CRASH_H

#include <globals.h>

#include <simplex/startingbasisfinder/sbf_super.h>

class SbfCrash : public SbfSuper
{
public:
    SbfCrash(const SimplexModel& model,
               std::vector<int>* basisHead,
               IndexList<const Numerical::Double*>* variableStates,
               StartingBasisFinder::STARTING_NONBASIC_STATES nonbasicStates);

    virtual ~SbfCrash();

    virtual void run();

    double getTime() const {
        return m_time;
    }

    unsigned int getStructuralVariableCount() const {
        return m_structuralVariables;
    }
private:

    double m_time;

    unsigned int m_structuralVariables;
};

#endif // SBF_CRASH_H
