/**
 * @file dualpricing.h This file contains the API of the DualPricing class.
 */

#ifndef DUALPRICING_H
#define	DUALPRICING_H

#include <globals.h>
#include <linalg/vector.h>
#include <utils/indexlist.h>
#include <simplex/basis.h>
#include <simplex/pricing/simpri.h>
#include <simplex/pricing.h>

class SimplexModel;

/**
 * Describes the dual pricing module.
 * It determines an improving outgoing variable. Different pricing strategies can be derived from the class.
 * @class DualPricing
 */
class DualPricing: public Pricing {
public:

    /**
     * Initializing constructor of the class
     * @param basicVariableValues the X_B values
     * @param basicVariableFeasibilities the feasibility states of the X_B values
     * @param reducedCostFeasibilities the feasibility states of the reduced costs
     * @param basisHead containing basicvariable indices
     * @param simplexModel model of the LP problem
     * @param basis the B basis
     * @constructor
     */
    DualPricing(const Vector &basicVariableValues,
                IndexList<> * basicVariableFeasibilities,
                const IndexList<> & reducedCostFeasibilities,
                const std::vector<int> & basisHead,
                const SimplexModel & simplexModel,
                const Basis & basis);

    /**
     * Default destructor of the class.
     * @destructor
     */
    virtual ~DualPricing();

    /**
     * This function performs and controlls the phase 1 dual pricing.
     * Pure virtual function should be implemented in the derived classes.
     * @return the variable index of the selected candidate.
     */
    virtual int performPricingPhase1() = 0;

    /**
     * This function performs and controlls the phase 2 dual pricing.
     * Pure virtual function should be implemented in the derived classes.
     * @return the variable index of the selected candidate.
     */
    virtual int performPricingPhase2() = 0;

    /**
     * Returns the selected reduced cost value.
     * @return DualPricing::m_reducedCost.
     */
    inline Numerical::Double getReducedCost() const {return m_reducedCost;}

    /**
     * Function used at checking optimality, dual unboundedness.
     * @return true if there is any locked variable index.
     */
    bool hasLockedVariable()const;

    /**
     * Getter of the measure of primal infeasibility.
     * @return The DualPricing::m_primalInfeasibility member.
     */
    inline Numerical::Double getPrimalInfeasibility()const{return m_primalInfeasibility;}

    /**
     * Clears and reinitializes the m_used member.
     * Releases all locked variables if called. For more information see DualPricing::lockLastIndex().
     */
    void releaseUsed();

    /**
     * Locks the last variable index so this improving variable can not be selected again.
     * Until it is released its index is stored in the m_used vector.
     * @throws NumericalException if invalid row index (-1) is given.
     */
    void lockLastIndex();

    /**
     * Pricing updating function, for details see DualDevexPricing::update().
     */
    virtual void update(int incomingIndex,
                        int outgoingIndex,
                        const Vector & incomingAlpha,
                        const Vector & pivotRow,
                        const Vector & pivotRowOfBasisInverse) = 0;

    /**
     * Checks the updated data.
     */
    virtual void checkAndFix() = 0;

    /**
     * Initializing function.
     */
    void init() {}
protected:

    /**
     * X_B vector.
     */
    const Vector & m_basicVariableValues;

    /**
     * Pointer of the indexlist containing the basicvariable feasibilities.
     */
    IndexList<> * m_basicVariableFeasibilities;

    /**
     * Indexlist containing the feasibility sets of the reduced costs.
     */
    const IndexList<> & m_reducedCostFeasibilities;

    /**
     * Vector containing the basicvariable indices.
     */
    const std::vector<int> & m_basisHead;

    /**
     * Model of the LP problem.
     */
    const SimplexModel & m_simplexModel;

    /**
     * The basis containing the basic vectors.
     */
    const Basis & m_basis;

    /**
     * The selected reduced cost value.
     */
    Numerical::Double m_reducedCost;

    /**
     * Vector storing the locked variables.
     * See DualPricing::lockLastIndex().
     */
    std::vector<char> m_used;

    /**
     * The selected outgoing variable.
     */
    int m_outgoingIndex;

    /**
     * The selected phase 2 reduced cost.
     */
    Numerical::Double m_phase2ReducedCost;

    /**
     * The phase 1 simpri framework.
     */
    Simpri m_phase1Simpri;

    /**
     * The phase 2 simpri framework.
     */
    Simpri m_phase2Simpri;

    /**
     * Array of the phase 1 reduced costs.
     */
    Numerical::Double * m_phase1ReducedCosts;


    /**
     * Array of the phase 1 reduced cost summarizers.
     */
    Numerical::Summarizer * m_phase1ReducedCostSummarizers;

    /**
     * Measure of primal infeasibility. (X_B)
     */
    Numerical::Double m_primalInfeasibility;

    //Parameter references

    /**
     * Parameter reference of the run-time parameter "e_optimality",
     * for details see SimplexParameterHandler.
     */
    const double & m_feasibilityTolerance;

    /**
     * Parameter reference of the run-time parameter "simpri_phaseI_clusters",
     * for details see SimplexParameterHandler.
     */
    const int & m_phaseIClusters;

    /**
     * Parameter reference of the run-time parameter "simpri_phaseI_visit_clusters",
     * for details see SimplexParameterHandler.
     */
    const int & m_phaseIVisitClusters;

    /**
     * Parameter reference of the run-time parameter "simpri_phaseI_improving_candidates",
     * for details see SimplexParameterHandler.
     */
    const int & m_phaseIImprovingCandidates;

    /**
     * Parameter reference of the run-time parameter "simpri_phaseII_clusters",
     * for details see SimplexParameterHandler.
     */
    const int & m_phaseIIClusters;

    /**
     * Parameter reference of the run-time parameter "simpri_phaseII_visit_clusters",
     * for details see SimplexParameterHandler.
     */
    const int & m_phaseIIVisitClusters;

    /**
     * Parameter reference of the run-time parameter "simpri_phaseII_improving_candidates",
     * for details see SimplexParameterHandler.
     */
    const int & m_phaseIIImprovingCandidates;


    /**
     * Clears the array of the phase 1 reduced costs.
     */
    void clearPhase1ReducedCosts();

    /**
     * Clears the array of the phase 1 reduced cost summarizers.
     */
    void clearPhase1ReducedCostSummarizers();

    /**
     * Initializer function for phase 1.
     * This function computes the phase 1 reduced cost values.
     */
    void initPhase1();

    /**
     * Initializing function for phase 2.
     * This function computes the feasibilities of the basicvariables.
     * This is necessary because there is no primal feasibility checker module in the dual simplex.
     */
    void initPhase2();
};

#endif	/* DUALPRICING_H */

