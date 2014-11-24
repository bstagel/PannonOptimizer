/**
 * @file debug.h
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <globals.h>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <map>

#include <ctime>
#include <cstring>
#include <cstdlib>

#include <utils/colors.h>
#include <utils/outputhandler.h>

/*
 * To use development messages with module names, timestamps, file names, line numbers,
 *  function names and block formatted output.
 * If DEVELOPMENT macro is defined, the messages will be generated and shown according
 *  to the active modules. Modules can be set at D class. (See below.)
 * If DEVELOPMENT macro is not defined, the messages will not be generated at all.
 */

#define DEVELOPMENT

/***********************************************************************************************/
/**************************************** END USER MESSAGES ************************************/
/***********************************************************************************************/

/*
 *
 * This messages are always shown for the users.
 * You can use LPINFO, LPWARNING or LPERROR.
 * !!!LPDEBUG has been turned off!!!
 * If there are any debug messages use LPDEBUG for them. This function is only called if
 *  the NDEBUG macro was not defined. This feature can be used if you have something more detailed
 *  information which does not fit to LPINFO/LPWARNING/LPERROR, but can be useful some cases.
 *  (Like something went wrong and the user would like to send bug reports, and so on.)
 *
 * Usage:
 *  LPINFO("Some " << var1 << " = " << var2);
 *  LPWARNING("Some " << var1 << " = " << var2);
 *  LPERROR("Some " << var1 << " = " << var2);
 *  LPDEBUG("Some " << var1 << " = " << var2);
 *
 * Output:
 *  [LPINFO   ] Some 7 = 8
 *  [LPWARNING] Some 7 = 8
 *  [LPERROR  ] Some 7 = 8
 *  [file.cpp : 12345 ] Some 7 = 8
 *
 *  Default behaviour is to use different colors different log levels,
 *   but you can disable color flags defining ECLIPSE environment variable.
 *
 *
 */

/*********************************HELPER****************************************************************/

#define LPTEST(msg)

#define LPREPORT(msg, ss) { std::ostringstream str; \
    str << msg; \
    OutputHandler::getInstance().addReport(str, ss); \
    }

#define LPINFO(msg) { std::ostringstream str; \
    str << msg; \
    OutputHandler::getInstance().addMessage(str); \
    }

#define LPWARNING(msg) { std::ostringstream str; \
    str << msg; \
    OutputHandler::getInstance().addWarning(str); \
    }
#define LPERROR(msg) { std::ostringstream str; \
    str << msg; \
    OutputHandler::getInstance().addError(str); \
    }

/***********************************************************************************************/
/************************************* DEVELOPMENT MESSAGES ************************************/
/***********************************************************************************************/

/*
 *
 * This messages are shown if the DEVELOPMENT macro is defined.
 * You can use DEVINFO, DEVWARNING or DEVERROR.
 * If there are any debug messages use DEVDEBUG for them. This function is only called if
 *  the NDEBUG macro was not defined. This feature can be used if you have something more detailed
 *  information which does not fit to DEVINFO/DEVWARNING/DEVERROR, but can be useful some cases.
 *  (Like something went wrong and the user would like to send bug reports, and so on.)
 *
 * Usage:
 *  DEVINFO(D::STARTINGBASISFINDER, "Some " << var1 << " = " << var2);
 *  DEVWARNING(D::PRESOLVER, "Some " << var1 << " = " << var2);
 *  DEVERROR(D::PFIMAKER, "Some " << var1 << " = " << var2);
 *  DEVDEBUG(D::PFIMAKER, "Some " << var1 << " = " << var2);
 *
 * Output:
 * [I StartingBasisFi  | 000.0000 |  main.cpp:0113: main()                  ] Some 7 = 8
 * [W Presolver        | 000.0000 |  main.cpp:0114: main()                  ] Some 7 = 8
 * [E PFIMaker         | 000.0000 |  main.cpp:0115: main()                  ] Some 7 = 8
 * [D PFIMaker         | 000.0000 |  main.cpp:0116: main()                  ] Some 7 = 8
 *
 *  Default behaviour is to use different colors different log levels,
 *   but you can disable color flags defining ECLIPSE environment variable.
 *
 *
 */

/*
 * Class for enumerating modules.
 * RegisteredModuleType enumeration contains all known modules in the program.
 *  If you would like to add a custom module, just expand RegisteredModuleType enum and
 *   put it to the map containing strings to each module types with fillInModuleTypes function.
 * ActiveModules contains the modules, whom messages will be shown.
 *  You should modify fillInActiveModules to filter the output.
 *  Just comment the unnecessary modules in fillInActiveModules function
 *   and add the necessaries.
 */
class D
{
    friend class InitPanOpt;
public:

    enum RegisteredModuleType
    {
        MPSREADER = 1 << 0,
        PRESOLVER = 1 << 1,
        CFMAKER = 1 << 2,
        STARTINGBASISFINDER = 1 << 3,
        PFIMAKER = 1 << 4,
        SIMPLEX = 1 << 5,
        RATIOTEST = 1 << 6,
        FEASIBILITYCHECK = 1 << 7,
        MODEL = 1 << 8,
        PRICING = 1 << 9,
        SBF_SUPER = 1 << 10,
        SBF_LOGICAL = 1 << 11,
        SBF_SYMBO = 1 << 12,
        SBF_LTSF = 1 << 13,
        SBF_ADG = 1 << 14,
        SBF_CPLEX = 1 << 15
    };

    static std::string getName(RegisteredModuleType mod);

    static int getActiveModules();

    static void disableModule(RegisteredModuleType mod);

    static void enableModule(RegisteredModuleType mod);

private:

    static std::map<RegisteredModuleType, std::string> fillInModuleTypes();

    static int fillInActiveModules();

    static void init();

    static void release();

    static std::map<RegisteredModuleType, std::string> * m_registeredModules;
    static int m_activeModules;
};

#ifdef DEVELOPMENT

/*
 *  Headers of printed messages will be minimum this long
 */
#define DEBUG_JUSTIFY_TO 50

/*
 * Amount of whitespace necessary to properly justify from length n, as string
 */
std::string _debug_justify_(int);

/*
 * Strlen of a string ignoring color codes
 */
unsigned int _debug_strlen_(std::string);

/*
 * Take only the filename without the path
 */
const std::string _debug_file_name_(const std::string);

#define DEBUG_FILE _debug_file_name_(__FILE__)

/*
 * Timestamp, as string
 */
#define DEBUG_TIME (Debug::time())

/*
 *  Current identation level, as string
 */
#define DEBUG_INDENT (Debug::indentation())

/*
 * Put DEBUG_BLOCK at the top of a function to log debug output whenever
 *  the function/block is entered or exited, and indent everything in between
 */
#define DEBUG_BLOCK DEBUG_BLOCK_X(__PRETTY_FUNCTION__)

/*
 * Same as DEBUG_BLOCK, but log a custom message instead of the function name
 */
#define DEBUG_BLOCK_X(msg) \
    Debug::Block debug_block_ = \
        Debug::block("B ", DEBUG_FILE, __LINE__, __FUNCTION__, msg);

/*
 * Writes the given content inside a standard debug header,
 *  with timestamp, alignment, and indentation
 */
#define DEBUG_HEADER(level, content, white, def_color, bg_color, sep_color)           \
    std::cerr << sep_color bg_color "[" white                      \
              << level << sep_color " | "  white << DEBUG_TIME << sep_color " | " white << " " << (content)            \
              << _debug_justify_( _debug_strlen_(content) + 10 )    \
              << sep_color "]" def_color << DEBUG_INDENT  << " "


/*************************
 * Standard message types
 *************************/

/*
 * Helper class to handle block formatted output.
 */
class Debug
{
public:
    static void init();

    /*
     * Time since init(), in seconds format
     */
    static std::string time();

    /*
     * Current indentation level as spaces
     */
    static std::string indentation();

    /*
     * Logs text at Block construction and destruction (end of scope),
     * indents in between
     */
    class Block;
    static Block block(const std::string level, const std::string file, int line,
        const std::string func, const std::string text);

private:
    class Private;
    static clock_t cl_start;
    static clock_t cl_end;
};

/*
 * Helper class to handle one block for formatter output.
 */
class Debug::Block
{
public:
    ~Block();
private:
    Block(const std::string level, const std::string file, int line, const std::string func, const std::string text);
    void operator=(const Block&);

    const std::string m_level, m_file, m_func, m_text;
    int m_line;

    friend Block Debug::block(const std::string level, const std::string file, int line,
        const std::string func, const std::string text);
};


/*
 * Prints a header with the given file, line, and function names
 */
#define DEBUG_LONG_HEADER(level, file, line, func, white, def_color, bg_color, sep_color, msg)        \
    {   std::string _fu = func;                                                      \
        std::ostringstream _l; _l << std::setfill('0') << std::setw(4) << line;       \
        std::ostringstream o; o << file << sep_color  << ":" << white << _l.str()   \
            << sep_color << ":" << white " " << _fu << "()";                        \
        _fu = o.str();                                                               \
        DEBUG_HEADER(level, _fu, white, def_color, bg_color, sep_color) << msg << std::endl;      }


/*
 * Prints a standard debug message calling long header maker.
 */
#define DEBUG_DEBUG(mod, msg) \
    DEBUG_LONG_HEADER("D " + mod + " ", DEBUG_FILE, __LINE__, __FUNCTION__, DC_EMW, DC_D, DC_BGM, DC_EMM, msg)

#define DEBUG_INFO(mod, msg) \
    DEBUG_LONG_HEADER("I " + mod + " ", DEBUG_FILE, __LINE__, __FUNCTION__, DC_EMW, DC_D, DC_BGB, DC_EMB, msg)

#define DEBUG_WARNING(mod, msg) \
    DEBUG_LONG_HEADER("W " + mod + " ", DEBUG_FILE, __LINE__, __FUNCTION__, DC_EMW, DC_D, DC_BGY, DC_EMY, msg)

#define DEBUG_ERROR(mod, msg) \
    DEBUG_LONG_HEADER("E " + mod + " ", DEBUG_FILE, __LINE__, __FUNCTION__, DC_EMW, DC_D, DC_BGR, DC_EMR, msg)

/*
 * No colors
 */
#define DEBUG_NCDEBUG(mod, msg) \
    DEBUG_LONG_HEADER("D " + mod + " ", DEBUG_FILE, __LINE__, __FUNCTION__, "", "", "", "", msg)

#define DEBUG_NCINFO(mod, msg) \
    DEBUG_LONG_HEADER("I " + mod + " ", DEBUG_FILE, __LINE__, __FUNCTION__, "", "", "", "", msg)

#define DEBUG_NCWARNING(mod, msg) \
    DEBUG_LONG_HEADER("W " + mod + " ", DEBUG_FILE, __LINE__, __FUNCTION__, "", "", "", "", msg)

#define DEBUG_NCERROR(mod, msg) \
    DEBUG_LONG_HEADER("E " + mod + " ", DEBUG_FILE, __LINE__, __FUNCTION__, "", "", "", "", msg)



/***********************************
 * Use these macros in your code!  *
 ***********************************/
#if COLORFLAGS == WINDOWSCOLOR
#define DEVINFO(mod, msg)                               \
    if (D::getActiveModules()&mod) {                    \
            DEBUG_NCINFO(D::getName(mod), msg);         \
    }
#define DEVWARNING(mod, msg)                            \
    if (D::getActiveModules()&mod) {                    \
            DEBUG_NCWARNING(D::getName(mod), msg);      \
    }
#define DEVERROR(mod, msg)                              \
    if (D::getActiveModules()&mod) {                    \
            DEBUG_NCERROR(D::getName(mod), msg);        \
    }
#define DEVDEBUG(mod, msg)                              \
    if (D::getActiveModules()&mod) {                    \
            DEBUG_NCDEBUG(D::getName(mod), msg);        \
    }

#else
#define DEVINFO(mod, msg)                               \
    if (D::getActiveModules()&mod) {                    \
        if (getenv("ECLIPSE")) {                        \
            DEBUG_NCINFO(D::getName(mod), msg);         \
    } else {                                            \
            DEBUG_INFO(D::getName(mod), msg);           \
        }                                               \
    }

#define DEVWARNING(mod, msg)                            \
    if (D::getActiveModules()&mod) {                    \
        if (getenv("ECLIPSE")) {                        \
            DEBUG_NCWARNING(D::getName(mod), msg);      \
        } else {                                        \
            DEBUG_WARNING(D::getName(mod), msg);        \
        }                                               \
    }

#define DEVERROR(mod, msg)                              \
    if (D::getActiveModules()&mod) {                    \
        if (getenv("ECLIPSE")) {                        \
            DEBUG_NCERROR(D::getName(mod), msg);        \
        } else {                                        \
            DEBUG_ERROR(D::getName(mod), msg);          \
        }                                               \
    }

#define DEVDEBUG(mod, msg)                              \
    if (D::getActiveModules()&mod) {                    \
        if (getenv("ECLIPSE")) {                        \
            DEBUG_NCDEBUG(D::getName(mod), msg);        \
        } else {                                        \
            DEBUG_DEBUG(D::getName(mod), msg);          \
        }                                               \
    }

#endif

#define UNIMPLEMENTED LPDEBUG("AUTOGENERATED: Not yet implemented: " << __FUNCTION__);

#else

#define DEVINFO(mod, msg)       { }
#define DEVWARNING(mod, msg)    { }
#define DEVERROR(mod, msg)      { }
#define DEVDEBUG(mod, msg)      { }

#define UNIMPLEMENTED { }

#endif // DEVELOPMENT



#endif /* DEBUG_H_ */
