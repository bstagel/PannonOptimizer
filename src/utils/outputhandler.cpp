/**
 * @file outputhandler.cpp
 */

#include <debug.h>
#include <utils/outputhandler.h>
#include <iostream>
#include <cstdlib>

GeneralMessageHandler::GeneralMessageHandler():
    m_colors(true)
{

}

void GeneralMessageHandler::enableColors() {
    m_colors = true;
}

void GeneralMessageHandler::disableColors() {
    m_colors = false;
}

bool GeneralMessageHandler::isColored() const {
    return m_colors;
}

void ReportHandler::addMessage(const std::string & message) {
    std::cerr << "[REPORT ] " << message << std::endl;
}

void ReportHandler::addMessage(const std::string & message, std::ostringstream & stringstream) {
    stringstream << "[REPORT ] " << message << std::endl;
}

void MessageHandler::addMessage(const std::string & message) {
#if COLORFLAGS != WINDOWSCOLOR
    if (getenv("ECLIPSE") || m_colors == false) {
        std::cerr<<"[INFO   ] ";
    } else {
        std::cerr<<(DC_EMB DC_BGB "[" DC_EMW "INFO   " DC_EMB "]" DC_D );
    }
    std::cerr << message << std::endl;
#else // WINDOWSCOLOR
    if ( getenv("ECLIPSE") || m_colors == false) {
        std::cerr << "[INFO   ] " << message << std::endl;
    } else {
        if(m_colors){
            SetConsoleTextAttribute(hConsole, 25);
            std::cerr << "[";
            SetConsoleTextAttribute(hConsole, 31);
            std::cerr << "INFO   ";
            SetConsoleTextAttribute(hConsole, 25);
            std::cerr << "]";
            SetConsoleTextAttribute(hConsole, 7);
            std::cerr << " " << message << std::endl;
        }
    }
#endif
}

void WarningHandler::addMessage(const std::string & message) {
#if COLORFLAGS != WINDOWSCOLOR
    if (getenv("ECLIPSE") || m_colors == false) {
        std::cerr<<"[WARNING   ] ";
    } else {
        std::cerr<<(DC_EMY DC_BGY "[" DC_EMW "WARNING" DC_EMY "]" DC_D);
    }
    std::cerr << message << std::endl;
#else // WINDOWSCOLOR
    if (getenv("ECLIPSE") || m_colors == false) {
        std::cerr << "[WARNING] " << message << std::endl;
    } else {
        SetConsoleTextAttribute(hConsole, 110);
        std::cerr << "[";
        SetConsoleTextAttribute(hConsole, 111);
        std::cerr << "WARNING";
        SetConsoleTextAttribute(hConsole, 110);
        std::cerr << "]";
        SetConsoleTextAttribute(hConsole, 7);
        std::cerr << " " << message << std::endl;
    }
#endif
}

void ErrorHandler::addMessage(const std::string & message) {
#if COLORFLAGS != WINDOWSCOLOR
    if (getenv("ECLIPSE") || m_colors == false) {
        std::cerr<<"[ERROR   ] ";
    } else {
        std::cerr<<(DC_EMR DC_BGR "[" DC_EMW "ERROR  " DC_EMR "]" DC_D );
    }
    std::cerr << message << std::endl;
#else // WINDOWSCOLOR
    if (getenv("ECLIPSE") || m_colors == false) {
        std::cerr << "[ERROR  ] " << message << std::endl;
    } else {
        SetConsoleTextAttribute(hConsole, 76);
        std::cerr << "[";
        SetConsoleTextAttribute(hConsole, 79);
        std::cerr << "ERROR  ";
        SetConsoleTextAttribute(hConsole, 76);
        std::cerr << "]";
        SetConsoleTextAttribute(hConsole, 7);
        std::cerr << " " << message << std::endl;
    }
#endif
}

void DebugHandler::addMessage(const std::string &message) {
    addMessage(message, "", 0);
}

void DebugHandler::addMessage(const std::string & message,
                              const char *file,
                              unsigned int line) {

#if COLORFLAGS != WINDOWSCOLOR
    __UNUSED(file);
    __UNUSED(line);
    if (getenv("ECLIPSE") || m_colors == false) {
        std::cerr<<"[ERROR   ] ";
    } else {
        std::cerr<<(DC_EMM DC_BGM "[" DC_EMW "DEBUG " DC_EMM "]" DC_EMW );
    }
    std::cerr << message << std::endl;
#else // WINDOWSCOLOR
    if (getenv("ECLIPSE") || m_colors == false) {
        std::cerr << "[" << file << ":"  << std::setw(5) << file << "] " << message << std::endl;
    } else {
        SetConsoleTextAttribute(hConsole, 93);
        std::cerr << "[";
        SetConsoleTextAttribute(hConsole, 95);
        std::cerr << file;
        SetConsoleTextAttribute(hConsole, 93);
        std::cerr << ":";
        SetConsoleTextAttribute(hConsole, 95);
        std::cerr << std::setw(5) << line;
        SetConsoleTextAttribute(hConsole, 93);
        std::cerr << "]";
        SetConsoleTextAttribute(hConsole, 7);
        std::cerr << " " << message << std::endl;
    }
#endif
}

OutputHandler & OutputHandler::getInstance() {
    static OutputHandler handler;
    return handler;
}

OutputHandler::OutputHandler():
    m_reportHandler( new ReportHandler ),
    m_messageHandler( new MessageHandler ),
    m_warningHandler( new WarningHandler ),
    m_errorHandler( new ErrorHandler ),
    m_debugHandler( new DebugHandler ),
    m_colors(true)
{

}

OutputHandler::~OutputHandler() {
    delete m_reportHandler;
    m_reportHandler = 0;
    delete m_messageHandler;
    m_messageHandler = 0;
    delete m_warningHandler;
    m_warningHandler = 0;
    delete m_errorHandler;
    m_errorHandler = 0;
    delete m_debugHandler;
    m_debugHandler = 0;
}

void OutputHandler::setReportHandler(ReportHandler * handler) {
    delete m_reportHandler;
    m_reportHandler = handler;
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

void OutputHandler::addReport(const std::ostringstream & stream, std::ostringstream & ss) {
    if ( m_reportHandler != 0 ) {
        m_reportHandler->addMessage( stream.str(), ss );
    }
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

void OutputHandler::addDebug(const std::ostringstream & stream,
                             const char *file,
                             unsigned int line) {
    if ( m_debugHandler != 0 ) {
        DebugHandler * handler = dynamic_cast<DebugHandler*>(m_debugHandler);
        if (handler != 0) {
            handler->addMessage( stream.str(), file, line );
        } else {
            m_debugHandler->addMessage( stream.str() );
        }
    }
}

void OutputHandler::setDefaultReportHandler() {
    delete m_reportHandler;
    m_reportHandler = new ReportHandler;
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

void OutputHandler::enableAllColors() {
    m_reportHandler->enableColors();
    m_messageHandler->enableColors();
    m_warningHandler->enableColors();
    m_errorHandler->enableColors();
    m_debugHandler->enableColors();
}

void OutputHandler::disableAllColors() {
    m_reportHandler->disableColors();
    m_messageHandler->disableColors();
    m_warningHandler->disableColors();
    m_errorHandler->disableColors();
    m_debugHandler->disableColors();
}

