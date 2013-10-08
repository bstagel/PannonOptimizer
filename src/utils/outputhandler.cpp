/**
 * @file outputhandler.cpp
 */

#include <utils/outputhandler.h>
#include <iostream>
#include <cstdlib>

/*********************************HELPER****************************************************************/
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

void MessageHandler::addMessage(const std::string & message) {
    if (getenv("ECLIPSE")) {
        std::cerr<<"[INFO   ]";
    } else {
        std::cerr<<(DC_EMB DC_BGB "[" DC_EMW "INFO   " DC_EMB "]" DC_D );
    }
    std::cerr << message << std::endl;
}

void WarningHandler::addMessage(const std::string & message) {
    if (getenv("ECLIPSE")) {
        std::cerr<<"[WARNING   ]";
    } else {
        std::cerr<<(DC_EMY DC_BGY "[" DC_EMW "WARNING" DC_EMY "]" DC_D);
    }
    std::cerr << message << std::endl;
}

void ErrorHandler::addMessage(const std::string & message) {
    if (getenv("ECLIPSE")) {
        std::cerr<<"[ERROR   ]";
    } else {
        std::cerr<<(DC_EMR DC_BGR "[" DC_EMW "ERROR  " DC_EMR "]" DC_D );
    }
    std::cerr << message << std::endl;
}

void DebugHandler::addMessage(const std::string & message) {
    if (getenv("ECLIPSE")) {
        std::cerr<<"[ERROR   ]";
    } else {
        std::cerr<<(DC_EMM DC_BGM "[" DC_EMW "DEBUG " DC_EMM "]" DC_EMW );
    }
    std::cerr << message << std::endl;
}

OutputHandler & OutputHandler::getInstance() {
    static OutputHandler handler;
    return handler;
}

OutputHandler::OutputHandler():
    m_messageHandler( new MessageHandler ),
    m_warningHandler( new WarningHandler ),
    m_errorHandler( new ErrorHandler ),
    m_debugHandler( new DebugHandler )
{

}

OutputHandler::~OutputHandler() {
    delete m_messageHandler;
    m_messageHandler = 0;
    delete m_warningHandler;
    m_warningHandler = 0;
    delete m_errorHandler;
    m_errorHandler = 0;
    delete m_debugHandler;
    m_debugHandler = 0;
}

void OutputHandler::setMessageHandler(GeneralMessageHandler * handler) {
    delete m_messageHandler;
    m_messageHandler = handler;
}

void OutputHandler::setWarningHandler(GeneralMessageHandler * handler) {
    delete m_warningHandler;
    m_warningHandler = handler;
}

void OutputHandler::setErrorHandler(GeneralMessageHandler * handler) {
    delete m_errorHandler;
    m_errorHandler = handler;
}

void OutputHandler::setDebugHandler(GeneralMessageHandler * handler) {
    delete m_debugHandler;
    m_debugHandler = handler;
}

void OutputHandler::addMessage(const std::ostringstream & stream) {
    if ( m_messageHandler != 0 ) {
        m_messageHandler->addMessage( stream.str() );
    }
}

void OutputHandler::addWarning(const std::ostringstream & stream) {
    if ( m_warningHandler != 0 ) {
        m_warningHandler->addMessage( stream.str() );
    }
}

void OutputHandler::addError(const std::ostringstream & stream) {
    if ( m_errorHandler != 0 ) {
        m_errorHandler->addMessage( stream.str() );
    }
}

void OutputHandler::addDebug(const std::ostringstream & stream) {
    if ( m_debugHandler != 0 ) {
        m_debugHandler->addMessage( stream.str() );
    }
}

void OutputHandler::setDefaultMessageHandler() {
    delete m_messageHandler;
    m_messageHandler = new MessageHandler;
}

void OutputHandler::setDefaultWarningHandler() {
    delete m_warningHandler;
    m_warningHandler = new WarningHandler;
}

void OutputHandler::setDefaultErrorHandler() {
    delete m_errorHandler;
    m_errorHandler = new ErrorHandler;
}

void OutputHandler::setDefaultDebugHandler() {
    delete m_debugHandler;
    m_debugHandler = new DebugHandler;
}



