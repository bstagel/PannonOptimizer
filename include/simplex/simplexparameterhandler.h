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

private:

    THREAD_STATIC_DECL const char * sm_filename;

    SimplexParameterHandler();
    SimplexParameterHandler(const SimplexParameterHandler &){}
    void initParameters();
};

#endif // SIMPLEXPARAMETERHANDLER_H
