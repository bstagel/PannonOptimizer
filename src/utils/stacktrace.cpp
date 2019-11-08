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

#include <utils/stacktrace.h>
#include <utils/platform.h>

#ifdef UNIX

void printStackTrace( std::ostream & out, unsigned int max_frames )
{
    out << "stack trace:" << std::endl;

    // storage array for stack trace address data
    void* addrlist[max_frames+1];

    // retrieve current stack addresses
    unsigned int addrlen = backtrace( addrlist, sizeof( addrlist ) / sizeof( void* ));

    if ( addrlen == 0 )
    {
        out << std::endl;
        return;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // Actually it will be ## program address function + offset
    // this array must be free()-ed
    char** symbollist = backtrace_symbols( addrlist, addrlen );

    size_t funcnamesize = 1024;
    char funcname[1024];

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.

    for ( unsigned int i = 0; i < addrlen; i++ )
    {

        char* begin_name   = nullptr;
        char* begin_offset = nullptr;
        char* end_offset   = nullptr;

        // find parentheses and +address offset surrounding the mangled name
#ifdef DARWIN
        // OSX style stack trace
        for ( char *p = symbollist[i]; *p; ++p )
        {
            if (( *p == '_' ) && ( *(p-1) == ' ' ))
                begin_name = p-1;
            else if ( *p == '+' )
                begin_offset = p-1;
        }

        if ( begin_name && begin_offset && ( begin_name < begin_offset ))
        {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():
            int status;
            char* ret = abi::__cxa_demangle( begin_name, &funcname[0],
                    &funcnamesize, &status );
            if ( status == 0 )
            {
                funcname = ret; // use possibly realloc()-ed string
                fprintf( out, "  %-30s %-40s %s\n",
                         symbollist[i], funcname, begin_offset );
            } else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                fprintf( out, "  %-30s %-38s() %s\n",
                         symbollist[i], begin_name, begin_offset );
            }

#else // !DARWIN - but is posix
        // not OSX style
        // ./module(function+0x15c) [0x8048a6d]

        for ( char *p = symbollist[i]; *p; ++p )
        {
            if ( *p == '(' )
                begin_name = p;
            else if ( *p == '+' )
                begin_offset = p;
            else if ( *p == ')' && ( begin_offset || begin_name ))
                end_offset = p;
        }

        if ( begin_name && end_offset && ( begin_name && end_offset ))
        {
            *begin_name++   = '\0';
            *end_offset++   = '\0';
            if ( begin_offset )
                *begin_offset++ = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():

            int status = 0;
            char* ret = abi::__cxa_demangle( begin_name, funcname,
                                             &funcnamesize, &status );
            char* fname = begin_name;
            if ( status == 0 )
                fname = ret;

            if ( begin_offset )
            {
                out << symbollist[i] << " ( " << fname << " + " << begin_offset << " ) " << end_offset << std::endl;
            } else {
                out << symbollist[i] << " ( " << fname << " ) " << end_offset << std::endl;
            }
#endif  // !DARWIN - but is posix
        } else {
            // couldn't parse the line? print the whole line.
            out << symbollist[i] << std::endl;
        }
    }

    free(symbollist);
}

void signal_detected(int sig) {

    switch (sig) {
    case SIGINT:
        std::cout << "Interrupt from keyboard" << std::endl;
        break;
    case SIGILL:
        std::cout << "Illegal Instruction" << std::endl;
        break;
    case SIGABRT:
        std::cout << "Abort signal from abort(3)" << std::endl;
        break;
    case SIGFPE:
        std::cout << "Floating point exception" << std::endl;
        break;
        //case SIGKILL: cout<<""<<endl;
    case SIGSEGV:
        std::cout << "Invalid memory reference" << std::endl;
        break;
    case SIGTERM:
        std::cout << "Termination signal" << std::endl;
        break;
#ifdef UNIX
    case SIGHUP:
        std::cout << "Hangup detected on controlling terminal or death of controlling process" << std::endl;
        break;
    case SIGQUIT:
        std::cout << "Quit from keyboard" << std::endl;
        break;
    case SIGPIPE:
        std::cout << "Broken pipe: write to pipe with no readers" << std::endl;
        break;
    case SIGALRM:
        std::cout << "Timer signal from alarm(2)" << std::endl;
        break;
    case SIGBUS:
        std::cout << "Bus error (bad memory access)" << std::endl;
        break;
#endif
    }

    printStackTrace(std::cout);

    exit(sig);
}

#endif

/**
 * Initializes the signals.
 */
void initSignalHandler() {
#ifdef UNIX
    signal(SIGSEGV, signal_detected);
    signal(SIGINT, signal_detected);
    signal(SIGILL, signal_detected);
    signal(SIGABRT, signal_detected);
    signal(SIGFPE, signal_detected);
    signal(SIGTERM, signal_detected);

    signal(SIGHUP, signal_detected);
    signal(SIGQUIT, signal_detected);
    signal(SIGPIPE, signal_detected);
    signal(SIGALRM, signal_detected);
    signal(SIGBUS, signal_detected);
#endif

}

