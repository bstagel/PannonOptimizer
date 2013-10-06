#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H

#include <globals.h>
#include <sstream>

class GeneralMessageHandler {
public:
    virtual void addMessage( const std::string & message ) = 0;
    virtual ~GeneralMessageHandler() {}
};

class MessageHandler: public GeneralMessageHandler {
public:
    void addMessage( const std::string & message );
};

class WarningHandler: public GeneralMessageHandler {
    void addMessage( const std::string & message );
};

class ErrorHandler: public GeneralMessageHandler {
    void addMessage( const std::string & message );
};

class DebugHandler: public GeneralMessageHandler {
    void addMessage( const std::string & message );
};

class OutputHandler
{
public:
    static OutputHandler & getInstance();
    void setMessageHandler(GeneralMessageHandler * handler);
    void setWarningHandler(GeneralMessageHandler * handler);
    void setErrorHandler(GeneralMessageHandler * handler);
    void setDebugHandler(GeneralMessageHandler * handler);
    void addMessage(const std::ostringstream & stream);
    void addWarning(const std::ostringstream & stream);
    void addError(const std::ostringstream & stream);
    void addDebug(const std::ostringstream & stream);
    void setDefaultMessageHandler();
    void setDefaultWarningHandler();
    void setDefaultErrorHandler();
    void setDefaultDebugHandler();
private:
    OutputHandler();
    ~OutputHandler();

    GeneralMessageHandler * m_messageHandler;
    GeneralMessageHandler * m_warningHandler;
    GeneralMessageHandler * m_errorHandler;
    GeneralMessageHandler * m_debugHandler;
};

#endif // OUTPUTHANDLER_H
