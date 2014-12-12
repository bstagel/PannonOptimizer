/**
 * @file simplex.h This file containts the API of the Simplex class.
 */


#ifndef SIMPLEX_H
#define	SIMPLEX_H

#include <globals.h>

#include <lp/method.h>
#include <simplex/simplexmodel.h>
#include <utils/numerical.h>
#include <utils/indexlist.h>
#include <utils/iterationreport.h>
#include <utils/iterationreportprovider.h>
#include <utils/timer.h>
#include <simplex/pricing.h>

class SimplexController;
class StartingBasisFinder;
class Basis;
class BasisHeadIO;

/**
 * This class describes a general simplex object. It has all elements of the simplex algorithm:
 * the pricing, ratiotest and other abstract members.
 *
 * @class Simplex
 */
class Simplex : public Method, public IterationReportProvider
{
    friend class Basis;
    friend class BasisHeadIO;
    friend class Checker;
    friend class SimplexController;
    friend class SimplexThread;
public:

    /**
     * Descibes three different feasibility types.
     * It depends on the sign, and the feasibility range of the investigated object. (variable, reduced cost)
     */
    enum FEASIBILITY
    {
        FEASIBLE = 0,
        MINUS,
        PLUS,
        FEASIBILITY_ENUM_LENGTH
    };

    /**
     * This indicates the two solver algorithm: the primal and the dual simplex.
     */
    enum ALGORITHM
    {
        PRIMAL = 0,
        DUAL
    };

    /**
     * This enum describes the possible variable states.
     * The basic variables, and the nonbasic variables at certain bounds.
     */
    enum VARIABLE_STATE
    {
        BASIC = 0,
        NONBASIC_AT_LB,
        NONBASIC_AT_UB,
        NONBASIC_FIXED,
        NONBASIC_FREE,
        VARIABLE_STATE_ENUM_LENGTH
        // Do not modify the order of the enum!
    };


    /**
     * Default constructor of the class.
     *
     * @constructor
     */
    Simplex(Basis* basis);

    /**
     * Default destructor of the class.
     *
     * @destructor
     */
    virtual ~Simplex();

    /**
     * Getter of the phase1Iteration member.
     * @return Simplex::m_phase1Iteration
     */
    int getPhase1Iteration()const{return m_phase1Iteration;}
protected:

    /**
     * This counts the simplex iterations.
     */
    int m_iterationIndex;

    /**
     * The number of iterations spent in phase 1 before entering phase 2 for the first time.
     */
    int m_phase1Iteration;

    /**
     * Time spent by the solver in phase 1.
     */
    Numerical::Double m_phase1Time;

    // Interface of the iteration report provider
    /**
     * The function returns the iteration report fields of this class.
     *
     * @see IterationReportProvider.
     */
    std::vector<IterationReportField> getIterationReportFields(enum ITERATION_REPORT_FIELD_TYPE & type) const;

    /**
     * Getter of one iteration Entry.
     *
     * @see IterationReportProvider.
     */
    Entry getIterationEntry(const std::string & name,
                            enum ITERATION_REPORT_FIELD_TYPE & type) const;

    /**
     * Pointer to the iteration reporter.
     */
    IterationReport * m_iterationReport;

    /**
     * The simplex model of the LP problem.
     */
    SimplexModel * m_simplexModel;

    /**
     * The basis head containing the variable indices of the basic variables.
     */
    std::vector<int> m_basisHead;

    /**
     * Indicates wheter we are in a degenerate situation or not.
     */
    bool m_degenerate;

    /**
     * Index list storing the variable states described at Simplex::VARIABLE_STATE.
     * The template parameter holds the value of the variable.
     */
    IndexList<const Numerical::Double*> m_variableStates;

    /**
     * Index list containts the feasibilities of the basic variables.
     * Its possible values can be found at Simplex::FEASIBILITY.
     */
    IndexList<> m_basicVariableFeasibilities;

    /**
     * Index list containts the feasibilities of the reduced costs..
     * Its possible values can be found at Simplex::FEASIBILITY.
     */
    IndexList<> m_reducedCostFeasibilities;

    /**
     * Vector containing the basic variable values (X_B vector).
     */
    DenseVector m_basicVariableValues;

    /**
     * Vector of the reduced costs (primal d_j values).
     */
    DenseVector m_reducedCosts;

    /**
     * The phase 2 objective function value of the LP problem.
     */
    Numerical::Double m_objectiveValue;

    /**
     * The phase 1 objective function value of the LP problem: the sum of infeasibilities.
     */
    Numerical::Double m_phaseIObjectiveValue;

    /**
     * Alpha column vector corresponding to the incoming variable.
     */
    DenseVector m_pivotColumn;

    /**
     * Alpha row vector corresponding to the outgoing variable.
     */
    DenseVector m_pivotRow;

    /**
     * Variable index of the incoming candidate.
     */
    int m_incomingIndex;

    /**
     * The outgoing index is the variable's location in the basishead.
     * The variable index can be derived as: basishead[outgoingindex].
     */
    int m_outgoingIndex;

    /**
     * A simple boolean variable describing the feasibility of the problem.
     */
    bool m_feasible;

    /**
     * A simple boolean variable describing the actual iteration (Phase I or Phase II).
     */
    bool m_feasibleIteration;

    /**
     * Describes whether the basis in the previous iteration was feasible or not.
     */
    bool m_lastFeasible;

    /**
     * Indicates whether the basis was changed or not.
     */
    bool m_baseChanged;

    /**
     * Computes the time of the basis inversions.
     */
    Timer m_inversionTimer;

    /**
     * Measures the time of computing the basic solution.
     */
    Timer m_computeBasicSolutionTimer;

    /**
     * Measures the time of computing the reduced costs.
     */
    Timer m_computeReducedCostsTimer;

    /**
     * Computes the time that the pricing takes.
     */
    Timer m_priceTimer;

    /**
     * Computes the time that the ratiotest takes.
     */
    Timer m_selectPivotTimer;

    /**
     * Measures the time that the updating takes.
     */
    Timer m_updateTimer;

    //Parameter variables
    /**
     * Parameter reference of the run-time parameter "reinversion_frequency".
     *
     * @see SimplexParameterHandler
     */
    const int & m_reinversionFrequency;

    /**
     * Parameter reference of the run-time parameter "enable_export".
     *
     * @see SimplexParameterHandler
     */
    const bool & m_enableExport;

    /**
     * Parameter reference of the run-time parameter "export_filename".
     *
     * @see SimplexParameterHandler
     */
    const std::string & m_exportFilename;

    /**
     * Parameter reference of the run-time parameter "export_type".
     *
     * @see SimplexParameterHandler
     */
    const std::string & m_exportType;

    /**
     * Parameter reference of the run-time parameter "save_format".
     *
     * @see SimplexParameterHandler
     */
    const std::string & m_saveFormat;

    /**
     * Parameter reference of the run-time parameter "save_iteration".
     *
     * @see SimplexParameterHandler
     */
    const int & m_saveIteration;

    /**
     * Parameter reference of the run-time parameter "save_periodically".
     *
     * @see SimplexParameterHandler
     */
    const int & m_savePeriodically;

    /**
     * Parameter reference for "save_filename" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const std::string & m_saveFilename;

    /**
     * Parameter reference for "load_filename" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const std::string & m_loadFilename;

    /**
     * Parameter reference for "load_format" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const std::string & m_loadFormat;

    //Tolerance handling
    /**
     * Master tolerance given in the parameter file.
     */
    Numerical::Double m_masterTolerance; //Initialized in child classes

    /**
     * The tolerance multiplier is needed in the expand procedure.
     * Its value is given in the simplex parameter file.
     */
    Numerical::Double m_toleranceMultiplier; //Initialized in child classes

    /**
     * The tolerance divider is needed in the expand procedure.
     * Its value is given in the simplex parameter file.
     */
    Numerical::Double m_toleranceDivider; //Initialized in child classes

    /**
     * The tolerance step is the measure of the tolerance increment in every iteration in the expand procedure.
     * It can be computed as: (masterTolerance - masterTolerance * toleranceMultiplier) / toleranceDivider.
     */
    Numerical::Double m_toleranceStep;

    /**
     * The actual working tolerance.
     * Its value is incremented by the toleranceStep in every iteration during the expand procedure.
     */
    Numerical::Double m_workingTolerance;

    //Measure variables
    /**
     * Reference objective value to be compared with the computed objective value.
     * Degenerate iterations, bad iterations can be discovered thi way.
     */
    Numerical::Double m_referenceObjective;

    /**
     * Number of fallbacks to phase 1.
     */
    int m_fallbacks;

    /**
     * Number of iterations, where the objective value got worse.
     */
    int m_badIterations;

    /**
     * Number of iterations, where the objective value was the same.
     */
    int m_degenerateIterations;

    //Modules
    /**
     * The starting basis finder modul ensures a starting ponint for the algorithm.
     */
    StartingBasisFinder* m_startingBasisFinder;

    /**
     * Pointer to the nonsingular matrix: the basis.
     */
    Basis* m_basis;

    /**
     * Pointer to the pricing, object initialized in child classes.
     */
    Pricing * m_pricing;

    /**
     * Parameter reference of run-time parameter "expand",
     * see SimplexParameterHandler for details.
     */
    const std::string & m_expand;

    /**
     * Trigger for recomputation of the reduced costs at the next reinversion
     */
    bool m_recomputeReducedCosts;

    /**
     * Performs the starting basis finding according to the strategy given in the simplex parameter file.
     */
    void findStartingBasis();

    /**
     * This function performs one simplex iteration. (pricing, ratiotest, update)
     * @param iterationIndex The index of the actual iteration. Important for reporting
     */
    void iterate(int iterationIndex);

    /**
     * Setter for the iteration reporter.
     *
     * @param iterationReport Pointer to the IterationReport object to be set.
     */
    inline void setIterationReport(IterationReport * iterationReport){ m_iterationReport = iterationReport;}

    /**
     * Returns whether the current solution is feasible or not.
     *
     * @return Simplex::m_feasible
     */
    inline bool isFeasible()const{return m_feasible;}

    /**
     * Returns whether the last basis was feasible or not.
     *
     * @return Simplex::m_lastFeasible
     */
    inline bool getLastFeasible()const{return m_lastFeasible;}

    /**
     * Description can be found at SimplexController::getObjectiveValue().
     */
    inline const Numerical::Double & getObjectiveValue() const {return m_objectiveValue;}

    /**
     * Description can be found in SimplexController::getPhaseIObjectiveValue().
     */
    inline const Numerical::Double & getPhaseIObjectiveValue() const {return m_phaseIObjectiveValue;}

    /**
     * Description can be found in SimplexController::getBasisHead().
     */
    inline const std::vector<int> & getBasisHead() const {return m_basisHead;}

    /**
     * Description can be found in SimplexController::getPrimalSolution().
     */
    const DenseVector getPrimalSolution() const;

    /**
     * Description can be found in SimplexController::getDualSolution().
     */
    const std::vector<Numerical::Double> getDualSolution() const;

    /**
     * Setter for the mathematical model of the LP problem.
     *
     * @param model is the other Model object
     */
    void setModel(const Model & model);

    /**
     * This function saves the current state of the simplex algorithm, and copies it to the Simplex object given in parameter.
     *
     * @param simplex The target Simplex object to copy the state to.
     */
    void setSimplexState(const Simplex &simplex);

    /**
     * Saves the current basis to a file.
     *
     * @param fileName Name of the file where the basis will be saved.
     * @param basisWriter The bases writer performing the basis writing.
     * @param releaseWriter Optional parameter for releasing the basisWriter.
     */
    void saveBasisToFile(const char * fileName, BasisHeadIO * basisWriter, bool releaseWriter);

    /**
     * Loads the basis from a file.
     *
     * @param fileName Name of the file where the basis will be loaded from.
     * @param basisReader Basis reader performing the basis reading.
     * @param releaseWriter Optional parameter for releasing the basisReader.
     */
    void loadBasisFromFile(const char * fileName, BasisHeadIO * basisReader, bool releaseReader);

    /**
     * Calls the saveBasisToFile() function, if needed.
     * If the iteration index given as parameter corresponds to the reinversion frequency
     * given in the simplex parameter file or the current iteration is to be saved (also from simplex parameterfile).
     *
     * @param iteratonIndex Index of the current iteration.
     * @param threadIndex Index of the current thread (-1 in sequential mode).
     *
     * @throws ParameterException at invalid save basis format (read from parameterfile).
     */
    void saveBasis(int iterationIndex, int threadIndex = -1);

    /**
     * Calls the loadBasisFromFile() function, if needed.
     *
     * @throws ParameterException at invalid load basis format (read from parameterfile).
     */
    void loadBasis();

    /**
     * Performs a reinversion on the basis, computing the basic solution, the reduced costs and the feasibility.
     * It also releases the variable indices locked since the last inverson. (eg. numerical reasons)
     */
    void reinvert();

    /**
     * This function computes the basic solution from the rhs and the cost vector.
     * It's called in every reinversion.
     */
    void computeBasicSolution();

    /**
     * Computes the reduced costs from the cost vector.
     * The function is called in every reinversion.
     */
    void computeReducedCosts();

    /**
     * Pure virtual function for computing the feasibility, implemented in child classes.
     */
    virtual void computeFeasibility() = 0;

    /**
     * Pure virtual function for the pricing module.
     */
    virtual void price() = 0;

    /**
     * Pure virtual function for the ratiotest module.
     */
    virtual void selectPivot() = 0;

    /**
     * Pure virtual function for updating the solution.
     */
    virtual void update() = 0;

    /**
     * Pure virtual function for updating the reduced costs.
     */
    virtual void updateReducedCosts() = 0;

    /**
     * Pure virtual setter for the reference objective value.
     */
    virtual void setReferenceObjective(bool secondPhase) = 0;

    /**
     * Pure virtual function to check the reference objective value.
     */
    virtual void checkReferenceObjective(bool secondPhase) = 0;

    /**
     * Pure virtual function for initializing the working tolerance.
     *
     * @see Simplex::m_workingTolerance
     */
    virtual void initWorkingTolerance() = 0;

    /**
     * Pure virtual function for computing the working tolerance in every iteration.
     *
     * @see Simplex::m_workingTolerance
     */
    virtual void computeWorkingTolerance() = 0;

    /**
     * Pure virtual function for releasing the locked variables (eg. because numerical issues).
     */
    virtual void releaseLocks() = 0;

    /**
     * This function performs the basis change according to the parameters (incoming, outgoing
     * variables). The value of the incoming variable is determined by the pirmalTheta parameter.
     * This function also performs the boundflips according to the third parameter.
     *
     * @param incomingIndex Index of the incoming variable.
     * @param outgoingIndex Index of the outgoing variable.
     * @param boundflips Indices of the variables on which a boundflip operation shall be performed.
     * @param primalTheta The value of the incoming variable in the new basis.
     */
    void transform(int incomingIndex,
                   int outgoingIndex,
                   const std::vector<unsigned int>& boundflips,
                   Numerical::Double* primalTheta);

    void registerIntoIterationReport(const IterationReportProvider & provider);

    Numerical::Double sensitivityAnalysisRhs() const;

    void reset();

    virtual void resetTolerances() = 0;
};

#endif /* SIMPLEX_H */
