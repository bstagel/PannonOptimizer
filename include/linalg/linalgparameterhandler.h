#ifndef LINALGPARAMETERHANDLER_H
#define LINALGPARAMETERHANDLER_H

#include <globals.h>
#include <utils/parameterhandler.h>

class LinalgParameterHandler : public ParameterHandler
{
    friend class LinalgParameterHandlerTestSuite;

public:
    static ParameterHandler & getInstance();
    void writeParameterFile();

private:
    LinalgParameterHandler();
    LinalgParameterHandler(const LinalgParameterHandler &){}
    void initParameters();
};

#endif // LINALGPARAMETERHANDLER_H
