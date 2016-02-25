//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

/**
  * @file simplexcontroller.h This file contains the API of the SimplexController class.
  */
#ifndef SIMPLEXCONTROLLER_H
#define SIMPLEXCONTROLLER_H

#include <globals.h>
#include <utils/iterationreportprovider.h>
#include <simplex/primalsimplex.h>
#include <simplex/dualsimplex.h>
#include <simplex/simplexstate.h>

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
     * Function that solves the LP problem according to the parameterization.
     *
     * @param model The LP problem model to be solved.
     */
    void solve(const Model & model);

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

    /**
     * Getter of the phase 2 objective value according to the current algorithm.
     *
     * @return phase 2 objective value.
     */
    const Numerical::Double &getObjectiveValue() const;

    /**
     * Getter of the phase 1 objective value according to the current algorithm.
     *
     * @return phase 1 objective value.
     */
    const Numerical::Double &getPhaseIObjectiveValue() const;

    /**
     * Getter of the basishead that containts the basic variable indicies.
     *
     * @return vector Basis head containing the basic variable indices.
     */
    const std::vector<int> & getBasisHead() const;

    /**
     * This function returns with the primal solution vector x.
     *
     * @return the x primal solution vector.
     */
    const DenseVector getPrimalSolution() const;

    /**
     * This function returns with the dual solution vector y.
     *
     * @return the y dual solution vector.
     */
    const std::vector<Numerical::Double> getDualSolution() const;

    /**
     * This function returns with the reduced costs vector.
     *
     * @return the y dual solution vector.
     */
    const DenseVector getReducedCosts() const;

    /**
     * Getter of the solveTimer member.
     * @return SimplexController::sm_solveTimer
     */
    static const Timer& getSolveTimer(){return sm_solveTimer;}

    /**
     * This function returns with the number of iterations.
     * @return The number of iterations.
     */
    unsigned int getIterationCount() const { return m_iterations; }

    /**
     * Returns whether the solution is optimal or not.
     */
    inline bool isOptimal() const { return m_isOptimal; }

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
     * Pointer to the nonsingular matrix: the basis.
     */
    Basis* m_basis;

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
    double m_phase1Time;

    /**
     * This boolean variable is true, if the basis has been inverted in the current iteration.
     */
    bool m_freshBasis;

    /**
     * The Timer object computes the solution time of the solver algorithm.
     */
    static Timer sm_solveTimer;

    //Parameter values
    /**
     * Parameter reference for "debug_level" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const int & m_debugLevel;

    /**
     * Parameter reference for "enable_parallelization" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const bool & m_enableParallelization;

    /**
     * Parameter reference for "enable_thread_synchronization" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const bool & m_enableThreadSynchronization;

    /**
     * Parameter reference for "number_of_threads" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const int & m_numberOfThreads;

    /**
     * Parameter reference for "save_basis" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const bool & m_saveBasis;

    /**
     * Parameter reference for "save_filename" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const std::string & m_saveFilename;

    /**
     * Parameter reference for "save_last_basis" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const bool & m_saveLastBasis;

    /**
     * Parameter reference for "load_basis" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const bool & m_loadBasis;

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

    /**
     * Parameter reference for "export_type" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const std::string & m_exportType;

    /**
     * Parameter reference for "enable_export" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const bool & m_enableExport;

    /**
     * Parameter reference for "export_filename" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const std::string & m_exportFilename;

    /**
     * Counter for the triggered reinversions. (eg. numerical issues)
     */
    int m_triggeredReinversion;

    /**
     * Counter for the iterations.
     */
    unsigned int m_iterations;

    /**
     * Optimality flag for the solution.
     */

    bool m_isOptimal;

    /**
     * For testing the warm start algorithm.
     */
    SimplexState * m_simplexState;

    /**
     * Function that solves the LP problem given by the parameter using a main thread.
     *
     * @param model The LP problem model to be solved.
     */
    void sequentialSolve(const Model& model);

    /**
     * Function that solves the LP problem given by the parameter using synchronized threads.
     *
     * @param model The LP problem model to be solved.
     */
    void parallelSolve(const Model& model);

    /**
     * Function that solves the LP problem given by the parameter using unsychronized threads.
     *
     * @param model The LP problem model to be solved.
     */
    void parallelSequentialSolve(const Model* model);

    /**
     * Function solving the LP problem with warm start (algorithm is started from arbitary basis).
     *
     * @param model The LP problem model to be solved.
     * @param simplexState The simplex state describes the arbitrary basis to start the simplex algorithm from.
     */
    void solveWithWarmStart(const Model& model, SimplexState* simplexState = NULL);

    /**
     * This function can perform a switching among the solver algorithms (Simplex::ALGORITHM)
     *
     * @param model The model of the LP problem.
     */
    void switchAlgorithm(const Model &model, IterationReport *iterationReport);
};

#endif // SIMPLEXCONTROLLER_H
