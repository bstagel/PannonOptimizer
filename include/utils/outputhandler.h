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

/**
 * @file outputhandler.h This file contains the API of the output message handler classes.
 * @author Jozsef Smidla
 */

#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H

#include <globals.h>
#include <sstream>
#include <mutex>

/**
 * @brief The GeneralMessageHandler class Declares the interface of the concrete message handler classes.
 *
 * @class GeneralMessageHandler
 */
class GeneralMessageHandler {
protected:
    /**
     * When it is true, the output can use colors, otherwise not.
     */
    bool m_colors;
public:

    /**
     * Default constructor. Initializes m_colors to true.
     *
     * @constructor
     */
    GeneralMessageHandler();

    /**
     * Adds a new message to the handler.
     *
     * @param message The message.
     */
    virtual void addMessage( const std::string & message ) = 0;

    /**
     * Destructor of the message handler.
     *
     * @destructor
     */
    virtual ~GeneralMessageHandler() {}

    /**
     * Enables using colors in the output.
     */
    void enableColors();

    /**
     * Disables colors in the output.
     */
    void disableColors();

    /**
     * Returns with true, if the coloring is enabled, otherwise returns with false.
     *
     * @return True if coloring is enabled.
     */
    bool isColored() const;

    /**
     * A getter for the output locking mutex.
     *
     * @return A mutex to provide output locking
     */
    static std::mutex& getMutex(){return sm_outputMutex;}

    static void _globalInit();

protected:

    //Parameter variables
    /**
     * Parameter reference of the run-time parameter "enable_parallelization".
     * Note that this member cannot be a const reference, since it is static.
     *
     * @see SimplexParameterHandler
     */
    static bool sm_enableParallelization;

private:
    /**
     * A mutex to lock the output.
     */
    static std::mutex sm_outputMutex;
};

/**
 * Report handler class for solution reports.
 *
 * @class ReportHandler
 */
class ReportHandler: public GeneralMessageHandler {
public:

    /**
     * Adds a new message to the handler.
     *
     * @param message The message.
     */
    void addMessage( const std::string & message );

    /**
     * Adds a new message to the handler.
     *
     * @param message The message.
     * @param ss The string stream.
     */
    void addMessage(const std::string & message , std::ostringstream & stringstream);

};

/**
 * Message handler class for information messages.
 *
 * @class MessageHandler
 */
class MessageHandler: public GeneralMessageHandler {
public:

    /**
     * Adds a new message to the handler.
     *
     * @param message The message.
     */
    void addMessage( const std::string & message );
};

/**
 * Message handler class for warning messages.
 *
 * @class WarningHandler
 */
class WarningHandler: public GeneralMessageHandler {
public:

    /**
     * Adds a new message to the handler.
     *
     * @param message The message.
     */
    void addMessage( const std::string & message );
};

/**
 * Message handler class for error messages.
 *
 * @class ErrorHandler
 */
class ErrorHandler: public GeneralMessageHandler {
public:

    /**
     * Adds a new message to the handler.
     *
     * @param message The message.
     */
    void addMessage( const std::string & message );
};

/**
 * Message handler class for debug messages.
 *
 * @class MessageHandler
 */
class DebugHandler: public GeneralMessageHandler {
public:

    /**
     * Adds a message to the handler.
     *
     * @param message The message.
     * @param file The name of the source file of the debug message command.
     * @param line The line of the debug message command in the source file.
     */
    void addMessage( const std::string & message,
                     const char * file,
                     unsigned int line);

    /**
     * Adds a new message to the handler.
     *
     * @param message The message.
     */
    void addMessage( const std::string & message );
};

/**
 * This static class can handle all the output messages of the program.
 *
 * @class OutputHandler
 */
class OutputHandler
{
public:

    /**
     * Returns with a reference to the OutputHandler instance.
     *
     * @return A reference to the OutputHandler instance.
     */
    static OutputHandler & getInstance();

    /**
     * Sets the report handler of the program to a given handler object.
     *
     * @param handler The new information message handler.
     */
    void setReportHandler(ReportHandler * handler);

    /**
     * Sets the information message handler of the program to a given handler object.
     *
     * @param handler The new information message handler.
     */
    void setMessageHandler(GeneralMessageHandler * handler);

    /**
     * Sets the warning message handler of the program to a given handler object.
     *
     * @param handler The new warning message handler.
     */
    void setWarningHandler(GeneralMessageHandler * handler);

    /**
     * Sets the error message handler of the program to a given handler object.
     *
     * @param handler The new error message handler.
     */
    void setErrorHandler(GeneralMessageHandler * handler);

    /**
     * Sets the debug message handler of the program to a given handler object.
     *
     * @param handler The new debug message handler.
     */
    void setDebugHandler(GeneralMessageHandler * handler);

    /**
     * Adds a report to the given output with the used report handler.
     *
     * @param stream The output stream.
     */
    void addReport(const std::ostringstream & stream, std::ostringstream & stringstream);

    /**
     * Adds a report to the given output with the used report handler.
     *
     * @param stream The output stream.
     */
    void addReport(const std::ostringstream & stream);

    /**
     * Adds an information message to the given output with the used information message handler.
     *
     * @param stream The output stream.
     */
    void addMessage(const std::ostringstream & stream);

    /**
     * Adds a warning message to the given output with the used warning message handler.
     *
     * @param stream The output stream.
     */
    void addWarning(const std::ostringstream & stream);

    /**
     * Adds an error message to the given output with the used error message handler.
     *
     * @param stream The output stream.
     */
    void addError(const std::ostringstream & stream);

    /**
     * Adds a debug message to the given output with the used debug message handler.
     *
     * @param stream The output stream.
     * @param file The name of the source file of the debug message command.
     * @param line The line of the debug message command in the source file.
     */
    void addDebug(const std::ostringstream & stream,
                  const char * file, unsigned int line);

    /**
     * Sets the report handler of the program to default.
     */
    void setDefaultReportHandler();

    /**
     * Sets the information message handler of the program to default.
     */
    void setDefaultMessageHandler();

    /**
     * Sets the warning message handler of the program to default.
     */
    void setDefaultWarningHandler();

    /**
     * Sets the error message handler of the program to default.
     */
    void setDefaultErrorHandler();

    /**
     * Sets the debug message handler of the program to default.
     */
    void setDefaultDebugHandler();

    /**
     * Enables coloring for all the message handler classes.
     */
    void enableAllColors();

    /**
     * Disables coloring for all the message handler classes.
     */
    void disableAllColors();
private:

    /**
     * Constructor of the OutputHandler class.
     *
     * @constructor
     */
    OutputHandler();

    /**
     * Destructor of the OutputHandler class.
     *
     * @destructor
     */
    ~OutputHandler();

    /**
     * The report handler of the program.
     */
    ReportHandler * m_reportHandler;

    /**
     * The information message handler of the program.
     */
    GeneralMessageHandler * m_messageHandler;

    /**
     * The warning message handler of the program.
     */
    GeneralMessageHandler * m_warningHandler;

    /**
     * The error message handler of the program.
     */
    GeneralMessageHandler * m_errorHandler;

    /**
     * The debug message handler of the program.
     */
    GeneralMessageHandler * m_debugHandler;

    /**
     * When it is true, the output can use colors, otherwise not.
     */
    bool m_colors;
};

#endif // OUTPUTHANDLER_H
