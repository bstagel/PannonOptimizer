/**
 * @file simplexcontroller.cpp
 */
#include <simplex/simplexcontroller.h>
#include <simplex/simplexparameterhandler.h>
#include <prettyprint.h>

const static char * ITERATION_INDEX_NAME = "Iteration";
const static char * ITERATION_TIME_NAME = "Time";
const static char * ITERATION_INVERSION_NAME = "Inv";

const static char * SOLUTION_ITERATION_NAME = "Total iterations";
const static char * SOLUTION_SOLVE_TIMER_NAME = "Total solution time";

const static char * EXPORT_ITERATION = "export_iteration";
const static char * EXPORT_TIME = "export_time";

const static char * EXPORT_TRIGGERED_REINVERSION = "export_triggered_reinversion";

SimplexController::SimplexController():
    m_primalSimplex(NULL),
    m_dualSimplex(NULL),
    m_currentSimplex(NULL),
    m_freshBasis(false),
    //Parameter references
    m_debugLevel(SimplexParameterHandler::getInstance().getIntegerParameterValue("debug_level")),
    m_saveBasis(SimplexParameterHandler::getInstance().getBoolParameterValue("save_basis")),
    m_saveFilename(SimplexParameterHandler::getInstance().getStringParameterValue("save_filename")),
    m_saveLastBasis(SimplexParameterHandler::getInstance().getBoolParameterValue("save_last_basis")),
    m_loadBasis(SimplexParameterHandler::getInstance().getBoolParameterValue("load_basis")),
    m_loadFilename(SimplexParameterHandler::getInstance().getStringParameterValue("load_filename")),
    m_loadFormat(SimplexParameterHandler::getInstance().getStringParameterValue("load_format")),
    m_exportType(SimplexParameterHandler::getInstance().getIntegerParameterValue("export_type")),
    m_enableExport(SimplexParameterHandler::getInstance().getBoolParameterValue("enable_export")),
    m_exportFilename(SimplexParameterHandler::getInstance().getStringParameterValue("export_filename")),
    m_triggeredReinversion(0)
{
    m_iterationReport = new IterationReport();

    m_iterationReport->addProviderForStart(*this);
    m_iterationReport->addProviderForIteration(*this);
    m_iterationReport->addProviderForSolution(*this);

    if(m_enableExport){
        m_iterationReport->addProviderForExport(*this);
    }
}

SimplexController::~SimplexController()
{
    if(m_iterationReport){
        delete m_iterationReport;
        m_iterationReport = NULL;
    }
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
}

std::vector<IterationReportField> SimplexController::getIterationReportFields(
        enum ITERATION_REPORT_FIELD_TYPE & type) const {
    std::vector<IterationReportField> result;

    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
    {
        IterationReportField inversionField(ITERATION_INVERSION_NAME, 3, 1, IterationReportField::IRF_CENTER,
                                            IterationReportField::IRF_STRING, *this);
        result.push_back(inversionField);
        IterationReportField iterationField(ITERATION_INDEX_NAME, 9, 1, IterationReportField::IRF_CENTER,
                                            IterationReportField::IRF_INT, *this);
        result.push_back(iterationField);
        IterationReportField timeField(ITERATION_TIME_NAME, 10, 1, IterationReportField::IRF_RIGHT,
                                       IterationReportField::IRF_FLOAT, *this,
                                       4, IterationReportField::IRF_FIXED);
        result.push_back(timeField);
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
        if(m_exportType == 0){
            result.push_back(IterationReportField(EXPORT_TRIGGERED_REINVERSION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_ITERATION, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_TIME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  6, IterationReportField::IRF_FIXED));
            // Ratio test research set
        } else if (m_exportType == 1) {
            result.push_back(IterationReportField(EXPORT_TRIGGERED_REINVERSION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
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
    reply.m_integer = 0;
    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
        if (name == ITERATION_INDEX_NAME) {
            reply.m_integer = m_iterationIndex;
        } else if (name == ITERATION_TIME_NAME) {
            reply.m_double = m_solveTimer.getCPURunningTime();
        } else if (name == ITERATION_INVERSION_NAME) {
            if(m_freshBasis){
                reply.m_string = new std::string("*I*");
            } else {
                reply.m_string = new std::string("");
            }
        }
        break;

    case IterationReportProvider::IRF_SOLUTION:
        if (name == SOLUTION_ITERATION_NAME) {
            reply.m_integer = m_iterationIndex;
        } else if (name == SOLUTION_SOLVE_TIMER_NAME) {
            reply.m_double = m_solveTimer.getCPUTotalElapsed();
        }
        break;

    case IterationReportProvider::IRF_EXPORT:
        if(name == EXPORT_TRIGGERED_REINVERSION){
            reply.m_integer = m_triggeredReinversion;
        } else if(name == EXPORT_ITERATION){
            reply.m_integer = m_iterationIndex;
        } else if(name == EXPORT_TIME){
            reply.m_double = m_solveTimer.getCPUTotalElapsed();
        }
        break;

    default:
        break;
    }

    return reply;
}

void SimplexController::writeSolutionReport()
{
    m_iterationReport->createSolutionReport();
    m_iterationReport->writeSolutionReport();
    if(m_enableExport){
        m_iterationReport->createExportReport();
        m_iterationReport->writeExportReport(m_exportFilename);
    }
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

const std::vector<Numerical::Double> SimplexController::getPrimalSolution() const
{
    return m_currentAlgorithm == Simplex::PRIMAL ? m_primalSimplex->getPrimalSolution() :
                                                   m_dualSimplex->getPrimalSolution();
}

const std::vector<Numerical::Double> SimplexController::getDualSolution() const
{
    return m_currentAlgorithm == Simplex::PRIMAL ? m_primalSimplex->getDualSolution() :
                                                   m_dualSimplex->getDualSolution();
}

void SimplexController::solve(const Model &model)
{
    ParameterHandler & simplexParameters = SimplexParameterHandler::getInstance();

    const int & iterationLimit = simplexParameters.getIntegerParameterValue("iteration_limit");
    const double & timeLimit = simplexParameters.getDoubleParameterValue("time_limit");
    const int & reinversionFrequency = simplexParameters.getIntegerParameterValue("reinversion_frequency");
    unsigned int reinversionCounter = reinversionFrequency;
    const int & switching = simplexParameters.getIntegerParameterValue("switch_algorithm");

    m_currentAlgorithm = (Simplex::ALGORITHM)simplexParameters.getIntegerParameterValue("starting_algorithm");

    if (m_currentAlgorithm == Simplex::PRIMAL){
        m_primalSimplex = new PrimalSimplex();
        m_currentSimplex = m_primalSimplex;
    }else{
        m_dualSimplex = new DualSimplex();
        m_currentSimplex = m_dualSimplex;
    }

    try{
        m_currentSimplex->setModel(model);
        m_currentSimplex->setSolveTimer(&m_solveTimer);
        m_currentSimplex->setIterationReport(m_iterationReport);
        m_currentSimplex->initModules();
        m_solveTimer.start();
        if (m_loadBasis){
            m_currentSimplex->loadBasis();
        }else{
            m_currentSimplex->findStartingBasis();
        }

        m_iterationReport->addProviderForStart(*m_currentSimplex);
        m_iterationReport->writeStartReport();
        m_iterationReport->addProviderForIteration(*m_currentSimplex);
        m_iterationReport->createStartReport();

        Numerical::Double lastObjective = 0;
        //Simplex iterations
        for (m_iterationIndex = 1; m_iterationIndex <= iterationLimit &&
             (m_solveTimer.getCPURunningTime()) < timeLimit; m_iterationIndex++) {

            if(m_saveBasis){
                m_currentSimplex->saveBasis(m_iterationIndex);
            }

            if((int)reinversionCounter == reinversionFrequency){
                m_currentSimplex->reinvert();
                reinversionCounter = 0;
                m_freshBasis = true;
                switch(switching){
                //every reinversion
                case 1:
                    if (m_iterationIndex > 1){
                        switchAlgorithm(model);
                    }
                    break;
//                //at entering phase-2
                case 2:
                    break;
//                first reinversion in phase-2
                case 3:
                    if (!m_currentSimplex->m_lastFeasible && m_currentSimplex->m_feasible){
                        switchAlgorithm(model);
                    }
                    break;
                //at degeneracy
                case 4:
                    if(m_iterationIndex > 1){
                        if(!m_currentSimplex->m_feasible && m_currentSimplex->getPhaseIObjectiveValue() == lastObjective){
                            switchAlgorithm(model);
                    }else if(m_currentSimplex->m_feasible && m_currentSimplex->getObjectiveValue() == lastObjective){
                            switchAlgorithm(model);
                        }
                    }
                    break;
                }
            }
            try{
            //iterate
                m_currentSimplex->iterate();
                if(!m_currentSimplex->m_feasible){
                    lastObjective = m_currentSimplex->getPhaseIObjectiveValue();
                }else{
                    lastObjective = m_currentSimplex->getObjectiveValue();
                }
                reinversionCounter++;

                if(m_debugLevel>1 || (m_debugLevel==1 && m_freshBasis)){
                    m_iterationReport->createIterationReport();
                    m_iterationReport->writeIterationReport();
                }
                m_freshBasis = false;
            } catch ( const FallbackException & exception ) {
                LPINFO("Fallback detected in the ratio test: " << exception.getMessage());
                m_currentSimplex->reinvert();
                m_iterationIndex--;
            } catch ( const OptimizationResultException & exception ) {
//                LPWARNING("OptimizationResultException cought! "<<m_freshBasis);
                m_currentSimplex->m_simplexModel->resetModel();
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    throw;
                } else {
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            } catch ( const NumericalException & exception ) {
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    throw;
                } else {
                    LPINFO("Numerical error: "<<exception.getMessage());
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            } catch(const SwitchAlgorithmException& exception){
                LPINFO("Algorithm switched: "<< exception.getMessage());
                switchAlgorithm(model);
            }
        }
    } catch ( const ParameterException & exception ) {
        LPERROR("Parameter error: "<<exception.getMessage());
    } catch ( const OptimalException & exception ) {
            LPINFO("OPTIMAL SOLUTION found! ");
            m_solveTimer.stop();
            m_iterationReport->addProviderForSolution(*m_currentSimplex);
            writeSolutionReport();
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
}

void SimplexController::switchAlgorithm(const Model &model)
{
    //init algorithms to be able to switch
    if (m_primalSimplex == NULL){
        m_primalSimplex = new PrimalSimplex();
        m_primalSimplex->setModel(model);
        m_primalSimplex->setSolveTimer(&m_solveTimer);
        m_primalSimplex->setIterationReport(m_iterationReport);
        m_primalSimplex->initModules();
    }
    if (m_dualSimplex == NULL){
        m_dualSimplex = new DualSimplex();
        m_dualSimplex->setModel(model);
        m_dualSimplex->setSolveTimer(&m_solveTimer);
        m_dualSimplex->setIterationReport(m_iterationReport);
        m_dualSimplex->initModules();
    }
    //Primal->Dual
    if (m_currentAlgorithm == Simplex::PRIMAL){
        m_iterationReport->removeIterationProvider(*m_currentSimplex);
        m_currentSimplex = m_dualSimplex;
        m_iterationReport->addProviderForIteration(*m_currentSimplex);
        m_currentSimplex->setSimplexState(*m_primalSimplex);
        m_currentAlgorithm = Simplex::DUAL;
    //Dual->Primal
    }else{
        m_iterationReport->removeIterationProvider(*m_currentSimplex);
        m_currentSimplex = m_primalSimplex;
        m_iterationReport->addProviderForIteration(*m_currentSimplex);
        m_currentSimplex->setSimplexState(*m_dualSimplex);
        m_currentAlgorithm = Simplex::PRIMAL;
    }
    //reinvert
    m_currentSimplex->reinvert();
    m_freshBasis = true;
    LPINFO("Algorithm switched!");
}

