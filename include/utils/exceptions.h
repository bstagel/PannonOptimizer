/**
 * @file exceptions.h
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

/**
 * Base class of all customized exception classes
 * @see InfeasibleException
 * @see NumericalException
 * @see OptimalException
 * @see UnboundedException
 * @class Exception
 */
class PanOptException
{
public:
    PanOptException(const std::string & message): m_message(message) {}
    virtual ~PanOptException() {}

    inline std::string getMessage() const {return m_message;}

protected:
    std::string m_message;
};

/**
 * @class OptimizationResultException
 */
class OptimizationResultException: public PanOptException {
public:
    OptimizationResultException(const std::string& message): PanOptException(message) {}
};


/**
 * @class OptimalException
 */
class OptimalException: public OptimizationResultException {
public:
    OptimalException(const std::string& message): OptimizationResultException(message) {}
};

/**
 * This exception should be thrown when unboundedness raised and therefore the problem will be always infeasible
 * (for example during presolving)
 * @class PrimalUnboundedException
 */
class PrimalUnboundedException: public OptimizationResultException {
public:
    PrimalUnboundedException(const std::string& message): OptimizationResultException(message) {}
};

/**
 * This exception should be thrown when unboundedness raised and therefore the problem will be always infeasible
 * (for example during presolving)
 * @class DualUnboundedException
 */
class DualUnboundedException: public OptimizationResultException {
public:
    DualUnboundedException(const std::string& message): OptimizationResultException(message) {}
};

/**
 * This exception should be thrown when infeasibility raised and therefore the problem will be always infeasible
 * (for example during presolving)
 * @class PrimalInfeasibleException
 */
class PrimalInfeasibleException: public OptimizationResultException {
public:
    PrimalInfeasibleException(const std::string& message): OptimizationResultException(message) {}
};

/**
 * This exception should be thrown when infeasibility raised and therefore the problem will be always infeasible
 * (for example during presolving)
 * @class DualInfeasibleException
 */
class DualInfeasibleException: public OptimizationResultException {
public:
    DualInfeasibleException(const std::string& message): OptimizationResultException(message) {}
};

/**
 * @class NumericalException
 */
class NumericalException: public PanOptException {
public:
    NumericalException(const std::string& message): PanOptException(message) {}
};


#endif	/* EXCEPTION_H */

