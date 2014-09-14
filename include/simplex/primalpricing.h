/**
 * @file primalpricing.h This file contains the API of the PrimalPricing class.
 */

#ifndef PRIMALPRICING_H
#define	PRIMALPRICING_H

#include <globals.h>
#include <utils/indexlist.h>
#include <simplex/basis.h>
#include <simplex/pricing/simpri.h>
#include <simplex/pricing.h>

class SimplexModel;

/**
 * Describes the primal pricing module.
 * It determines an improving incoming variable. Different pricing strategies can be derived from the class.
 * @class PrimalPricing
 */
class PrimalPricing: public Pricing {
public:

    /**
     * Initializing constructor of the class
     * @param basicVariableValues the X_B values
     * @param basicVariableFeasibilities the feasibility states of the X_B values
     * @param reducedCostFeasibilities the feasibility states of the reduced costs
     * @param variableStates indexlist holding variable states
     * @param basisHead containing basicvariable indices
     * @param simplexModel model of the LP problem
     * @param basis the B basis
     * @param reducedCosts vector of the reduced costs
     * @constructor
     */
    PrimalPricing(const Vector & basicVariableValues,
                  const IndexList<> & basicVariableFeasibilities,
                  IndexList<> * reducedCostFeasibilities,
                  const IndexList<const Numerical::Double*> & variableStates,
                  const std::vector<int> & basisHead,
                  const SimplexModel & model,
                  const Basis & basis,
                  const Vector & reducedCosts);

    /**
     * Default destructor of the class.
     * @destructor
     */
    virtual ~PrimalPricing();

    /**
     * Pure virtual function performing and controlling the phase 1 primal pricing.
     * @return variable index of the chosen variable
     */
    virtual int performPricingPhase1() = 0;

    /**
     * Pure virtual function performing and controlling the phase 2 primal pricing.
     * @return variable index of the chosen variable
     */
    virtual int performPricingPhase2() = 0;

    /**
     * Returns the selected reduced cost value.
     * @return PrimalPricing::m_reducedCost.
     */
    inline Numerical::Double getReducedCost() const {return m_reducedCost;}

    /**
     * Function used at checking optimality, primal unboundedness.
     * @return true if there is any locked variable index.
     */
    bool hasLockedVariable()const;

    /**
     * Getter of the measure of dual infeasibility.
     * @return The DualPricing::m_dualInfeasibility member.
     */
    inline Numerical::Double getDualInfeasibility()const{return m_dualInfeasibility;}

    /**
     * Clears and reinitializes the m_used member.
     * Releases all locked variables if called. For more information see PrimalPricing::lockLastIndex().
     */
    void releaseUsed();

    /**
     * Locks the last variable index so this improving variable can not be selected again.
     * Until it is released its index is stored in the m_used vector.
     * @throws NumericalException if invalid column index (-1) is given.
     */
    void lockLastIndex();

    /**
     * Pricing updating pure virtual function.
     */
    virtual void update(int incomingIndex,
                        int outgoingIndex,
                        const Vector * incomingAlpha,
                        const Vector * pivotRow) = 0;

    /**
     * Initializing function of the simpri modules.
     */
    void init();
protected:

    /**
     * X_B vector.
     */
    const Vector & m_basicVariableValues;

    /**
     * The indexlist containing basicvariable feasibilities.
     */
    const IndexList<> & m_basicVariableFeasibilities;

    /**
     * Indexlist containing the feasibility sets of the reduced costs.
     */
    IndexList<> * m_reducedCostFeasibilities;

    /**
     * Indexlist of the variable states.
     */
    const IndexList<const Numerical::Double*> & m_variableStates;

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
     * Vector storing the reduced costs.
     */
    const Vector & m_reducedCosts;

    /**
     * Currently unused.
     */
    int m_phase2Index;

    /**
     * The selected phase 2 reduced cost.
     */
    Numerical::Double m_phase2ReducedCost;

    /**
     * Vector storing the locked variables.
     * See PrimalPricing::lockLastIndex().
     */
    std::vector<char> m_used;

    /**
     * The incoming variable selected by the pricing.
     */
    int m_incomingIndex;

    /**
     * Vector of the negative reduced costs.
     * By separating the positive and negative values numerical stability is increased.
     */
    std::vector<Numerical::Double> m_negativeSums;

    /**
     * Vector of the positive reduced costs.
     * By separating the positive and negative values numerical stability is increased.
     */
    std::vector<Numerical::Double> m_positiveSums;

    /**
     * Vector of the phase 1 reduced costs.
     */
    std::vector<Numerical::Double> m_phase1ReducedCosts;

    /**
     * Phase 1 simpri module.
     */
    Simpri m_phase1Simpri;

    /**
     * Phase 2 simpri module.
     */
    Simpri m_phase2Simpri;

    /**
     * Measure of dual infeasibility. (d_j)
     */
    Numerical::Double m_dualInfeasibility;

    //Parameter references

    /**
     * Parameter reference of the run-time parameter "e_feasibility",
     * for details see SimplexParameterHandler.
     */
    const double & m_feasibilityTolerance;

    /**
     * Parameter reference of the run-time parameter "e_feasibility",
     * for details see SimplexParameterHandler.
     */
    const double & m_optimalityTolerance;

    /**
     * Parameter reference of the run-time parameter "e_optimality",
     * for details see SimplexParameterHandler.
     */
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
     * This function computes the phase 1 reduced costs.
     */
    void initPhase1();
};


#endif	/* PRIMALPRICING_H */

