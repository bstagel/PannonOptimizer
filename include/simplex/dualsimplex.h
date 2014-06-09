/**
 * @file dualsimplex.h
 */


#ifndef DUALSIMPLEX_H
#define	DUALSIMPLEX_H

#include <globals.h>

#include <simplex/simplex.h>
#include <simplex/dualpricing.h>
#include <simplex/dualfeasibilitychecker.h>
#include <simplex/dualratiotest.h>
#include <utils/iterationreportprovider.h>
#include <utils/iterationreport.h>
#include <simplex/dualpricingcontroller.h>

class DualSimplex: public Simplex
{
    friend class SimplexController;
private:
    DualSimplex();
    ~DualSimplex();

    DualPricing * m_pricing;
    DualPricingController * m_pricingController;

    DualFeasibilityChecker * m_feasibilityChecker;

    DualRatiotest * m_ratiotest;

//    int m_incomingIndex;
//    int m_outgoingIndex;
    Numerical::Double m_primalReducedCost;
    Numerical::Double m_primalTheta;
    Numerical::Double m_dualTheta;

    const char * m_phaseName;

    Vector m_pivotRowOfBasisInverse;

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

    void computeTransformedRow(Vector* alpha, int rowIndex);
    Numerical::Double computePrimalTheta(const Vector& alpha, int outgoingIndex, VARIABLE_STATE outgoingState);

    void registerIntoIterationReport(IterationReport * iterationReport) const;

    // Interface of the iteration report provider:
    std::vector<IterationReportField> getIterationReportFields(
            enum ITERATION_REPORT_FIELD_TYPE & type) const;

    Entry getIterationEntry(const std::string & name,
                            enum ITERATION_REPORT_FIELD_TYPE & type) const;

};

#endif /* DUALSIMPLEX_H */
