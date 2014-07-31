/**
 * @file outputhandler.h This file contains the API of the output message handler classes.
 * @author Jozsef Smidla
 */

#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H

#include <globals.h>
#include <sstream>

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
    THREAD_STATIC_DECL OutputHandler & getInstance();

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
