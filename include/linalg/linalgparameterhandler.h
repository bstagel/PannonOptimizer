/**
 * @file linalgparameterhandler.h This file contains the API for the LinalgParameterHandler class
 * @author Peter Tar
 */

#ifndef LINALGPARAMETERHANDLER_H
#define LINALGPARAMETERHANDLER_H

#include <globals.h>
#include <utils/parameterhandler.h>

/**
 * Class for retrieving and handling linear algebra specific parameters of the solver.
 *
 * @class LinalgParameterHandler
 */
class LinalgParameterHandler : public ParameterHandler
{
    friend class LinalgParameterHandlerTestSuite;
    friend class InitPanOpt;
public:
    /**
     * Returns an instance of the ParameterHandler static object.
     *
     * @return An instance of the ParameterHandler static object.
     */
    static ParameterHandler & getInstance();

    /**
     * Creates a new parameter file for the linear algebra specific parameters.
     * The file is created with the default paramteres.
     */
    void writeParameterFile();

    /**
     * Initializes the handler with the default linear algebra specific parameters.
     */
    void initParameters();

private:

    /**
     * The default filename to read the paramters from or write them to.
     */
    static thread_local const char * sm_defaultFilename;

    static LinalgParameterHandler * sm_instance;

    static void _globalInit();

    /**
     * Constructor of the LinalgParameterHandler class.
     *
     * @constructor
     */
    LinalgParameterHandler();

    //TODO: No implementation here
    /**
     * Copy constructor of the LinalgParameterHandler class.
     *
     * @constructor
     * @param original The function copies this handler.
     */
    LinalgParameterHandler(const LinalgParameterHandler & original){
        __UNUSED(original);
    }
};

#endif // LINALGPARAMETERHANDLER_H
