#ifndef LINALGPARAMETERHANDLER_H
#define LINALGPARAMETERHANDLER_H

#include "utils/parameterhandler.h"

class LinalgParameterHandler : public ParameterHandler
{
public:
    ParameterHandler* getInstance();
    void writeParameterFile();

private:
    LinalgParameterHandler();
    void initParameters();
};

#endif // LINALGPARAMETERHANDLER_H
