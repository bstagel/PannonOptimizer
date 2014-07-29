/**
 * @file exceptions.h This file contains the API of all exception classes.
 * @author smidla, tar
 */


#ifndef EXCEPTION_H
#define	EXCEPTION_H

#include <globals.h>

#include <iostream>
#include <typeinfo>
#include <functional>
#include <exception>
#include <typeinfo>
#include <iostream>
#include <stdexcept>
#if __cplusplus > 199711L
    #include <system_error>
#endif
#include <debug.h>

/**
 * Base class of all customized exception classes
 *
 * @see InfeasibleException
 * @see NumericalException
 * @see OptimalException
 * @see UnboundedException
 * @class Exception
 */
class PanOptException
{
public:

    /**
     * Default constructor of the PanOptException class.
     *
     * @constructor
     * @param message Custom message describing the circumstances of the exception.
     */
    PanOptException(const std::string & message): m_message(message) {}

    /**
     * Destructor of the PanOptException class.
     *
     * @destructor
     */
    virtual ~PanOptException() {}

    /**
     * Returns the circumstances of the exception.
     *
     * @return The circumstances of the exception.
     */
    inline std::string getMessage() const {return m_message;}

protected:
    /**
     * Custom message describing the circumstances of the exception.
     */
    std::string m_message;
};

/**
 * This exception should be thrown when the program attempts to use memory pointed by a null pointer.
 *
 * @class NullPointerException
 */
class NullPointerException: PanOptException {
public:

    /**
     * Constructor of the NullPointerException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     */
    NullPointerException(const std::string & message):
        PanOptException(message) {}
};

/**
 * Base class of all exceptions returning information about the solution of the problem.
 *
 * @see OptimalException
 * @see PrimalUnboundedException
 * @see DualUnboundedException
 * @see PrimalInfeasibleException
 * @see DualInfeasibleException
 * @class OptimizationResultException
 */
class OptimizationResultException: public PanOptException {
public:

    /**
     * Constructor of the OptimizationResultException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     */
    OptimizationResultException(const std::string& message): PanOptException(message) {}
};


/**
 * This exception should be thrown when optimal result is found for the problem.
 *
 * @class OptimalException
 */
class OptimalException: public OptimizationResultException {
public:

    /**
     * Constructor of the OptimalException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     */
    OptimalException(const std::string& message): OptimizationResultException(message) {}
};

/**
 * This exception should be thrown when unboundedness raised and therefore the problem will be always infeasible
 * (for example during presolving)
 *
 * @class PrimalUnboundedException
 */
class PrimalUnboundedException: public OptimizationResultException {
public:

    /**
     * Constructor of the PrimalUnboundedException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     */
    PrimalUnboundedException(const std::string& message): OptimizationResultException(message) {}
};

/**
 * This exception should be thrown when unboundedness raised and therefore the problem will be always infeasible
 * (for example during presolving)
 *
 * @class DualUnboundedException
 */
class DualUnboundedException: public OptimizationResultException {
public:

    /**
     * Constructor of the DualUnboundedException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     */
    DualUnboundedException(const std::string& message): OptimizationResultException(message) {}
};

/**
 * This exception should be thrown when infeasibility raised and therefore the problem will be always infeasible
 * (for example during presolving)
 *
 * @class PrimalInfeasibleException
 */
class PrimalInfeasibleException: public OptimizationResultException {
public:

    /**
     * Constructor of the PrimalInfeasibleException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     */
    PrimalInfeasibleException(const std::string& message): OptimizationResultException(message) {}
};

/**
 * This exception should be thrown when infeasibility raised and therefore the problem will be always infeasible
 * (for example during presolving)
 *
 * @class DualInfeasibleException
 */
class DualInfeasibleException: public OptimizationResultException {
public:

    /**
     * Constructor of the DualInfeasibleException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     */
    DualInfeasibleException(const std::string& message): OptimizationResultException(message) {}
};

/**
 * This exception should be thrown when a Numerical error is detected.
 *
 * @class NumericalException
 */
class NumericalException: public PanOptException {
public:

    /**
     * Constructor of the NumericalException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     */
    NumericalException(const std::string& message): PanOptException(message) {}
};

/**
 * This exception should be thrown when invalid parameters are specified in the parameter files.
 *
 * @class ParameterException
 */
class ParameterException: public PanOptException {
public:

    /**
     * Constructor of the ParameterException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     */
    ParameterException(const std::string& message): PanOptException(message) {}
};

/**
 * This exception should be thrown when
 *
 * @class BadPivotException
 */
class BadPivotException: public PanOptException{
public:

    /**
     * Constructor of the BadPivotException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     */
    BadPivotException(const std::string &message): PanOptException(message){}
};

/**
 * This exception should be thrown when fallback is detected during the simplex iterations of the solver.
 *
 * @class FallbackException
 */
class FallbackException: public PanOptException{
public:

    /**
     * Constructor of the FallbackException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     */
    FallbackException(const std::string &message): PanOptException(message){}
};

/**
 * This exception should be thrown when it is desirable to switch between the dual and primal simplex algorithms.
 *
 * @class SwitchAlgorithmException
 */
class SwitchAlgorithmException: public PanOptException{
public:

    /**
     * Constructor of the SwitchAlgorithmException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     */
    SwitchAlgorithmException(const std::string &message):PanOptException(message){}
};

/**
 * This exception should be thrown when there is a syntax error in an input file.
 *
 * @class SyntaxErrorException
 */
class SyntaxErrorException: public PanOptException {
public:

    /**
     * Constructor of the SyntaxErrorException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     * @param row The content of the row with the syntax error.
     * @param rowIndex The index of the row in the file where the error is located.
     * @param columnIndex The index of the column in the file where the error is located.
     */
    SyntaxErrorException(const std::string & message,
                         const std::string & row,
                         unsigned int rowIndex,
                         unsigned int columnIndex):
        PanOptException(message),
        m_row(row),
        m_rowIndex(rowIndex),
        m_columnIndex(columnIndex) {

    }

    /**
     * Returns the content of the row with the syntax error.
     *
     * @return The content of the row with the syntax error.
     */
    const std::string getRow() const {
        return m_row;
    }

    /**
     * @brief Returns the index of the row in the file where the error is located.
     *
     * @return The index of the row in the file where the error is located.
     */
    unsigned int getRowIndex() const {
        return m_rowIndex;
    }

    /**
     * @brief Returns the index of the column in the file where the error is located.
     *
     * @return The index of the column in the file where the error is located.
     */
    unsigned int getColumnIndex() const {
        return m_columnIndex;
    }

    /**
     * Prints the syntax error data to the output.
     */
    void show() const {
        LPERROR(m_rowIndex << ".: " << m_message);
        LPERROR(m_row);
        LPERROR(std::string(m_columnIndex, ' ') << "^");
    }
protected:

    /**
     * The content of the row with the syntax error.
     */
    std::string m_row;

    /**
     * The index of the row in the file where the error is located.
     */
    unsigned int m_rowIndex;

    /**
     * The index of the column in the file where the error is located.
     */
    unsigned int m_columnIndex;
};

#endif	/* EXCEPTION_H */

