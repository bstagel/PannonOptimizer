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

#ifndef UNKNOWN_H
#define UNKNOWN_H

#include <globals.h>
#include <utils/architecture.h>
#include <cstring>

#define PLATFORM_UNKNOWN


/**
 * This class implements detection of properties of the unknown architectures.
 */
class ArchitectureUnknown: public Architecture {
public:

    /**
     * Default constructor of the ArchitectureUnknown class.
     *
     * @constructor
     */
    ArchitectureUnknown();

    void detect();

    AddVecDenseToDense_NonzCount getAddVecDenseToDense_NonzCount() const;

    AddVecDenseToDense getAddVecDenseToDense() const;

protected:

};


#endif // UNKNOWN_H
