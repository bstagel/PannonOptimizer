/**
 * @file dualsimplex.h This file contains the API of the DualSimplex class.
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

/**
 * Describes a general dual simplex object, containing all members of the algorithm.
 * The members are the dual pricing, dual feasibility checker, dual ratiotest. In this class we
 * can find the dual implementations of the abstract members of class Simplex.
 *
 * @class DualSimplex
 */
class DualSimplex: public Simplex
{
    friend class SimplexController;
private:

    /**
     * Default constructor of the class.
     *
     * @constructor
     */
    DualSimplex();

    /**
     * Default destructor of the class.
     *
     * @destructor
     */
    ~DualSimplex();

    /**
     * The dual pricing determines an improving candidate (an outgoing variable, row).
     */
    DualPricing * m_pricing;

    /**
     * The dual pricing controller is needed for the paralell pricing framework.
     */
    DualPricingController * m_pricingController;

    /**
     * The dual feasibility checker can check the feasibility of the reduced costs.
     * It can compute the measure of dual infeasibility.
     */
    DualFeasibilityChecker * m_feasibilityChecker;

    /**
     * The dual ratiotest determines an incoming variable (column), and the dual steplength.
     * After the ratiotest is done the basis change can be performed.
     */
    DualRatiotest * m_ratiotest;

    /**
     * The primal reduced cost(d) is needed to update the objective function value.
     * The measure of the change is the product of d and the primal theta.
     */
    Numerical::Double m_primalReducedCost;

    /**
     * The primal theta (steplength) is needed to update the objective function value.
     * It is needed along with the primal reduced cost.
     */
    Numerical::Double m_primalTheta;

    /**
     * The dual theta (steplength) is used in updating the solution, and updating the reduced costs.
     */
    Numerical::Double m_dualTheta;

    /**
     * This parameter contains the name of the current phase of the solver algorithm.
     */
    const char * m_phaseName;

    /**
     * This vector is needed in the pricing updater.
     */
    Vector m_pivotRowOfBasisInverse;

    /**
     * Function that initializes the dual simplex modules.
     * The pricing is initialized according to the simplex parameter file, the ratiotest and
     * the dual feasibility checker are also initialized.
     */
    void initModules();

    /**
     * The function releases the dual simplex modules reserved in DualSimplex::initModules().
     */
    void releaseModules();

    /**
     * Inherited from the parent Simplex class, computing the measure of dual infeasibility of the reduced costs.
     */
    virtual void computeFeasibility();

    /**
     * Inherited from the parent Simplex class, checking the dual feasibility of the reduced costs.
     * @throws SwitchAlgorithmException at entering phase 2 if this option is turned on in the parameter file.
     */
    virtual void checkFeasibility();

    /**
     * Inherited from the parent Simplex class, performing the dual pricing.
     * @throws DualInfeasibleException if there is no improving candidate in phase 1
     * @throws OptimalException if there is no improving candidate in phase 2
     */
    virtual void price();

    /**
     * Inherited from the parent Simplex class, performing the dual ratiotest.
     */
    virtual void selectPivot();

    /**
     * Inherited from the parent Simplex class, performing the basis change.
     * It also computes the reduced costs, and the basic solution.
     * @throws PanOptException if there is a boundflipping variable in the basis
     * @throws PanOptException if there is an invalid variable type
     */
    virtual void update();

    /**
     * Inherited from the parent Simplex class, it updates the reduced cost values.
     */
    virtual void updateReducedCosts();

    /**
     * Inherited from the parent Simplex class, sets the objective function value.
     * It considers the current phase.
     */
    virtual void setReferenceObjective();

    /**
     * Inherited from the parent Simplex class, checks whether a bad iteration was done.
     */
    virtual void checkReferenceObjective();

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

    /**
     * This function computes the transformed alpha row.
     * @param alpha will hold the computed vector
     * @param rowIndex shows the row of the problem's matrix to be transformed
     * @throws NumericalException if the rowIndex is invalid
     */
    void computeTransformedRow(Vector* alpha, int rowIndex);

    /**
     * Function computing the value of the primal theta.
     * It can be computed as the ratio of the outgoing variable and the alpha pivot value.
     * @param alpha is the denominator of the primal theta
     * @param outgoingIndex is needed in the numerator of the primal theta
     * @param outgoingState the value of the primal theta depends on the state of the outgoing variable
     * @return the computed value of the primal theta.
     * @throws PanOptException in case of invalid outgoing state
     */
    Numerical::Double computePrimalTheta(const Vector& alpha, int outgoingIndex, VARIABLE_STATE outgoingState);

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

};

#endif /* DUALSIMPLEX_H */
