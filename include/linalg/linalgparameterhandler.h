/**
 * @file linalgparameterhandler.h This file contains the API for the LinalgParameterHandler class
 * @author smidla
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

public:
    /**
     * Returns an instance of the ParameterHandler static object.
     *
     * @return An instance of the ParameterHandler static object.
     */
    THREAD_STATIC_DECL ParameterHandler & getInstance();

    /**
     * Creates a new parameter file for the linear algebra specific parameters and fills it with the default data.
     */
    void writeParameterFile();

    /**
     * Initializes the default linear algebra specific parameters.
     */
    void initParameters();

private:

    /**
     * The default filename to read the paramters from or write them to.
     */
    THREAD_STATIC_DECL const char * sm_defaultFilename;

    /**
     * Constructor of the LinalgParameterHandler class.
     *
     * @constructor
     */
    LinalgParameterHandler();

    /**
     * Copy constructor of the LinalgParameterHandler class.
     *
     * @constructor
     */
    LinalgParameterHandler(const LinalgParameterHandler &){}
};

#endif // LINALGPARAMETERHANDLER_H
