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
 * @file simplexcontroller.cpp
 */
#include <simplex/simplexcontroller.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/pfibasis.h>
#include <simplex/lubasis.h>
#include <utils/thread.h>
#include <thread>
#include <simplex/simplexthread.h>

const static char * ITERATION_TIME_NAME = "Time";
const static char * ITERATION_INVERSION_NAME = "Inv";

const static char * SOLUTION_ITERATION_NAME = "Total iterations";
const static char * SOLUTION_SOLVE_TIMER_NAME = "Total solution time";

const static char * EXPORT_PHASE1_ITERATION = "export_phase1_iteration";
const static char * EXPORT_PHASE1_TIME = "export_phase1_time";
const static char * EXPORT_ITERATION = "export_iteration";
const static char * EXPORT_TIME = "export_time";

const static char * EXPORT_TRIGGERED_REINVERSION = "export_triggered_reinversion";

Timer SimplexController::sm_solveTimer;
SimplexController::SimplexController():
    m_primalSimplex(NULL),
    m_dualSimplex(NULL),
    m_currentSimplex(NULL),
    m_iterationIndex(0),
    m_phase1Iteration(-1),
    m_phase1Time(0.0),
    m_freshBasis(false),
    //Parameter references
    m_debugLevel(SimplexParameterHandler::getInstance().getIntegerParameterValue("Global.debug_level")),
    m_enableParallelization(SimplexParameterHandler::getInstance().getBoolParameterValue("Parallel.enable_parallelization")),
    m_enableThreadSynchronization(SimplexParameterHandler::getInstance().getBoolParameterValue("Parallel.enable_thread_synchronization")),
    m_numberOfThreads(SimplexParameterHandler::getInstance().getIntegerParameterValue("Parallel.number_of_threads")),
    m_saveBasis(SimplexParameterHandler::getInstance().getBoolParameterValue("Global.SaveBasis.basis")),
    m_saveFilename(SimplexParameterHandler::getInstance().getStringParameterValue("Global.SaveBasis.filename")),
    m_saveLastBasis(SimplexParameterHandler::getInstance().getBoolParameterValue("Global.SaveBasis.last_basis")),
    m_loadBasis(SimplexParameterHandler::getInstance().getBoolParameterValue("Global.LoadBasis.basis")),
    m_loadFilename(SimplexParameterHandler::getInstance().getStringParameterValue("Global.LoadBasis.filename")),
    m_loadFormat(SimplexParameterHandler::getInstance().getStringParameterValue("Global.LoadBasis.format")),
    m_exportType(SimplexParameterHandler::getInstance().getStringParameterValue("Global.Export.type")),
    m_enableExport(SimplexParameterHandler::getInstance().getBoolParameterValue("Global.Export.enable")),
    m_exportFilename(SimplexParameterHandler::getInstance().getStringParameterValue("Global.Export.filename")),
    m_triggeredReinversion(0),
    m_iterations(0),
    m_isOptimal(false),
    m_minIndex(0)
{
    std::string factorizationType = SimplexParameterHandler::getInstance().getStringParameterValue("Factorization.type");
    if (factorizationType == "PFI"){
        m_basis = new PfiBasis();
    } else if (factorizationType == "LU"){
        m_basis = new LuBasis();
    } else {
        LPERROR("Wrong parameter: factorization_type");
        throw ParameterException("Wrong factorization type parameter");
    }
}

SimplexController::~SimplexController()
{
    if (m_primalSimplex){
        delete m_primalSimplex;
        m_primalSimplex = NULL;
    }
    if (m_dualSimplex){
        delete m_dualSimplex;
        m_dualSimplex = NULL;
    }
    if (m_currentSimplex){
        m_currentSimplex = NULL;
    }
    if(m_basis){
        delete m_basis;
        m_basis = 0;
    }
}

std::vector<IterationReportField> SimplexController::getIterationReportFields(
        enum ITERATION_REPORT_FIELD_TYPE & type) const {
    std::vector<IterationReportField> result;

    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
    {
        IterationReportField timeField(ITERATION_TIME_NAME, 10, 1, IterationReportField::IRF_RIGHT,
                                       IterationReportField::IRF_FLOAT, *this,
                                       4, IterationReportField::IRF_FIXED);
        result.push_back(timeField);
        IterationReportField inversionField(ITERATION_INVERSION_NAME, 4, 1, IterationReportField::IRF_RIGHT,
                                            IterationReportField::IRF_STRING, *this);
        result.push_back(inversionField);
        break;
    }

    case IterationReportProvider::IRF_SOLUTION:
    {
        IterationReportField iterationField(SOLUTION_ITERATION_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                            IterationReportField::IRF_INT, *this);
        result.push_back(iterationField);
        IterationReportField solveTimerField(SOLUTION_SOLVE_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                             IterationReportField::IRF_FLOAT, *this,
                                             4, IterationReportField::IRF_FIXED);
        result.push_back(solveTimerField);
        break;
    }

    case IterationReportProvider::IRF_EXPORT:
    {
        // Parameter study export set
        if(m_exportType == "PARAMETER_STUDY"){
            result.push_back(IterationReportField(EXPORT_TRIGGERED_REINVERSION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_PHASE1_ITERATION, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_PHASE1_TIME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  6, IterationReportField::IRF_FIXED));
            result.push_back(IterationReportField(EXPORT_ITERATION, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_TIME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  6, IterationReportField::IRF_FIXED));
            // Ratio test research set
        } else if (m_exportType == "RATIOTEST_STUDY") {
            result.push_back(IterationReportField(EXPORT_TRIGGERED_REINVERSION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_PHASE1_ITERATION, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_PHASE1_TIME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  6, IterationReportField::IRF_FIXED));
            result.push_back(IterationReportField(EXPORT_ITERATION, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_TIME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  6, IterationReportField::IRF_FIXED));

        } else if (m_exportType == "REVISION_CHECK") {
            result.push_back(IterationReportField(EXPORT_ITERATION, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_TIME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  6, IterationReportField::IRF_FIXED));

        } else {
            throw ParameterException("Invalid export type specified in the parameter file!");
        }
        break;
    }
    default:
        break;
    }

    return result;
}

Entry SimplexController::getIterationEntry(const string &name, ITERATION_REPORT_FIELD_TYPE &type) const {
    Entry reply;

    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
    {
        if (name == ITERATION_TIME_NAME) {
            reply.m_double = sm_solveTimer.getCPURunningTime();
        } else if (name == ITERATION_INVERSION_NAME) {
            if (m_freshBasis) {
                reply.m_string = new std::string("*I*");
            } else {
                reply.m_string = new std::string("");
            }
        }

        break;
    }
    case IterationReportProvider::IRF_SOLUTION:
        if (name == SOLUTION_ITERATION_NAME) {
            reply.m_integer = m_iterationIndex;
        } else if (name == SOLUTION_SOLVE_TIMER_NAME) {
            reply.m_double = sm_solveTimer.getCPUTotalElapsed();
        }
        break;

    case IterationReportProvider::IRF_EXPORT:
        if(name == EXPORT_TRIGGERED_REINVERSION){
            reply.m_integer = m_triggeredReinversion;
        } else if(name == EXPORT_PHASE1_TIME){
            reply.m_double = m_phase1Time;
        } else if(name == EXPORT_PHASE1_ITERATION){
            reply.m_integer = m_phase1Iteration;
        } else if(name == EXPORT_ITERATION){
            reply.m_integer = m_iterationIndex;
        } else if(name == EXPORT_TIME){
            reply.m_double = sm_solveTimer.getCPUTotalElapsed();
        }
        break;

    default:
        break;
    }

    return reply;
}

const Numerical::Double &SimplexController::getObjectiveValue() const
{
    return m_currentAlgorithm == Simplex::PRIMAL ? m_primalSimplex->getObjectiveValue() :
                                                   m_dualSimplex->getObjectiveValue();
}

const Numerical::Double &SimplexController::getPhaseIObjectiveValue() const
{
    return m_currentAlgorithm == Simplex::PRIMAL ? m_primalSimplex->getPhaseIObjectiveValue() :
                                                   m_dualSimplex->getPhaseIObjectiveValue();
}

const std::vector<int> &SimplexController::getBasisHead() const
{
    return m_currentAlgorithm == Simplex::PRIMAL ? m_primalSimplex->getBasisHead() :
                                                   m_dualSimplex->getBasisHead();
}

const DenseVector SimplexController::getPrimalSolution() const
{
    return m_currentAlgorithm == Simplex::PRIMAL ? m_primalSimplex->getPrimalSolution() :
                                                   m_dualSimplex->getPrimalSolution();
}

const std::vector<Numerical::Double> SimplexController::getDualSolution() const
{
    return m_currentAlgorithm == Simplex::PRIMAL ? m_primalSimplex->getDualSolution() :
                                                   m_dualSimplex->getDualSolution();
}

const DenseVector SimplexController::getReducedCosts() const
{
    return m_currentAlgorithm == Simplex::PRIMAL ? m_primalSimplex->getReducedCosts() :
                                                   m_dualSimplex->getReducedCosts();
}

void SimplexController::solve(const Model &model)
{
    if (SimplexParameterHandler::getInstance().getStringParameterValue("Global.starting_algorithm") == "PRIMAL") {
        m_currentAlgorithm = Simplex::PRIMAL;
        m_primalSimplex = new PrimalSimplex(m_basis);
        m_currentSimplex = m_primalSimplex;
        LPINFO("Solving problem with primal simplex method...");
    } else if (SimplexParameterHandler::getInstance().getStringParameterValue("Global.starting_algorithm") == "DUAL") {
        m_currentAlgorithm = Simplex::DUAL;
        m_dualSimplex = new DualSimplex(m_basis);
        m_currentSimplex = m_dualSimplex;
        LPINFO("Solving problem with dual simplex method...");
    }
    m_isOptimal = false;
    m_basis->prepareForModel(model);
    m_iterations = 0;
    if (m_enableParallelization) {
        if(m_enableThreadSynchronization){
            parallelSolve(model);
        }else{
            LPWARNING("Not supported function, set enable_thread_synchronization parameter to true!");
            return;
            ThreadSupervisor::registerMyThread();
            std::vector<std::thread*> threads(m_numberOfThreads, nullptr);
            //spawn threads
            for(int i=0; i < m_numberOfThreads; ++i){
                threads[i] = new std::thread(&SimplexController::parallelSequentialSolve,
                                             this,
                                             &model);
            }
//            LPINFO("Threads spawned!");
            //synchronise threads
            for(int i=0; i < m_numberOfThreads; ++i){
                threads[i]->join();
            }
//            LPINFO("Threads finished!");
            for(int i=0; i < m_numberOfThreads; ++i){
                delete threads[i];
            }
            ThreadSupervisor::unregisterMyThread();
        }
    } else {
        const int & repeatSolution = SimplexParameterHandler::getInstance().getIntegerParameterValue("Global.repeat_solution");
        for(int i=0; i <= repeatSolution; ++i){
            sequentialSolve(model);
        }
    }
    m_basis->releaseModel();
}

void SimplexController::sequentialSolve(const Model &model)
{
    IterationReport* iterationReport = new IterationReport();

    iterationReport->addProviderForStart(*this);
    iterationReport->addProviderForIteration(*this);
    iterationReport->addProviderForSolution(*this);

    if(m_enableExport){
        iterationReport->addProviderForExport(*this);
    }

    ParameterHandler & simplexParameters = SimplexParameterHandler::getInstance();

    const int & iterationLimit = simplexParameters.getIntegerParameterValue("Global.iteration_limit");
    const double & timeLimit = simplexParameters.getDoubleParameterValue("Global.time_limit");
    const std::string & switching = simplexParameters.getStringParameterValue("Global.switch_algorithm");

    const bool & adaptiveInversion = simplexParameters.getBoolParameterValue("Factorization.adaptive_inversion");
    int reinversionFrequency = simplexParameters.getIntegerParameterValue("Factorization.reinversion_frequency");
    unsigned int reinversionCounter = reinversionFrequency;

    try{
        m_currentSimplex->setModel(model);

        m_currentSimplex->setIterationReport(iterationReport);
        m_basis->registerThread();
        m_basis->setSimplexState(m_currentSimplex);

        sm_solveTimer.reset();
        sm_solveTimer.start();
        if (m_loadBasis){
            m_currentSimplex->loadBasis();
        }else{
            m_currentSimplex->findStartingBasis();
        }

        iterationReport->addProviderForStart(*m_currentSimplex);
        iterationReport->addProviderForIteration(*m_currentSimplex);
        iterationReport->addProviderForSolution(*m_currentSimplex);
        if(m_enableExport){
            iterationReport->addProviderForExport(*m_currentSimplex);
        }
        iterationReport->createStartReport();
        iterationReport->writeStartReport();

        Numerical::Double lastObjective = 0;
        int checkCounter = 10;
        //Simplex iterations
        for (m_iterationIndex = 0; m_iterationIndex <= iterationLimit &&
             (sm_solveTimer.getCPURunningTime()) < timeLimit;) {

            if(m_saveBasis){
                m_currentSimplex->saveBasis(m_iterationIndex);
            }

            //if wolfe is active no inversion should be done (reordering the basishead causes undefined behaviour in the procedure)
            if((int)reinversionCounter >= reinversionFrequency && !((m_currentAlgorithm == Simplex::PRIMAL &&
                                                                   m_primalSimplex->m_ratiotest != NULL) ?
                    m_primalSimplex->m_ratiotest->isWolfeActive() :
                 (m_currentAlgorithm == Simplex::DUAL && m_dualSimplex->m_ratiotest != NULL) ?
                    (m_dualSimplex->m_ratiotest->isWolfeActive()) : false) ){

                int checkFrequency = 1;//check at every checkFrequency^th inversion
                if (adaptiveInversion && m_iterationIndex >= reinversionFrequency &&
                        checkCounter >= checkFrequency) {
                    std::vector<Numerical::Double> iterations = m_currentSimplex->getIterationTimes();
                    if (m_predictedTimes.size() > 0) {
//                        LPINFO("Actual time: "<<m_currentSimplex->getLastIterationTime());
                        if (Numerical::fabs(m_currentSimplex->getLastIterationTime() - m_predictedTimes[m_minIndex]) > 0.01 ) {
                            LPWARNING("Inaccurate predicted time, diff: "<<Numerical::fabs(m_currentSimplex->getLastIterationTime() - m_predictedTimes[m_minIndex]));
                        }
                    }
                    m_predictedTimes.clear();
                    int minFrequency = reinversionFrequency / 2;
                    int maxFrequency = reinversionFrequency * 2;
                    m_predictedTimes.resize(maxFrequency);
                    m_minIndex = minFrequency;
                    int minOfBiggers = reinversionFrequency;
                    Numerical::Double lastNonzero = 0.001;
                    for(int i = minFrequency; i < maxFrequency; ++i) {
                        if (i < reinversionFrequency) {//decreasing reinv. freq.
                            lastNonzero = iterations[i] > 0 ? iterations[i] : lastNonzero;
                            m_predictedTimes[i] = (reinversionFrequency / (i+1)) * (m_currentSimplex->getLastInversionTime() + lastNonzero);
                            if (m_predictedTimes[i] <= m_predictedTimes[m_minIndex]) {
                                m_minIndex = i;
                            }
                        } else {//increasing reinv. freq.
                            lastNonzero = iterations[i-reinversionFrequency] > 0 ? iterations[i-reinversionFrequency] : lastNonzero;
                            Numerical::Double t_lastIteration = iterations[reinversionFrequency-1] > 0 ? iterations[reinversionFrequency-1] : lastNonzero;
                            Numerical::Double inversion = (double)reinversionFrequency / (double)(i+1);
                            m_predictedTimes[i] = std::floor(inversion * (m_currentSimplex->getLastInversionTime() + t_lastIteration + lastNonzero) * 1000)/1000;
                            if (m_predictedTimes[i] <= m_predictedTimes[minOfBiggers]) {
                                minOfBiggers = i;
                            }
                        }
//                        if(i == reinversionFrequency-1) LPINFO("*");
//                        LPINFO(m_predictedTimes[i]);
                    }
//                    LPINFO("diff "<<m_predictedTimes[minOfBiggers] - m_predictedTimes[m_minIndex]);
                    Numerical::Double timeTolerance = 1.0;
                    m_minIndex = m_predictedTimes[m_minIndex] * timeTolerance < m_predictedTimes[minOfBiggers] ? m_minIndex : minOfBiggers;
                    reinversionFrequency = 1 + m_minIndex;
//                    LPINFO("Choosen inversion frequency: "<<reinversionFrequency<<" predicted time: "<<m_predictedTimes[m_minIndex]);
                }
                if (checkCounter == 10) {
                    checkCounter = 0;
                } else {
                    ++checkCounter;
                }

                m_currentSimplex->reinvert();
                reinversionCounter = 0;
                m_freshBasis = true;

                if (switching == "SWITCH_BEFORE_INV") {
                    if (m_iterationIndex > 1){
                        switchAlgorithm(model, iterationReport);
                    }
                } else if (switching == "SWITCH_BEFORE_INV_PH2") {
                    if (!m_currentSimplex->m_lastFeasible && m_currentSimplex->m_feasible){
                        switchAlgorithm(model, iterationReport);
                    }
                } else if (switching == "SWITCH_WHEN_NO_IMPR") {
                    if(m_iterationIndex > 1){
                        if(!m_currentSimplex->m_feasible && m_currentSimplex->getPhaseIObjectiveValue() == lastObjective){
                            switchAlgorithm(model, iterationReport);
                        }else if(m_currentSimplex->m_feasible && m_currentSimplex->getObjectiveValue() == lastObjective){
                            switchAlgorithm(model, iterationReport);
                        }
                    }
                }
                m_currentSimplex->computeFeasibility();
            }
            try{
                //iterate
                m_currentSimplex->iterate(m_iterationIndex);

                m_iterations++;

                if(!m_currentSimplex->m_feasible){
                    lastObjective = m_currentSimplex->getPhaseIObjectiveValue();
                }else{
                    lastObjective = m_currentSimplex->getObjectiveValue();
                }
                reinversionCounter++;

                m_iterationIndex++;
                if(m_debugLevel>1 || (m_debugLevel==1 && m_freshBasis)){
                    iterationReport->writeIterationReport();
                }
                m_freshBasis = false;
            } catch ( const FallbackException & exception ) {
#ifndef NDEBUG
                LPINFO("Fallback detected in the ratio test: " << exception.getMessage());
#endif
                reinversionCounter = reinversionFrequency;
            } catch ( const OptimizationResultException & exception ) {
                m_currentSimplex->reset();
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    throw;
                } else {
                    reinversionCounter = reinversionFrequency;
                }
            } catch ( const NumericalException & exception ) {
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    throw;
                } else {
                    LPINFO("Numerical error: "<<exception.getMessage());
                    reinversionCounter = reinversionFrequency;
                }
            }
        }
    } catch ( const ParameterException & exception ) {
        LPERROR("Parameter error: "<<exception.getMessage());
    } catch ( const OptimalException & exception ) {
        m_isOptimal = true;
        LPINFO("OPTIMAL SOLUTION found for "<<m_currentSimplex->getModel().getName()<<"!");
        LPINFO("Total inv time: "<<m_currentSimplex->totalInversionTime());
        LPINFO("Total iter time: "<<m_currentSimplex->totalIterationTime());
//        Numerical::Double avgInv = m_currentSimplex->totalInversionTime() / m_currentSimplex->getInversionCounter();
//        Numerical::Double avgIter = m_currentSimplex->totalIterationTime() / m_iterations;
//        LPWARNING("AVG inv time: "<<avgInv);
//        LPWARNING("AVG iter time: "<<avgIter);
//        std::vector<std::pair<Numerical::Double, Numerical::Double> > times = m_currentSimplex->getTimeValues();
//        std::pair<Numerical::Double, Numerical::Double> deviation;
//        deviation.first = 0.0;
//        deviation.second = 0.0;
//        for(unsigned i = 0; i < times.size(); ++i){
//            deviation.first += pow(times[i].first - avgInv, 2);
//            deviation.second += pow(times[i].second - avgIter, 2);
//        }
//        deviation.first /= times.size();
//        deviation.second /= times.size();
//        deviation.first = sqrt(deviation.first);
//        deviation.second = sqrt(deviation.second);
//        LPINFO("Inv dev: "<<deviation.first);
//        LPINFO("Iter dev: "<<deviation.second);

        // TODO: postsovle, post scaling
        // TODO: Save optimal basis if necessary
    } catch ( const PrimalInfeasibleException & exception ) {
        LPINFO("The problem is PRIMAL INFEASIBLE.");
    } catch ( const DualInfeasibleException & exception ) {
        LPINFO("The problem is DUAL INFEASIBLE.");
    } catch ( const PrimalUnboundedException & exception ) {
        LPINFO("The problem is PRIMAL UNBOUNDED.");
    } catch ( const DualUnboundedException & exception ) {
        LPINFO("The problem is DUAL UNBOUNDED.");
    } catch ( const NumericalException & exception ) {
        LPINFO("Numerical error: "<<exception.getMessage());
    } catch ( const SyntaxErrorException & exception ) {
        exception.show();
    } catch ( const InvalidIndexException & exception ) {
        LPERROR("InvalidIndexException exception:"<< exception.getMessage() );
    } catch ( const PanOptException & exception ) {
        LPERROR("PanOpt exception:"<< exception.getMessage() );
    } catch ( const std::bad_alloc & exception ) {
        LPERROR("STL bad alloc exception: " << exception.what() );
    } catch ( const std::bad_cast & exception ) {
        LPERROR("STL bad cast exception: " << exception.what() );
    } catch ( const std::bad_exception & exception ) {
        LPERROR("STL bad exception exception: " << exception.what() );
    } catch ( const std::bad_typeid & exception ) {
        LPERROR("STL bad typeid exception: " << exception.what() );
    } catch (const std::ios_base::failure & exception ) {
        LPERROR("STL ios_base::failure exception: " << exception.what() );
    } catch (const std::logic_error & exception ) {
        LPERROR("STL logic error exception: " << exception.what() );
    } catch ( const std::range_error & exception ) {
        LPERROR("STL range error: " << exception.what() );
    } catch ( const std::overflow_error & exception ) {
        LPERROR("STL arithmetic overflow error: " << exception.what() );
    } catch ( const std::underflow_error & exception ) {
        LPERROR("STL arithmetic underflow error: " << exception.what() );
#if __cplusplus > 199711L
    } catch ( const std::system_error & exception ) {
        //LPERROR("STL system error: \"" << exception.code().message() << "\" " << exception.what() );
        LPERROR("STL system error: " << std::endl);
        LPERROR("\tError: " << exception.what() << std::endl);
        LPERROR("\tCode: " << exception.code().value() << std::endl);
        LPERROR("\tCategory: " << exception.code().category().name() << std::endl);
        LPERROR("\tMessage: " << exception.code().message() << std::endl);
    } catch ( const std::bad_function_call & exception ) {
        LPERROR("STL bad function call exception: " << exception.what() );
#endif
    } catch (const std::runtime_error & exception ) {
        LPERROR("STL runtime error: " << exception.what() );
    }
    catch (const std::exception & exception) {
        LPERROR("General STL exception: " << exception.what());
    } catch (...) {
        LPERROR("Unknown exception");
    }
    sm_solveTimer.stop();

    iterationReport->createSolutionReport();
    iterationReport->writeSolutionReport();

    if(m_enableExport){
        iterationReport->createExportReport();
        iterationReport->writeExportReport(m_exportFilename);
    }

    m_basis->releaseThread();

    if(iterationReport){
        delete iterationReport;
        iterationReport = NULL;
    }
}

void SimplexController::parallelSolve(const Model &model)
{
    ThreadSupervisor::registerMyThread();

    ParameterHandler & simplexParameters = SimplexParameterHandler::getInstance();

    //The master thread guides the inversion
    int masterIndex=0;

    const int & iterationLimit = simplexParameters.getIntegerParameterValue("Global.iteration_limit");
    const double & timeLimit = simplexParameters.getDoubleParameterValue("Global.time_limit");
    const int & reinversionFrequency = simplexParameters.getIntegerParameterValue("Factorization.reinversion_frequency");

    if (simplexParameters.getStringParameterValue("Global.starting_algorithm") == "PRIMAL") {
        m_currentAlgorithm = Simplex::PRIMAL;
    } else if (simplexParameters.getStringParameterValue("Global.starting_algorithm") == "DUAL") {
        m_currentAlgorithm = Simplex::DUAL;
    }

    //prepare simplex objects
    std::vector<SimplexThread> simplexThreads;
    std::vector<Simplex *> simplexes;
    simplexes.reserve(m_numberOfThreads);
    simplexThreads.reserve(m_numberOfThreads);
    for(int i=0; i < m_numberOfThreads; ++i){
        if(m_currentAlgorithm == Simplex::PRIMAL){
            simplexes.push_back(new PrimalSimplex(m_basis));
        }else{
            simplexes.push_back(new DualSimplex(m_basis));
        }
        simplexThreads.emplace_back(simplexes[i]);
    }

    m_basis->registerThread();

    sm_solveTimer.reset();
    sm_solveTimer.start();

    std::vector<IterationReport*> iterationReports(m_numberOfThreads, nullptr);
    try{
        for(int i=0; i < m_numberOfThreads; ++i){
            iterationReports[i] = new IterationReport();

            iterationReports[i]->addProviderForStart(*this);
            iterationReports[i]->addProviderForIteration(*this);
            iterationReports[i]->addProviderForSolution(*this);

            if(m_enableExport){
                iterationReports[i]->addProviderForExport(*this);
            }

            simplexes[i]->setModel(model);

            simplexes[i]->setIterationReport(iterationReports[i]);

            iterationReports[i]->addProviderForStart(*simplexes[i]);
            iterationReports[i]->addProviderForIteration(*simplexes[i]);
            iterationReports[i]->addProviderForSolution(*simplexes[i]);

            iterationReports[masterIndex]->createStartReport();
            iterationReports[masterIndex]->writeStartReport();
        }

        //Only one starting basis finding
        if (m_loadBasis){
            simplexes[masterIndex]->loadBasis();
        }else{
            simplexes[masterIndex]->findStartingBasis();
        }

        //Simplex iterations
        for (m_iterationIndex = 0; m_iterationIndex <= iterationLimit &&
             (sm_solveTimer.getCPURunningTime()) < timeLimit;) {

            //invert and set the simplex states to follow the master simplex
            m_basis->setSimplexState(simplexes[masterIndex]);
            simplexes[masterIndex]->reinvert();
            for(int i=0; i<m_numberOfThreads; i++){
                if(i != masterIndex){
                    simplexes[i]->setSimplexState(*(simplexes[masterIndex]));
                }
            }

            std::thread* threads = new std::thread [m_numberOfThreads];
            //spawn threads
            for(int i=0; i < m_numberOfThreads; ++i){
                threads[i] = std::thread(&SimplexThread::performIterations,
                                 &simplexThreads[i],
                                 m_basis, iterationReports[i], m_iterationIndex, reinversionFrequency);
            }
//            LPINFO("Threads spawned!");
            //synchronise threads
            for(int i=0; i < m_numberOfThreads; ++i){
                threads[i].join();
            }
//            LPINFO("Threads finished!");
            delete [] threads;

            //Select the master simplex
            //If all simplexes are iterated normally, choose the best one
            //If at least one has reached the optimum, choose that one and reset model
            //If all are terminated, choose one with the maximal iteration number
            int maxPhase1Index = -1;
            Numerical::Double maxPhase1 = -Numerical::Infinity;
            int maxPhase2Index = -1;
            Numerical::Double maxPhase2;
            if(m_currentAlgorithm == Simplex::PRIMAL) {
                maxPhase2 = Numerical::Infinity;
            } else {
                maxPhase2 = -Numerical::Infinity;
            }
            int terminatedNumber = 0;
            int terminatedMaxIndex = -1;
            int terminatedMaxIterations = -1;
            int finishedIndex = -1;
            for(int i=0; i<m_numberOfThreads; i++){
                switch (simplexThreads[i].getResult()){
                case SimplexThread::ITERATED:
                    if(!simplexes[i]->isFeasible()){
                        //In phase 1 max is better
                        if(simplexes[i]->getPhaseIObjectiveValue() > maxPhase1){
                            maxPhase1Index = i;
                            maxPhase1 = simplexes[i]->getPhaseIObjectiveValue();
                        }
                    } else {
                        if(m_currentAlgorithm == Simplex::PRIMAL){
                            //In phase 2 PRIMAL - min is better
                            if(simplexes[i]->getObjectiveValue() < maxPhase2){
                                maxPhase2Index = i;
                                maxPhase2 = simplexes[i]->getObjectiveValue();
                            }
                        } else {
                            //In phase 2 DUAL - max is better
                            if(simplexes[i]->getObjectiveValue() > maxPhase2){
                                maxPhase2Index = i;
                                maxPhase2 = simplexes[i]->getObjectiveValue();
                            }
                        }
                    }
                    break;

                case SimplexThread::FINISHED:
                    finishedIndex = i;
//                    i=m_numberOfThreads;
                    break;

                case SimplexThread::TERMINATED:
                    terminatedNumber++;
                    if(simplexThreads[i].getIterationNumber() > terminatedMaxIterations){
                        terminatedMaxIndex = i;
                        terminatedMaxIterations = simplexThreads[i].getIterationNumber();
                    }
                    break;
                }
            }
            if(finishedIndex != -1){
                masterIndex = finishedIndex;
                for(int i=0; i<m_numberOfThreads; i++){
                    simplexes[i]->reset();
                }
                //Throw the finished exception if the basis was fresh
                if(m_iterationIndex == simplexThreads[masterIndex].getIterationNumber()){
                    switch (simplexThreads[masterIndex].getExceptionType()){
                    case SimplexThread::OPTIMAL:
                        throw *(static_cast<OptimalException*>(simplexThreads[masterIndex].getException()));
                        break;
                    case SimplexThread::PRIMAL_INFEASIBLE:
                        throw *(static_cast<PrimalInfeasibleException*>(simplexThreads[masterIndex].getException()));
                        break;
                    case SimplexThread::PRIMAL_UNBOUNDED:
                        throw *(static_cast<PrimalUnboundedException*>(simplexThreads[masterIndex].getException()));
                        break;
                    case SimplexThread::DUAL_INFEASIBLE:
                        throw *(static_cast<DualInfeasibleException*>(simplexThreads[masterIndex].getException()));
                        break;
                    case SimplexThread::DUAL_UNBOUNDED:
                        throw *(static_cast<DualUnboundedException*>(simplexThreads[masterIndex].getException()));
                        break;
                    case SimplexThread::NUMERICAL:
                        throw *(static_cast<NumericalException*>(simplexThreads[masterIndex].getException()));
                        break;
                    case SimplexThread::FALLBACK:
                        throw *(static_cast<FallbackException*>(simplexThreads[masterIndex].getException()));
                        break;
                    }
                }
            } else if (terminatedNumber == m_numberOfThreads){
                masterIndex = terminatedMaxIndex;
                //Throw the error exception if the basis was fresh
                if(m_iterationIndex == simplexThreads[masterIndex].getIterationNumber()){
                    switch (simplexThreads[masterIndex].getExceptionType()){
                    case SimplexThread::OPTIMAL:
                        throw *(static_cast<OptimalException*>(simplexThreads[masterIndex].getException()));
                        break;
                    case SimplexThread::PRIMAL_INFEASIBLE:
                        throw *(static_cast<PrimalInfeasibleException*>(simplexThreads[masterIndex].getException()));
                        break;
                    case SimplexThread::PRIMAL_UNBOUNDED:
                        throw *(static_cast<PrimalUnboundedException*>(simplexThreads[masterIndex].getException()));
                        break;
                    case SimplexThread::DUAL_INFEASIBLE:
                        throw *(static_cast<DualInfeasibleException*>(simplexThreads[masterIndex].getException()));
                        break;
                    case SimplexThread::DUAL_UNBOUNDED:
                        throw *(static_cast<DualUnboundedException*>(simplexThreads[masterIndex].getException()));
                        break;
                    case SimplexThread::NUMERICAL:
                        throw *(static_cast<NumericalException*>(simplexThreads[masterIndex].getException()));
                        break;
                    case SimplexThread::FALLBACK:
                        throw *(static_cast<FallbackException*>(simplexThreads[masterIndex].getException()));
                        break;
                    }
                }
            } else {
                if(maxPhase2Index == -1){
                    //All threads are in phase 1
                    masterIndex = maxPhase1Index;
                } else {
                    //At least on thread is in phase 2
                    masterIndex = maxPhase2Index;
                }
            }
//            LPINFO("master: "<<masterIndex+1);
            m_iterationIndex = simplexThreads[masterIndex].getIterationNumber();
        }
    } catch ( const ParameterException & exception ) {
        LPERROR("Parameter error: "<<exception.getMessage());
    } catch ( const OptimalException & exception ) {
        m_isOptimal = true;
        LPINFO("OPTIMAL SOLUTION found! ");
        // TODO: postsovle, post scaling
        // TODO: Save optimal basis if necessary
    } catch ( const PrimalInfeasibleException & exception ) {
        LPINFO("The problem is PRIMAL INFEASIBLE.");
    } catch ( const DualInfeasibleException & exception ) {
        LPINFO("The problem is DUAL INFEASIBLE.");
    } catch ( const PrimalUnboundedException & exception ) {
        LPINFO("The problem is PRIMAL UNBOUNDED.");
    } catch ( const DualUnboundedException & exception ) {
        LPINFO("The problem is DUAL UNBOUNDED.");
    } catch ( const NumericalException & exception ) {
        LPINFO("Numerical error: "<<exception.getMessage());
    } catch ( const FallbackException & exception ) {
        LPINFO("Fallback error: "<<exception.getMessage());
    } catch ( const SyntaxErrorException & exception ) {
        exception.show();
    } catch ( const InvalidIndexException & exception ) {
        LPERROR("InvalidIndexException exception:"<< exception.getMessage() );
    } catch ( const PanOptException & exception ) {
        LPERROR("PanOpt exception:"<< exception.getMessage() );
    } catch ( const std::bad_alloc & exception ) {
        LPERROR("STL bad alloc exception: " << exception.what() );
    } catch ( const std::bad_cast & exception ) {
        LPERROR("STL bad cast exception: " << exception.what() );
    } catch ( const std::bad_exception & exception ) {
        LPERROR("STL bad exception exception: " << exception.what() );
    } catch ( const std::bad_typeid & exception ) {
        LPERROR("STL bad typeid exception: " << exception.what() );
    } catch (const std::ios_base::failure & exception ) {
        LPERROR("STL ios_base::failure exception: " << exception.what() );
    } catch (const std::logic_error & exception ) {
        LPERROR("STL logic error exception: " << exception.what() );
    } catch ( const std::range_error & exception ) {
        LPERROR("STL range error: " << exception.what() );
    } catch ( const std::overflow_error & exception ) {
        LPERROR("STL arithmetic overflow error: " << exception.what() );
    } catch ( const std::underflow_error & exception ) {
        LPERROR("STL arithmetic underflow error: " << exception.what() );
#if __cplusplus > 199711L
    } catch ( const std::system_error & exception ) {
        //LPERROR("STL system error: \"" << exception.code().message() << "\" " << exception.what() );
        LPERROR("STL system error: " << std::endl);
        LPERROR("\tError: " << exception.what() << std::endl);
        LPERROR("\tCode: " << exception.code().value() << std::endl);
        LPERROR("\tCategory: " << exception.code().category().name() << std::endl);
        LPERROR("\tMessage: " << exception.code().message() << std::endl);
    } catch ( const std::bad_function_call & exception ) {
        LPERROR("STL bad function call exception: " << exception.what() );
#endif
    } catch (const std::runtime_error & exception ) {
        LPERROR("STL runtime error: " << exception.what() );
    }
    catch (const std::exception & exception) {
        LPERROR("General STL exception: " << exception.what());
    } catch (...) {
        LPERROR("Unknown exception");
    }
    sm_solveTimer.stop();

    iterationReports[masterIndex]->createSolutionReport();
    iterationReports[masterIndex]->writeSolutionReport();

    //TODO: Export should follow the master simplex every time
    if(m_enableExport){
        iterationReports[masterIndex]->createExportReport();
        iterationReports[masterIndex]->writeExportReport(m_exportFilename);
    }

    for(int i=0; i<m_numberOfThreads; i++){
        delete simplexes[i];
        simplexes[i] = nullptr;
        delete iterationReports[i];
        iterationReports[i] = nullptr;
    }

    m_basis->releaseThread();
    ThreadSupervisor::unregisterMyThread();
}

void SimplexController::parallelSequentialSolve(const Model *model) {
#ifndef CLASSIC_NEW_DELETE
    MemoryManager::startParallel();
#endif
    SimplexController* wrapper = new SimplexController();
    ThreadSupervisor::registerMyThread();
    InitPanOpt::threadInit();

    wrapper->sequentialSolve(*model);

    InitPanOpt::threadRelease();
    ThreadSupervisor::unregisterMyThread();
    delete wrapper;
}

void SimplexController::switchAlgorithm(const Model &model, IterationReport* iterationReport)
{
    LPWARNING("Switching algorithm during the solution is not supported yet!");
    return;
    //init algorithms to be able to switch
    if (m_primalSimplex == NULL){
        m_primalSimplex = new PrimalSimplex(m_basis);
        m_primalSimplex->setModel(model);
        m_primalSimplex->setIterationReport(iterationReport);
    }
    if (m_dualSimplex == NULL){
        m_dualSimplex = new DualSimplex(m_basis);
        m_dualSimplex->setModel(model);
        m_dualSimplex->setIterationReport(iterationReport);
    }
    //Primal->Dual
    if (m_currentAlgorithm == Simplex::PRIMAL){
        iterationReport->removeIterationProvider(*m_currentSimplex);
        m_currentSimplex = m_dualSimplex;
        iterationReport->addProviderForIteration(*m_currentSimplex);
        m_currentSimplex->setSimplexState(*m_primalSimplex);
        m_currentAlgorithm = Simplex::DUAL;
        //Dual->Primal
    }else{
        iterationReport->removeIterationProvider(*m_currentSimplex);
        m_currentSimplex = m_primalSimplex;
        iterationReport->addProviderForIteration(*m_currentSimplex);
        m_currentSimplex->setSimplexState(*m_dualSimplex);
        m_currentAlgorithm = Simplex::PRIMAL;
    }
    //reinvert
    //TODO: It is not appropriate to reinvert here!
    //Use setSimplexState of the basis instead (probably)
    m_currentSimplex->reinvert();
    m_freshBasis = true;
#ifndef NDEBUG
    LPINFO("Algorithm switched!");
#endif
}
