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

private:

    THREAD_STATIC_DECL const char * sm_filename;

    LinalgParameterHandler();
    LinalgParameterHandler(const LinalgParameterHandler &){}
    void initParameters();
};

#endif // LINALGPARAMETERHANDLER_H
