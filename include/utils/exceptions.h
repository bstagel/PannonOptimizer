//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

/**
 * @file exceptions.h This file contains the API of all exception classes.
 * @author Jozsef Smidla, Peter Tar
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
 * This exception should be thrown when the specified file is not found.
 *
 * @class FileNotFoundException
 */
class FileNotFoundException: public PanOptException {
public:
        FileNotFoundException(const std::string & message,
                              const std::string & fileName):
            PanOptException(message),
            m_fileName(fileName){}

        /**
         * Returns with the file name.
         *
         * @return File name.
         */
        const std::string & getFileName() const {
            return m_fileName;
        }
protected:

        /**
         * This member contains the name of the file.
         */
        std::string m_fileName;
};

/**
 * This exception should be thrown when the program attempts to use memory pointed by a null pointer.
 *
 * @class NullPointerException
 */
class NullPointerException: public PanOptException {
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
 * This exception should be thrown when the program attempts to use an invalid index.
 *
 * @class InvalidIndexException
 */
class InvalidIndexException: public PanOptException {
public:

    /**
     * Constructor of the InvalidIndexException class.
     *
     * @param message Custom message describing the circumstances of the exception.
     * @param min Describes the minimum valid value of the index.
     * @param max Describes the maximum valid value of the index.
     * @param value Describes the real value of the index.
     */
    InvalidIndexException(const std::string & message,
                          long long int min,
                          long long int max,
                          long long int value):
        PanOptException(message),
        m_min(min),
        m_max(max),
        m_value(value)
    {}

    /**
     * Returns with the minimum valid value of the index.
     *
     * @return Minimum value of the index.
     */
    long long int getMin() const {
        return m_min;
    }

    /**
     * Returns with the maximum valid value of the index.
     *
     * @return Maximum value of the index.
     */
    long long int getMax() const {
        return m_min;
    }

    /**
     * Returns with the real value of the index.
     *
     * @return Real value of the index.
     */
    long long int getValue() const {
        return m_value;
    }

    /**
     * Returns the circumstances of the exception.
     *
     * @return The circumstances of the exception.
     */
    inline std::string getMessage() const {return m_message
                + std::string(" min: ")+ std::to_string(m_min)
                + std::string(" max: ")+ std::to_string(m_max)
                + std::string(" value: ")+ std::to_string(m_value);}

protected:

    /**
     * Minimum valid value of the index.
     */
    long long int m_min;

    /**
     * Maximum valid value of the index.
     */
    long long int m_max;

    /**
     * Real value of the index.
     */
    long long int m_value;
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
 * This exception should be thrown when the selected pivot has bad numerical properties.
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

