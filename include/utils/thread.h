/**
 * @file thread.h This file contains macros defining the parallel processing settings.
 * @author Jozsef Smidla
 */

#ifndef THREAD_H
#define THREAD_H

#include <utils/platform.h>
#include <thread>

//Instead of this thread_local macro thread_local C++0x specifier is used

//#ifdef GNU_COMPILER
//#define thread_local __thread
//#endif

//#ifdef INTEL_COMPILER

//#ifdef thread_local
//#undef thread_local
//#endif

//#ifdef UNIX

//#define thread_local __thread

//#endif
//#ifdef WIN32

//#define thread_local __declspec(thread)

//#endif

//#endif

// Majd kesobb meg kiegeszitjuk a tobbi platformhoz es forditohoz
// Segitseg: http://sourceforge.net/p/predef/wiki/Compilers/

//#ifndef thread_local
//    #error Cannot use thread local variables.
//#endif

#endif // THREAD_H
