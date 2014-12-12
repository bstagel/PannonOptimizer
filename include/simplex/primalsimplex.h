/**
 * @file primalsimplex.h This file contains the API of the PrimalSimplex class.
 */

#ifndef PRIMALSIMPLEX_H
#define	PRIMALSIMPLEX_H

#include <globals.h>
#include <simplex/simplex.h>
#include <simplex/primalpricing.h>
#include <simplex/primalratiotest.h>
#include <simplex/primalfeasibilitychecker.h>

/**
 * This class describes a general primal simplex object, containing all members of the algorithm.
 * The members are the primal pricing, primal feasibility checker, primal ratiotest. In this class we can find
 * the primal implementations of the abstract members of class Simplex.
 *
 * @class PrimalSimplex
 */
class PrimalSimplex: public Simplex
{
    friend class SimplexController;
private:

    /**
     * Default constructor of the class.
     *
     * @param simplexController A reference to the controller to reach the controller API.
     *
     * @constructor
     */
    PrimalSimplex(Basis* basis);

    /**
     * Default destructor of the class.
     *
     * @destructor
     */
    ~PrimalSimplex();

    /**
     * The primal pricing determines an improving candidate (an incoming variable, column).
     */
    PrimalPricing * m_pricing;

    /**
     * The primal feasibility checker can check the feasibility of the basic variables.
     * It can copmute the measure of primal infeasibility.
     */
    PrimalFeasibilityChecker * m_feasibilityChecker;

    /**
     * The primal ratiotest determines an outgoing variable (row), and the primal steplength.
     * After the ratiotest is done the basis change can be performed.
     */
    PrimalRatiotest * m_ratiotest;

    /**
     * The primal reduced cost(d) is needed to update the objective function value.
     * It is also needed by the iteration reporter.
     * The measure of the change is the product of d and the primal theta.
     */
    Numerical::Double m_primalReducedCost;

    /**
     * The primal theta (steplength) is needed to update the objective function value.
     * It's needed along with PrimalSimplex::m_primalReducedCost.
     */
    Numerical::Double m_primalTheta;

    /**
     * The dual theta (steplength) is used in updating the reduced costs.
     */
    Numerical::Double m_dualTheta;

    /**
     * Inherited from the parent Simplex class, computing the measure of primal infeasibility of the basicvariables.
     */
    virtual void computeFeasibility();

    /**
     * Inherited from the parent Simplex class, performing the primal pricing.
     *
     * @throws PrimalInfeasibleException if there is no improving candidate in phase 1
     * @throws OptimalException if there is no improving candidate in phase 2
     */
    virtual void price();

    /**
     * Inherited from the parent Simplex class, performing the primal ratiotest.
     */
    virtual void selectPivot();

    /**
     * Inherited from the parent Simplex class, performing the basis change.
     * It also computes the reduced costs, and the basic solution.
     * @throws PanOptException in case of invalid variable type
     */
    virtual void update();

    /**
     * Inherited from the parent Simplex class, it updates the reduced cost values.
     */
    virtual void updateReducedCosts();

    /**
     * Inherited from the parent Simplex class, sets the objective function value according to the
     * current phase.
     *
     * @param secondPhase Describes if the reference check is done in second phase or not.
     */
    virtual void setReferenceObjective(bool secondPhase);

    /**
     * Inherited from the parent Simplex class, checks whether a bad iteration was done.
     *
     * @param secondPhase Describes if the reference check is done in second phase or not.
     */
    virtual void checkReferenceObjective(bool secondPhase);

    /**
     * Inherited from the parent Simplex class, initializes the expand working tolerance.
     */
    virtual void initWorkingTolerance();

    /**
     * Inherited from the parent Simplex class, copmutes the expand working tolerance in every iteration.
     */
    virtual void computeWorkingTolerance();

    /**
     * Inherited from the parent Simplex class, releases the locked variables by the pricing.
     */
    virtual void releaseLocks();

    // Interface of the iteration report provider:
    /**
     * The function returns the iteration report fields of this class, see IterationReportProvider.
     */
    std::vector<IterationReportField> getIterationReportFields(enum ITERATION_REPORT_FIELD_TYPE & type) const;

    /**
     * Getter of one iteration Entry, see IterationReportProvider.
     */
    Entry getIterationEntry(const std::string & name,
                            enum ITERATION_REPORT_FIELD_TYPE & type) const;

    void resetTolerances();
};

#endif /* PRIMALSIMPLEX_H */
