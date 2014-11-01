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
