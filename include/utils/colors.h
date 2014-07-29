/**
 * @file colors.h This file contains macros defining the output colors.
 */

#ifndef COLORS_H
#define COLORS_H

#include <globals.h>


/*
 * Use COLORFLAGS macro to define the colors of the output.
 * To use bash compatible color flags: define COLORFLAGS to BASHCOLOR
 * To use windows color flags: define COLORFLAGS to WINDOWSCOLOR
 *  !!! In that case the default windows headers are needed !!!
 * For no colors at all: define COLORFLAGS to NOCOLOR
 *
 * !!! Note: if ECLIPSE environment variable is defined at run time, then
 *  it will result no color output independently from COLORFLAGS macro !!!
 *
 */

#define NOCOLOR             0
#define COLORFLAGS NOCOLOR

#ifdef UNIX
#define BASHCOLOR       1
#undef NOCOLOR
#undef COLORFLAGS
#define COLORFLAGS BASHCOLOR
#endif

/*
 * The WINDOWSCOLOR macro is available only on _WIN32 platforms.
 */
#ifdef WIN32
#define WINDOWSCOLOR    2
#undef NOCOLOR
#undef COLORFLAGS
#define COLORFLAGS WINDOWSCOLOR
#endif

#if COLORFLAGS == WINDOWSCOLOR
#include <windows.h>
static const HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
#endif

// Define colors
#if COLORFLAGS == BASHCOLOR

#define DC_K "\e[0;30m"
#define DC_R "\e[0;31m"
#define DC_G "\e[0;32m"
#define DC_Y "\e[0;33m"
#define DC_B "\e[0;34m"
#define DC_M "\e[0;35m"
#define DC_C "\e[0;36m"
#define DC_W "\e[0;37m"
#define DC_D "\e[0m"

#define DC_BGK "\e[40m"
#define DC_BGR "\e[41m"
#define DC_BGG "\e[42m"
#define DC_BGY "\e[43m"
#define DC_BGB "\e[44m"
#define DC_BGM "\e[45m"
#define DC_BGC "\e[46m"
#define DC_BGW "\e[47m"

#define DC_EMK "\e[1;30m"
#define DC_EMR "\e[1;31m"
#define DC_EMG "\e[1;32m"
#define DC_EMY "\e[1;33m"
#define DC_EMB "\e[1;34m"
#define DC_EMM "\e[1;35m"
#define DC_EMC "\e[1;36m"
#define DC_EMW "\e[1;37m"

#else
#include <cstdlib>
#define DC_K ""
#define DC_R ""
#define DC_G ""
#define DC_Y ""
#define DC_B ""
#define DC_M ""
#define DC_C ""
#define DC_W ""
#define DC_D ""

#define DC_BGK ""
#define DC_BGR ""
#define DC_BGG ""
#define DC_BGY ""
#define DC_BGB ""
#define DC_BGM ""
#define DC_BGC ""
#define DC_BGW ""

#define DC_EMK ""
#define DC_EMR ""
#define DC_EMG ""
#define DC_EMY ""
#define DC_EMB ""
#define DC_EMM ""
#define DC_EMC ""
#define DC_EMW ""

#endif

#endif // COLORS_H
