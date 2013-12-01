/**
 * @file linalgparameterhandler.h
 */

#ifndef LINALGPARAMETERHANDLER_H
#define LINALGPARAMETERHANDLER_H

#include <globals.h>
#include <utils/parameterhandler.h>

class LinalgParameterHandler : public ParameterHandler
{
    friend class LinalgParameterHandlerTestSuite;

public:
    THREAD_STATIC_DECL ParameterHandler & getInstance();
    void writeParameterFile();
    void initParameters();

private:
    THREAD_STATIC_DECL const char * sm_defaultFilename;

    LinalgParameterHandler();
    LinalgParameterHandler(const LinalgParameterHandler &){}
};

#endif // LINALGPARAMETERHANDLER_H
