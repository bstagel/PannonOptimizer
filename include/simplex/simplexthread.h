/**
 * @file simplexthread.h
 */

#ifndef SIMPLEXTHREAD_H
#define SIMPLEXTHREAD_H

#include <globals.h>
#include <utils/iterationreportprovider.h>
#include <simplex/primalsimplex.h>
#include <simplex/dualsimplex.h>

/**
 * @class SimplexThread
 */
class SimplexThread: public IterationReportProvider{
public:
    /**
     * The result of simplex iterations.
     * ITERATE: if iterations were done without exceptions
     * TERMINATE: if an unexpected exception (eg. NumericalException) is present
     * COMPLETE: if an OptimizationResultException is cought.
     */
    enum RESULT{
        ITERATE = 0,
        TERMINATE,
        COMPLETE
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

    /**
     * The function returns the iteration report fields of this class.
     *
     * @see IterationReportProvider
     */
    std::vector<IterationReportField> getIterationReportFields(enum ITERATION_REPORT_FIELD_TYPE & type) const;

    /**
     * Getter of one iteration Entry.
     *
     * @see IterationReportProvider
     */
    Entry getIterationEntry(const std::string & name,
                            enum ITERATION_REPORT_FIELD_TYPE & type) const;

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
    PanOptException getException()const{return m_exception;}

    /**
     * Getter of the objectiveValue member.
     * @return SimplexThread::m_objectiveValue
     */
    Numerical::Double getObjectiveValue()const{return m_objectiveValue;}

    /**
     * This function performs simplex iterations, according to the parameter.
     * @param iterationNumber number of iterations to do.
     */
    void performIterations(int iterationNumber);

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

    /**
     * Describes the number of iterations done.
     */
    int m_iterationNumber;

    /**
     * The current objective value.
     */
    Numerical::Double m_objectiveValue;

    /**
     * This points to the current solver algorithm, a Simplex pointer.
     */
    Simplex * m_currentSimplex;

    //Parameter references
    /**
     * Parameter reference for "debug_level" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const int & m_debugLevel;

    /**
     * Parameter reference for "enable_export" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const bool & m_enableExport;

    /**
     * Pointer to the iteration reporter.
     */
    IterationReport* m_iterationReport;
};

#endif // SIMPLEXTHREAD_H
