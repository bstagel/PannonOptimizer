/**
  * @file simplexcontroller.h This file contains the API of the SimplexController class.
  */
#ifndef SIMPLEXCONTROLLER_H
#define SIMPLEXCONTROLLER_H

#include <globals.h>
#include <utils/iterationreportprovider.h>
#include <simplex/primalsimplex.h>
#include <simplex/dualsimplex.h>

/**
 * This class controlls the solver, and makes possible to switch between
 * primal and dual iterations.
 *
 * @class SimplexController
 */
class SimplexController: public IterationReportProvider {
public:
    /**
     * Default constructor of the class.
     *
     * @constructor
     */
    SimplexController();
    /**
     * Default Destructor of the class.
     *
     * @destructor
     */
    virtual ~SimplexController();

    /**
     * Function that solves the LP problem given by the parameter.
     * @param model the model to be solved
     */
    void solve(const Model& model);

    /**
     * The function returns the iteration report fields of this class, see IterationReportProvider.
     */
    std::vector<IterationReportField> getIterationReportFields(enum ITERATION_REPORT_FIELD_TYPE & type) const;

    /**
     * Getter of one iteration Entry, see IterationReportProvider.
     */
    Entry getIterationEntry(const std::string & name,
                            enum ITERATION_REPORT_FIELD_TYPE & type) const;

    /**
     * Function that writes the solution report, the result of the algorithm, the objective value etc.
     */
    void writeSolutionReport();

    /**
     * Getter of the phase 2 objective value according to the current algorithm.
     * @return phase 2 objective value.
     */
    const Numerical::Double & getObjectiveValue() const;

    /**
     * Getter of the phase 1 objective value according to the current algorithm.
     * @return phase 1 objective value.
     */
    const Numerical::Double & getPhaseIObjectiveValue() const;

    /**
     * Getter of the basishead that containts the basic variable indicies.
     * @return vector containing the basicvariable indices.
     */
    const std::vector<int> & getBasisHead() const;

    /**
     * This function returns with the primal solution vector x.
     * @return the x primal solution vector.
     */
    const std::vector<Numerical::Double> getPrimalSolution() const;

    /**
     * This function returns with the dual solution vector y.
     * @return the y dual solution vector.
     */
    const std::vector<Numerical::Double> getDualSolution() const;

    /**
     * Record the actual iteration number as phase 1 iterations.
     *
     * @param phase1Time The time spent in phase 1.
     */
    void logPhase1Iteration(Numerical::Double phase1Time);

private:
    /**
     * This shows the current solver algorithm it can be either primal or dual simplex.
     */
    Simplex::ALGORITHM m_currentAlgorithm;

    /**
     * If we want to do primal simplex iterations this points to a PrimalSimplex object.
     * Otherwise it's a null pointer.
     */
    PrimalSimplex * m_primalSimplex;

    /**
     * If we want to do dual simplex iterations this points to a DualSimplex object.
     * Otherwise it's a null pointer.
     */
    DualSimplex * m_dualSimplex;

    /**
     * This points to the current solver algorithm, a Simplex pointer.
     */
    Simplex * m_currentSimplex;

    /**
     * This counts the simplex iterations.
     */
    int m_iterationIndex;

    /**
     * The number of iterations spent in phase 1 before entering phsae 2 for the first time.
     */
    int m_phase1Iteration;

    /**
     * Time spent by the solver in phase 1.
     */
    Numerical::Double m_phase1Time;

    /**
     * This boolean variable is true, if the basis has been inverted in the current iteration.
     */
    bool m_freshBasis;

    /**
     * The Timer object computes the solution time of the solver algorithm.
     */
    Timer m_solveTimer;

    //Parameter values
    /**
     * Parameter reference for "debug_level" run-time parameter,
     * for details see SimplexParameterHandler.
     */
    const int & m_debugLevel;

    /**
     * Parameter reference for "save_basis" run-time parameter,
     * for details see SimplexParameterHandler.
     */
    const bool & m_saveBasis;

    /**
     * Parameter reference for "save_filename" run-time parameter,
     * for details see SimplexParameterHandler.
     */
    const std::string & m_saveFilename;

    /**
     * Parameter reference for "save_last_basis" run-time parameter,
     * for details see SimplexParameterHandler.
     */
    const bool & m_saveLastBasis;

    /**
     * Parameter reference for "load_basis" run-time parameter,
     * for details see SimplexParameterHandler.
     */
    const bool & m_loadBasis;

    /**
     * Parameter reference for "load_filename" run-time parameter,
     * for details see SimplexParameterHandler.
     */
    const std::string & m_loadFilename;

    /**
     * Parameter reference for "load_format" run-time parameter,
     * for details see SimplexParameterHandler.
     */
    const std::string & m_loadFormat;

    /**
     * Parameter reference for "export_type" run-time parameter,
     * for details see SimplexParameterHandler.
     */
    const int & m_exportType;

    /**
     * Parameter reference for "enable_export" run-time parameter,
     * for details see SimplexParameterHandler.
     */
    const bool & m_enableExport;

    /**
     * Parameter reference for "export_filename" run-time parameter,
     * for details see SimplexParameterHandler.
     */
    const std::string & m_exportFilename;

    /**
     * Counter for the triggered reinversions. (eg. numerical issues)
     */
    int m_triggeredReinversion;

    /**
     * Pointer to the iteration reporter.
     */
    IterationReport* m_iterationReport;

    /**
     * This function can perform a switching among the solver algorithms (Simplex::ALGORITHM)
     * @param model is needed at the first switch to initialize the primal and dual modules.
     */
    void switchAlgorithm(const Model &model);
};

#endif // SIMPLEXCONTROLLER_H
