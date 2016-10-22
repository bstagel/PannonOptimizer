#include "outputhandlertest.h"
#include <utils/outputhandler.h>

OutputHandlerTestSuite::OutputHandlerTestSuite(const char * name):
    UnitTest(name)
{
    ADD_TEST( OutputHandlerTestSuite::init );
    ADD_TEST( OutputHandlerTestSuite::changeMessage );
    ADD_TEST( OutputHandlerTestSuite::changeWarning );
    ADD_TEST( OutputHandlerTestSuite::changeError );
}

void OutputHandlerTestSuite::init() {

}

class MyOutputHandler: public GeneralMessageHandler {
public:
    void addMessage( const std::string & message ) {
        std::cout << "This is a test message output handler: " << message << std::endl;
    }

    void addWarning( const std::string & message ) {
        std::cout << "This is a test warning output handler: " << message << std::endl;
    }

    void addError( const std::string & message ) {
        std::cout << "This is a test error output handler: " << message << std::endl;
    }
};

void OutputHandlerTestSuite::changeMessage() {
    TEST_ASSERT(2);
    LPINFO("Simple message output test");
    OutputHandler::getInstance().setMessageHandler( new MyOutputHandler );
    LPINFO("Another message output test");
}

void OutputHandlerTestSuite::changeWarning() {
    TEST_ASSERT(2);
    LPWARNING("Simple warning output test");
    OutputHandler::getInstance().setWarningHandler( new MyOutputHandler );
    LPWARNING("Another warning output test");
}

void OutputHandlerTestSuite::changeError() {
    TEST_ASSERT(2);
    LPERROR("Simple error output test");
    OutputHandler::getInstance().setErrorHandler( new MyOutputHandler );
    LPERROR("Another error output test");
}
