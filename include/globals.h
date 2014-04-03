/**
 * @file globals.h
 */

#ifndef GLOBALS_H
#define GLOBALS_H

//using namespace std;
#include <utils/platform.h>
#include <utils/thread.h>
#include <utils/memoryman.h>
#include <climits>
#include <float.h>

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define STATIC_ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

#define __UNUSED(x) (void)x;

#define INVALID_POSITION INT_MIN
#define SPARSE 1
#define DENSE 0

#if !(__cplusplus > 199711L)
  #error ERROR: PLEASE COMPILE WITH C++0x11!
#endif

#define CONCAT(a, b) a ## b

#define STL_FOREACH(_CONTAINER_TYPE, _CONTAINER, _ITERATOR) \
    _CONTAINER_TYPE::const_iterator _ITERATOR = _CONTAINER.begin(); \
    _CONTAINER_TYPE::const_iterator CONCAT(_ITERATOR, End) = _CONTAINER.end(); \
    for (; _ITERATOR != CONCAT(_ITERATOR, End); _ITERATOR++)

#define STL_FOREACH_MODIFY(_CONTAINER_TYPE, _CONTAINER, _ITERATOR) \
    _CONTAINER_TYPE::iterator _ITERATOR = _CONTAINER.begin(); \
    _CONTAINER_TYPE::iterator CONCAT(_ITERATOR, End) = _CONTAINER.end(); \
    for (; _ITERATOR != CONCAT(_ITERATOR, End); _ITERATOR++)

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

#endif // GLOBALS_H
