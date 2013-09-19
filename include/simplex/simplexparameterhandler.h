#ifndef SIMPLEXPARAMETERHANDLER_H
#define SIMPLEXPARAMETERHANDLER_H

#include "utils/parameterhandler.h"

class SimplexParameterHandler : public ParameterHandler
{
    friend class SimplexParameterHandlerTestSuite;

public:
    static ParameterHandler & getInstance();
    void writeParameterFile();

private:
    SimplexParameterHandler();
    SimplexParameterHandler(const SimplexParameterHandler &){}
    void initParameters();
};

#endif // SIMPLEXPARAMETERHANDLER_H