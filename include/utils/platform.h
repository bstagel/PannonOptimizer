/**
 * @file platform.h
 *
 * This file contains macros which identifies the current platform.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

/**
 * @def WIN32
 *
 * This macro is defined if the source is compiled into Windows platforms.
 */

/**
 * @def UNIX
 *
 * This macro is defined if the source is compiled into Unix compatible platforms.
 */

#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#endif

#ifdef __WIN32
#ifndef WIN32
#define WIN32
#endif
#endif

#ifdef __WIN32__
#ifndef WIN32
#define WIN32
#endif
#endif

#ifdef __unix__
#ifndef UNIX
#define UNIX
#endif
#endif

#ifdef __unix
#ifndef UNIX
#define UNIX
#endif
#endif

#ifdef __linux__
#ifndef UNIX
#define UNIX
#endif
#endif

#ifdef __linux
#ifndef UNIX
#define UNIX
#endif
#endif

#endif // PLATFORM_H
