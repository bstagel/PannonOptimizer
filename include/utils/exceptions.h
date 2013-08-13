/**
 * @file exceptions.h
 */


#ifndef EXCEPTION_H
#define	EXCEPTION_H

#include "globals.h"

#include <iostream>

/**
 * Base class of all customized exception classes
 * @see InfeasibleException
 * @see NumericalException
 * @see OptimalException
 * @see UnboundedException
 * @class Exception
 */
class Exception
{
public:
    Exception(const std::string & message): m_message(message) {}
    virtual ~Exception() {}

    inline std::string getMessage() const {return m_message;}

protected:
    std::string m_message;
};

/**
 * @class OptimalException
 */
class OptimalException: public Exception {
public:
    OptimalException(const std::string& message): Exception(message) {}
};

/**
 * This exception should be thrown when unboundedness raised and therefore the problem will be always infeasible
 * (for example during presolving)
 * @class UnboundedException
 */
class UnboundedException: public Exception {
public:
    UnboundedException(const std::string& message): Exception(message) {}
};

/**
 * This exception should be thrown when infeasibility raised and therefore the problem will be always infeasible
 * (for example during presolving)
 * @class InfeasibleException
 */
class InfeasibleException: public Exception {
public:
    InfeasibleException(const std::string& message): Exception(message) {}
};

/**
 * @class NumericalException
 */
class NumericalException: public Exception {
public:
    NumericalException(const std::string& message): Exception(message) {}
};


#endif	/* EXCEPTION_H */

