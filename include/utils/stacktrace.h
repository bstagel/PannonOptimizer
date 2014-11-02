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
