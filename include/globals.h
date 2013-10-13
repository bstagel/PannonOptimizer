/**
 * @file globals.h
 */

#ifndef GLOBALS_H
#define GLOBALS_H

//using namespace std;
#include <utils/platform.h>
#include <utils/memoryman.h>
#include <climits>
#include <float.h>

#define STATIC_ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

#define __UNUSED(x) (void)x;

#define INVALID_POSITION INT_MIN
#define SPARSE 1
#define DENSE 0

#if !(__cplusplus > 199711L)
  #error ERROR: PLEASE COMPILE WITH C++0x11!
#endif

const double ZERO = 0.0;
const double INVALID = -DBL_MAX;

enum OBJECTIVE_TYPE
{
    MINIMIZE = 0,
    MAXIMIZE,
    OBJECTIVE_TYPE_ENUM_LENGTH
};

#endif // GLOBALS_H
