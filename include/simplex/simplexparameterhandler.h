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
    friend class InitPanOpt;
public:
    static ParameterHandler & getInstance();
    /**
    * @brief writeParameterFile
    * @throws -1 if the outputstream is not open
    */
    void writeParameterFile();
    void initParameters();

private:
    static thread_local const char * sm_defaultFilename;

    static SimplexParameterHandler * sm_instance;

    static void init();

    SimplexParameterHandler();
    SimplexParameterHandler(const SimplexParameterHandler &){}
};

#endif // SIMPLEXPARAMETERHANDLER_H
