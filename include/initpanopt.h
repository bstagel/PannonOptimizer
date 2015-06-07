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

#ifndef INITPANOPT_H
#define INITPANOPT_H

#include <utils/architectureinterface.h>

//class ArchitectureInterface;

class InitPanOpt {
public:
    static InitPanOpt & getInstance();

    static void init();

    static void threadInit();

    static void release();

    static void threadRelease();

    const ArchitectureInterface & getArchitecture() const;
private:
    static thread_local ArchitectureInterface * sm_architecture;

    static thread_local InitPanOpt * sm_instance;

    InitPanOpt();
    InitPanOpt(const InitPanOpt & orig);
};

__attribute__((constructor))
void initPanOpt();

#endif // INITPANOPT_H
