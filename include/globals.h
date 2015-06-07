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
 * @file globals.h
 */

#ifndef GLOBALS_H
#define GLOBALS_H

//using namespace std;
#include <utils/memoryman.h>
#include <utils/platform.h>
#include <initpanopt.h>
#include <utils/thread.h>
#include <climits>
#include <float.h>
#include <macros.h>
#include <double.h>

//#define ANALYSE_DOT_PRODUCT

#if !(__cplusplus > 199711L)
  #error ERROR: PLEASE COMPILE WITH C++0x11!
#endif


#ifdef WIN32
    #define PATH_SEPARATOR '\\'
#else
    #define PATH_SEPARATOR '/'
#endif

enum OBJECTIVE_TYPE
{
    MINIMIZE = 0,
    MAXIMIZE,
    OBJECTIVE_TYPE_ENUM_LENGTH
};

template<typename T, typename F>
struct alias_cast_t
{
    union
    {
        F raw;
        T data;
    };
};

template<typename T, typename F>
T alias_cast(F raw_data)
{
    alias_cast_t<T, F> ac;
    ac.raw = raw_data;
    return ac.data;
}

__attribute__((constructor))
static void __init__PanOpt__() {
    InitPanOpt & init = InitPanOpt::getInstance();
    __UNUSED(init);
}

#endif // GLOBALS_H
