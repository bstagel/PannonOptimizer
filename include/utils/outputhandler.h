/**
 * @file outputhandler.h
 */

#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H

#include <globals.h>
#include <sstream>

/**
 * @brief The GeneralMessageHandler class
 *
 * Declares the interface of the concrete message handler classes.
 */
class GeneralMessageHandler {
protected:
    /**
     * @brief m_colors When it is true, the output can use colors, otherwise not.
     */
    bool m_colors;
public:

    /**
     * @brief GeneralMessageHandler Default constructor. Initializes m_colors to true;
     */
    GeneralMessageHandler();

    /**
     * @brief addMessage Adds a new message to the handler.
     * @param message The message.
     */
    virtual void addMessage( const std::string & message ) = 0;

    /**
     * @brief ~GeneralMessageHandler Destructor of the message handler.
     */
    virtual ~GeneralMessageHandler() {}

    /**
     * @brief enableColors Enables using colors in the output.
     */
    void enableColors();

    /**
     * @brief disableColors Disables colors in the output.
     */
    void disableColors();

    /**
     * @brief isColored Returns with true, if the coloring is enabled, otherwise returns with false.
     * @return
     */
    bool isColored() const;
};

/**
 * @brief The MessageHandler class
 */
class MessageHandler: public GeneralMessageHandler {
public:

    /**
     * @brief addMessage
     * @param message
     */
    void addMessage( const std::string & message );
};

/**
 * @brief The WarningHandler class
 */
class WarningHandler: public GeneralMessageHandler {
public:

    /**
     * @brief addMessage
     * @param message
     */
    void addMessage( const std::string & message );
};

/**
 * @brief The ErrorHandler class
 */
class ErrorHandler: public GeneralMessageHandler {
public:

    /**
     * @brief addMessage
     * @param message
     */
    void addMessage( const std::string & message );
};

/**
 * @brief The DebugHandler class
 */
class DebugHandler: public GeneralMessageHandler {
public:

    /**
     * @brief addMessage
     * @param message
     * @param file
     * @param line
     */
    void addMessage( const std::string & message,
                     const char * file,
                     unsigned int line);

    /**
     * @brief addMessage
     * @param message
     */
    void addMessage( const std::string & message );
};

/**
 * @brief The OutputHandler class
 */
class OutputHandler
{
public:

    /**
     * @brief OutputHandler
     * @return
     */
    THREAD_STATIC_DECL OutputHandler & getInstance();

    /**
     * @brief setMessageHandler
     * @param handler
     */
    void setMessageHandler(GeneralMessageHandler * handler);

    /**
     * @brief setWarningHandler
     * @param handler
     */
    void setWarningHandler(GeneralMessageHandler * handler);

    /**
     * @brief setErrorHandler
     * @param handler
     */
    void setErrorHandler(GeneralMessageHandler * handler);

    /**
     * @brief setDebugHandler
     * @param handler
     */
    void setDebugHandler(GeneralMessageHandler * handler);

    /**
     * @brief addMessage
     * @param stream
     */
    void addMessage(const std::ostringstream & stream);

    /**
     * @brief addWarning
     * @param stream
     */
    void addWarning(const std::ostringstream & stream);

    /**
     * @brief addError
     * @param stream
     */
    void addError(const std::ostringstream & stream);

    /**
     * @brief addDebug
     * @param stream
     * @param file
     * @param line
     */
    void addDebug(const std::ostringstream & stream,
                  const char * file, unsigned int line);

    /**
     * @brief setDefaultMessageHandler
     */
    void setDefaultMessageHandler();

    /**
     * @brief setDefaultWarningHandler
     */
    void setDefaultWarningHandler();

    /**
     * @brief setDefaultErrorHandler
     */
    void setDefaultErrorHandler();

    /**
     * @brief setDefaultDebugHandler
     */
    void setDefaultDebugHandler();

    /**
     * @brief enableAllColors
     */
    void enableAllColors();

    /**
     * @brief disableAllColors
     */
    void disableAllColors();
private:
    /**
     * @brief OutputHandler
     */
    OutputHandler();

    /**
     * @brief ~OutputHandler
     */
    ~OutputHandler();

    /**
     * @brief m_messageHandler
     */
    GeneralMessageHandler * m_messageHandler;

    /**
     * @brief m_warningHandler
     */
    GeneralMessageHandler * m_warningHandler;

    /**
     * @brief m_errorHandler
     */
    GeneralMessageHandler * m_errorHandler;

    /**
     * @brief m_debugHandler
     */
    GeneralMessageHandler * m_debugHandler;

    /**
     * @brief m_colors
     */
    bool m_colors;
};

#endif // OUTPUTHANDLER_H
