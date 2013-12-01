/**
 * @file simplexparameterhandler.h
 */

#ifndef SIMPLEXPARAMETERHANDLER_H
#define SIMPLEXPARAMETERHANDLER_H

#include <globals.h>
#include <utils/parameterhandler.h>

class SimplexParameterHandler : public ParameterHandler
{
    friend class SimplexParameterHandlerTestSuite;

public:
    THREAD_STATIC_DECL ParameterHandler & getInstance();
    void writeParameterFile();
    void initParameters();

private:
    THREAD_STATIC_DECL const char * sm_defaultFilename;

    SimplexParameterHandler();
    SimplexParameterHandler(const SimplexParameterHandler &){}
};

#endif // SIMPLEXPARAMETERHANDLER_H
