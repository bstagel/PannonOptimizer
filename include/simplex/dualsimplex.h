/**
 * @file dualsimplex.h
 */


#ifndef DUALSIMPLEX_H
#define	DUALSIMPLEX_H

#include <globals.h>

#include <simplex/simplex.h>
#include <simplex/dualpricing.h>
#include <simplex/dualupdater.h>
#include <simplex/dualfeasibilitychecker.h>
#include <simplex/dualratiotest.h>
#include <simplex/dualratiotestupdater.h>
#include <utils/iterationreportprovider.h>
#include <utils/iterationreport.h>

class DualSimplex: public Simplex
{
public:
    DualSimplex();

private:
    DualPricing * m_pricing;

    DualUpdater * m_updater;

    DualFeasibilityChecker * m_feasibilityChecker;

    DualRatiotest * m_ratiotest;

    bool m_feasible;
    int m_incomingIndex;
    int m_outgoingIndex;
    Numerical::Double m_primalReducedCost;
    Numerical::Double m_primalTheta;
    Numerical::Double m_dualTheta;

    const char * m_phaseName;

    void initModules();
    void releaseModules();

    virtual void computeFeasibility() throw (NumericalException);
    virtual void checkFeasibility() throw (OptimizationResultException, NumericalException);
    virtual void price() throw (OptimizationResultException, NumericalException);
    virtual void selectPivot() throw (OptimizationResultException, NumericalException);
    virtual void update()throw (NumericalException);

    void computeTransformedRow(Vector* alpha, unsigned int rowIndex) throw (NumericalException);
    Numerical::Double computePrimalTheta(const Vector& alpha, int rowIndex);

    void registerIntoIterationReport(IterationReport * iterationReport) const;

    // Interface of the iteration report provider:
    std::vector<IterationReportField> getIterationReportFields(
            enum ITERATION_REPORT_FIELD_TYPE & type) const;

    Entry getIterationEntry(const std::string & name,
                            enum ITERATION_REPORT_FIELD_TYPE & type) const;

};

#endif /* DUALSIMPLEX_H */
