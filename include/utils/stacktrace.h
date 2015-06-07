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

#ifndef STACKTRACE_H
#define STACKTRACE_H

#include <utils/platform.h>

#ifdef UNIX

#include <iostream>
#include <signal.h>
#include <cstdlib>
#include <execinfo.h>
#include <cxxabi.h>

void printStackTrace( std::ostream & out, unsigned int max_frames = 63 );

void signal_detected(int sig);

void initSignalHandler();

#endif

#endif // STACKTRACE_H
