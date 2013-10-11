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
    static ParameterHandler & getInstance();
    void writeParameterFile();

private:

    static const char * sm_filename;

    SimplexParameterHandler();
    SimplexParameterHandler(const SimplexParameterHandler &){}
    void initParameters();
};

#endif // SIMPLEXPARAMETERHANDLER_H
