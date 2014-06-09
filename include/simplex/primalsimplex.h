/**
 * @file primalsimplex.h
 */

#ifndef PRIMALSIMPLEX_H
#define	PRIMALSIMPLEX_H

#include <globals.h>

#include <simplex/simplex.h>
#include <simplex/primalpricing.h>
#include <simplex/primalratiotest.h>

class PrimalSimplex: public Simplex
{
    friend class SimplexController;
private:
    PrimalSimplex();
    ~PrimalSimplex();

    PrimalPricing * m_pricing;

    PrimalFeasibilityChecker * m_feasibilityChecker;

    PrimalRatiotest * m_ratiotest;

    Vector m_alpha;

//    int m_incomingIndex;
//    int m_outgoingIndex;
    Numerical::Double m_primalReducedCost;
    Numerical::Double m_primalTheta;
    Numerical::Double m_dualTheta;

    const char * m_phaseName;

    void initModules();
    void releaseModules();

    virtual void computeFeasibility();
    virtual void checkFeasibility();
    virtual void price();
    virtual void selectPivot();
    virtual void update();

    virtual void updateReducedCosts();

    virtual void setReferenceObjective();
    virtual void checkReferenceObjective();

    virtual void initWorkingTolerance();
    virtual void computeWorkingTolerance();

    virtual void releaseLocks();

    void registerIntoIterationReport(IterationReport * iterationReport) const;

    // Interface of the iteration report provider:
    std::vector<IterationReportField> getIterationReportFields(
            enum ITERATION_REPORT_FIELD_TYPE & type) const;

    Entry getIterationEntry(const std::string & name,
                            enum ITERATION_REPORT_FIELD_TYPE & type) const;
};

#endif /* PRIMALSIMPLEX_H */
