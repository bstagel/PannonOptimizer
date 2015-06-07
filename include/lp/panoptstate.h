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

#ifndef PANOPTSTATE_H
#define PANOPTSTATE_H

#include <globals.h>
#include <lp/model.h>

class PanOptState {
public:
    virtual void startWrting(const char * fileName,
                             const Model & model) = 0;

    virtual void finishWriting() = 0;

    virtual void addInfo(const char * info) = 0;

    virtual void startReading(const char * fileName,
                              const Model & model) = 0;

    virtual void finishReading() = 0;
};

#endif // PANOPTSTATE_H
