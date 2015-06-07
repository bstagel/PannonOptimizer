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
 * @file simplexthread.h
 */

#ifndef SIMPLEXTHREAD_H
#define SIMPLEXTHREAD_H

#include <globals.h>
#include <simplex/primalsimplex.h>
#include <simplex/dualsimplex.h>

/**
 * @class SimplexThread
 */
class SimplexThread{
public:
    /**
     * The result of simplex iterations.
     * ITERATE: if iterations were done without exceptions
     * TERMINATE: if an unexpected exception (eg. NumericalException) is present
     * COMPLETE: if an OptimizationResultException is cought.
     */
    enum RESULT{
        ITERATED = 0,
        TERMINATED,
        FINISHED
    };

    enum EXCEPTION_TYPE{
        OPTIMAL = 0,
        PRIMAL_INFEASIBLE,
        PRIMAL_UNBOUNDED,
        DUAL_INFEASIBLE,
        DUAL_UNBOUNDED,
        NUMERICAL,
        FALLBACK
    };

    /**
     * Default constructor of the class.
     *
     * @constructor
     * @param currentSimplex a pointer to the current simplex object.
     */
    SimplexThread(Simplex * currentSimplex);

    /**
     * Default destructor of the class.
     *
     * @destructor
     */
    virtual ~SimplexThread();

    //API
    /**
     * Getter of the iterationNumber member.
     * @return SimplexThread::m_iterationNumber
     */
    int getIterationNumber()const{return m_iterationNumber;}

    /**
     * Getter of the result member.
     * @return SimplexThread::m_result
     */
    RESULT getResult()const{return m_result;}

    /**
     * Getter of the exception member
     * @return SimplexThread::m_exception
     */
    PanOptException* getException(){return &m_exception;}

    EXCEPTION_TYPE getExceptionType()const{return m_exceptionType;}
    /**
     * This function performs simplex iterations, according to the parameter.
     * @param mainIterations The number of iterations at the last inversion.
     * @param iterationNumber number of iterations to do.
     */
    void performIterations(Basis* basis,
                           IterationReport* iterationreport,
                           int mainIterations,
                           int iterationNumber);

    /**
     * Sets the actual simplex object, necessary for primal-dual switching
     */
    void setCurrentSimplex(Simplex * currentSimplex);

private:

    /**
     * Result of the simplex iterations.
     * @see SimplexThread::RESULT
     */
    RESULT m_result;

    /**
     * If an exception is thrown, we register the type of it.
     * These exceptions are handled at the synchronisation of the threads.
     */
    PanOptException m_exception;

    EXCEPTION_TYPE m_exceptionType;

    /**
     * Describes the number of iterations done.
     */
    int m_iterationNumber;

    /**
     * This points to the current solver algorithm, a Simplex pointer.
     */
    Simplex * m_currentSimplex;

    //Parameter values
    /**
     * Parameter reference for "save_basis" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const bool & m_saveBasis;

    /**
     * Parameter reference for "debug_level" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const int & m_debugLevel;

};

#endif // SIMPLEXTHREAD_H
