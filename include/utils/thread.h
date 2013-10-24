#ifndef THREAD_H
#define THREAD_H

#include <utils/platform.h>
#include <thread>

#ifdef __GNUC__

#define THREAD_STATIC_DECL static __thread
#define THREAD_STATIC_DEF  __thread

#endif

#ifdef __INTEL_COMPILER

#ifdef THREAD_STATIC_DECL
#undef THREAD_STATIC_DECL
#undef THREAD_STATIC_DEF
#endif

#ifdef UNIX

#define THREAD_STATIC_DECL static __thread
#define THREAD_STATIC_DEF  __thread

#endif
#ifdef WIN32

#define THREAD_STATIC_DECL static __declspec(thread)
#define THREAD_STATIC_DEF  __declspec(thread)

#endif

#endif

// TODO: majd kesobb meg kiegeszitjuk a tobbi platformhoz es forditohoz
// Segitseg: http://sourceforge.net/p/predef/wiki/Compilers/

#ifndef THREAD_STATIC_DECL
    #error Cannot use thread local variables.
#endif

#endif // THREAD_H
