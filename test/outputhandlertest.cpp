#include "outputhandlertest.h"
#include <utils/outputhandler.h>

OutputHandlerTestSuite::OutputHandlerTestSuite(const char * name):
    UnitTest(name)
{
    ADD_TEST( OutputHandlerTestSuite::init );
    ADD_TEST( OutputHandlerTestSuite::change );
}

void OutputHandlerTestSuite::init() {

}

class MyOutputHandler: public GeneralMessageHandler {
public:
    void addMessage( const std::string & message ) {
        std::cout << "This is a test output handler: " << message << std::endl;
    }
};

void OutputHandlerTestSuite::change() {
    TEST_ASSERT(2);
    LPINFO("Simple output test");
    OutputHandler::getInstance().setMessageHandler( new MyOutputHandler );
    LPINFO("Another output test");
}
