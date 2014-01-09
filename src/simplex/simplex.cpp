/**
 * @file simplex.cpp
 */

#include <simplex/simplex.h>
#include <utils/parameterhandler.h>
#include <utils/exceptions.h>
#include <linalg/linalgparameterhandler.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/startingbasisfinder.h>
#include <simplex/pfibasis.h>
#include <globals.h>
#include <simplex/basisheadbas.h>
#include <simplex/basisheadpanopt.h>
#include <simplex/checker.h>

#include <utils/thirdparty/prettyprint.h>
//#include <qd/qd_real.h>
//#include <qd/fpu.h>

#include <algorithm>

const static char * ITERATION_INDEX_NAME = "Iteration";
const static char * ITERATION_TIME_NAME = "Time";
const static char * ITERATION_INVERSION_NAME = "Inv";

const static char * SOLUTION_ITERATION_NAME = "Total iterations";
const static char * SOLUTION_SOLVE_TIMER_NAME = "Total solution time";
const static char * SOLUTION_INVERSION_TIMER_NAME = "Inversion time";
const static char * SOLUTION_COMPUTE_BASIC_SOLUTION_TIMER_NAME = "Compute basic solution time";
const static char * SOLUTION_COMPUTE_REDUCED_COSTS_TIMER_NAME = "Compute reduced costs time";
const static char * SOLUTION_COMPUTE_FEASIBILITY_TIMER_NAME = "Compute feasibility time";
const static char * SOLUTION_CHECK_FEASIBILITY_TIMER_NAME = "Check feasibility time";
const static char * SOLUTION_PRICE_TIMER_NAME = "Pricing time";
const static char * SOLUTION_SELECT_PIVOT_TIMER_NAME = "Pivot selection time";
const static char * SOLUTION_UPDATE_TIMER_NAME = "Update time";

const static char * EXPORT_FILENAME = "export_filename";
const static char * EXPORT_ITERATION = "export_iteration";
const static char * EXPORT_TIME = "export_time";
const static char * EXPORT_SOLUTION = "export_solution";
const static char * EXPORT_FALLBACK = "export_fallback";
const static char * EXPORT_SINGULARITY = "export_simgulerity";
const static char * EXPORT_TRIGGERED_REINVERSION = "export_triggered_reinversion";
const static char * EXPORT_BAD_ITERATION = "export_bad_iteration";
const static char * EXPORT_E_ABSOLUTE = "export_e_absolute";
const static char * EXPORT_E_RELATIVE = "export_e_relative";
const static char * EXPORT_E_FEASIBILITY = "export_e_feasibility";
const static char * EXPORT_E_OPTIMALITY = "export_e_optimality";
const static char * EXPORT_E_PIVOT = "export_e_pivot";

Simplex::Simplex():
    m_expandingTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("e_optimality")),
    m_simplexModel(NULL),
    m_variableStates(0,0),
    m_variableFeasibilities(0,0),
    m_reducedCostFeasibilities(0,0),
    m_basicVariableValues(0),
    m_reducedCosts(0),
    m_objectiveValue(0),
    m_phaseIObjectiveValue(0),
    m_baseChanged(false),
    m_freshBasis(false),
    m_startingBasisFinder(NULL),
    m_basis(NULL),
    m_iterationIndex(0)
{
    m_basicVariableValues.setSparsityRatio(DENSE);
    m_reducedCosts.setSparsityRatio(DENSE);

//    fpu_fix_start(&m_old_cw);
}

Simplex::~Simplex() {
    releaseModules();

    if(m_simplexModel){
        delete m_simplexModel;
        m_simplexModel = 0;
    }
//    fpu_fix_end(&m_old_cw);
}

std::vector<IterationReportField> Simplex::getIterationReportFields(
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
    }
        break;

    case IterationReportProvider::IRF_SOLUTION:
    {
        IterationReportField iterationField(SOLUTION_ITERATION_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_INT, *this);
        result.push_back(iterationField);
        IterationReportField solveTimerField(SOLUTION_SOLVE_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(solveTimerField);
        IterationReportField inversionTimerField(SOLUTION_INVERSION_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(inversionTimerField);
        IterationReportField computeBasicSolutionTimerField(SOLUTION_COMPUTE_BASIC_SOLUTION_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(computeBasicSolutionTimerField);
        IterationReportField computeReducedCostsField(SOLUTION_COMPUTE_REDUCED_COSTS_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(computeReducedCostsField);
        IterationReportField computeFeasibilityTimerField(SOLUTION_COMPUTE_FEASIBILITY_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(computeFeasibilityTimerField);
        IterationReportField checkFeasibilityTimerField(SOLUTION_CHECK_FEASIBILITY_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(checkFeasibilityTimerField);
        IterationReportField priceTimerField(SOLUTION_PRICE_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(priceTimerField);
        IterationReportField selectpivotTimerField(SOLUTION_SELECT_PIVOT_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(selectpivotTimerField);
        IterationReportField updateTimerField(SOLUTION_UPDATE_TIMER_NAME, 20, 1, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_FLOAT, *this,
                                    4, IterationReportField::IRF_FIXED);
        result.push_back(updateTimerField);
        break;
    }

    case IterationReportProvider::IRF_EXPORT:
    {
        // Parameter study export set
        if(m_exportType == 0){
            result.push_back(IterationReportField(EXPORT_FILENAME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_STRING, *this));
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

Entry Simplex::getIterationEntry(const string &name,
                                                     ITERATION_REPORT_FIELD_TYPE &type) const {
    Entry reply;
    reply.m_integer = 0;
    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
        if (name == ITERATION_INDEX_NAME) {
            reply.m_integer = m_iterationIndex;
        } else if (name == ITERATION_TIME_NAME) {
            reply.m_double = m_solveTimer.getRunningTime()/1000000;
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
            reply.m_double = m_solveTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_INVERSION_TIMER_NAME) {
            reply.m_double = m_inversionTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_COMPUTE_BASIC_SOLUTION_TIMER_NAME) {
            reply.m_double = m_computeBasicSolutionTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_COMPUTE_REDUCED_COSTS_TIMER_NAME) {
            reply.m_double = m_computeReducedCostsTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_COMPUTE_FEASIBILITY_TIMER_NAME) {
            reply.m_double = m_computeFeasibilityTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_CHECK_FEASIBILITY_TIMER_NAME) {
            reply.m_double = m_checkFeasibilityTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_PRICE_TIMER_NAME) {
            reply.m_double = m_priceTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_SELECT_PIVOT_TIMER_NAME) {
            reply.m_double = m_selectPivotTimer.getTotalElapsed()/1000000;
        } else if (name == SOLUTION_UPDATE_TIMER_NAME) {
            reply.m_double = m_updateTimer.getTotalElapsed()/1000000;
        }
        break;

    case IterationReportProvider::IRF_EXPORT:
        if(name == EXPORT_FILENAME){
            reply.m_string = new std::string(m_simplexModel->getName());
        } else if(name == EXPORT_ITERATION){
            reply.m_string = new std::string("N.A.");
        } else if(name == EXPORT_TIME){
            reply.m_string = new std::string("N.A.");
        } else if(name == EXPORT_SOLUTION){
            reply.m_string = new std::string("N.A.");
        } else if(name == EXPORT_FALLBACK){
            reply.m_string = new std::string("N.A.");
        } else if(name == EXPORT_SINGULARITY){
            reply.m_string = new std::string("N.A.");
        } else if(name == EXPORT_TRIGGERED_REINVERSION){
            reply.m_string = new std::string("N.A.");
        } else if(name == EXPORT_BAD_ITERATION){
            reply.m_string = new std::string("N.A.");
        } else if(name == EXPORT_E_ABSOLUTE){
            reply.m_double = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_absolute");
        } else if(name == EXPORT_E_RELATIVE){
            reply.m_double = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_relative");
        } else if(name == EXPORT_E_FEASIBILITY){
            reply.m_double = SimplexParameterHandler::getInstance().getDoubleParameterValue("e_feasibility");
        } else if(name == EXPORT_E_OPTIMALITY){
            reply.m_double = SimplexParameterHandler::getInstance().getDoubleParameterValue("e_optimality");
        } else if(name == EXPORT_E_PIVOT){
            reply.m_double = SimplexParameterHandler::getInstance().getDoubleParameterValue("e_pivot");
    }
        break;

    default:
        break;
    }

    return reply;
}

void Simplex::setModel(const Model &model) {
    if(m_simplexModel){
        delete m_simplexModel;
    }
    unregisterMethodWithModel(this, model);
    m_simplexModel = new SimplexModel(model);
    registerMethodWithModel(this, model);

    //Init the data structures
    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();
    m_basisHead.resize(rowCount, INVALID_POSITION);
    m_variableStates.init(rowCount + columnCount, VARIABLE_STATE_ENUM_LENGTH);
    m_variableFeasibilities.init(rowCount + columnCount, FEASIBILITY_ENUM_LENGTH);
    m_reducedCostFeasibilities.init(rowCount + columnCount, FEASIBILITY_ENUM_LENGTH);
    m_basicVariableValues.reInit(rowCount);
    m_reducedCosts.reInit(rowCount + columnCount);
    //TODO: Ezt ne kelljen ujra beallitani egy init miatt
    m_basicVariableValues.setSparsityRatio(DENSE);
    m_reducedCosts.setSparsityRatio(DENSE);
}

void Simplex::constraintAdded()
{

}

void Simplex::variableAdded()
{

}

void Simplex::saveBasis(const char * fileName, BasisHeadIO * basisWriter, bool releaseWriter) {
    basisWriter->startWrting(fileName, m_simplexModel->getModel() );
    int variableIndex;
    unsigned int position = 0;
    STL_FOREACH(std::vector<int>, m_basisHead, basisHeadIter) {
        variableIndex = *basisHeadIter;
        const Variable * variable = &m_simplexModel->getVariable(variableIndex);
        basisWriter->addBasicVariable( variable, position, variableIndex, m_basicVariableValues.at(position) );
        position++;
    }

    IndexList<const Numerical::Double *>::Iterator iter, iterEnd;
    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_LB, 1);
    for (; iter != iterEnd; iter++) {
        variableIndex = iter.getData();
        const Variable * variable = &m_simplexModel->getVariable(variableIndex);
        // TODO: itt az erteket nem kell majd atadni
        basisWriter->addNonbasicVariable(variable, variableIndex, Simplex::NONBASIC_AT_LB, variable->getLowerBound());
    }

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_UB, 1);
    for (; iter != iterEnd; iter++) {
        variableIndex = iter.getData();
        const Variable * variable = &m_simplexModel->getVariable(variableIndex);
        // TODO: itt az erteket nem kell majd atadni
        basisWriter->addNonbasicVariable(variable, variableIndex, Simplex::NONBASIC_AT_UB, variable->getUpperBound());
    }

    basisWriter->finishWriting();

    if (releaseWriter == true) {
        delete basisWriter;
    }
}

void Simplex::loadBasis(const char * fileName, BasisHeadIO * basisReader, bool releaseReader) {
    const unsigned int variableCount = m_simplexModel->getColumnCount() + m_simplexModel->getRowCount();
    basisReader->setBasisHead(&m_basisHead);
    basisReader->setVariableStateList(&m_variableStates);
    basisReader->startReading(fileName, m_simplexModel->getModel());
    unsigned int index;
    for (index = 0; index < variableCount; index++) {
        basisReader->addVariable(m_simplexModel->getVariable(index));
    }

    basisReader->finishReading();

    if (releaseReader) {
        delete basisReader;
    }
}


void Simplex::solve() {
    initModules();

    ParameterHandler & simplexParameters = SimplexParameterHandler::getInstance();

    const unsigned int iterationLimit = simplexParameters.getIntegerParameterValue("iteration_limit");
    const double timeLimit = simplexParameters.getDoubleParameterValue("time_limit");
    StartingBasisFinder::STARTING_BASIS_STRATEGY startingBasisStratgy =
            (StartingBasisFinder::STARTING_BASIS_STRATEGY)simplexParameters.getIntegerParameterValue("starting_basis");
    unsigned int reinversionFrequency = simplexParameters.getIntegerParameterValue("reinversion_frequency");
    unsigned int reinversionCounter = reinversionFrequency;

    m_saveBasis = simplexParameters.getBoolParameterValue("save_basis");
    m_saveFilename = simplexParameters.getStringParameterValue("save_filename");
    m_saveFormat = simplexParameters.getStringParameterValue("save_format");
    m_saveLastBasis = simplexParameters.getBoolParameterValue("save_last_basis");
    m_saveIteration = simplexParameters.getIntegerParameterValue("save_iteration");
    m_savePeriodically = simplexParameters.getIntegerParameterValue("save_periodically");

    m_loadBasis = simplexParameters.getBoolParameterValue("load_basis");
    m_loadFilename = simplexParameters.getStringParameterValue("load_filename");
    m_loadFormat = simplexParameters.getStringParameterValue("load_format");

    m_enableExport = simplexParameters.getBoolParameterValue("enable_export");
    m_exportFilename = simplexParameters.getStringParameterValue("export_filename");
    m_exportType = simplexParameters.getIntegerParameterValue("export_type");

    m_iterationReport.addProviderForStart(*this);
    m_iterationReport.addProviderForIteration(*this);
    m_iterationReport.addProviderForSolution(*this);

    if(m_enableExport){
        m_iterationReport.addProviderForExport(*this);
    }

    m_debugLevel = SimplexParameterHandler::getInstance().getIntegerParameterValue("debug_level");
    m_iterationReport.setDebugLevel(m_debugLevel);

    m_iterationReport.createStartReport();
    m_iterationReport.writeStartReport();

    LPINFO("-----------------------------------");

    try {
        m_solveTimer.start();
        m_basicVariableValues.prepareForData(m_simplexModel->getRhs().nonZeros(), m_basicVariableValues.length(), DENSE);
        Vector::NonzeroIterator it = m_simplexModel->getRhs().beginNonzero();
        Vector::NonzeroIterator itend = m_simplexModel->getRhs().endNonzero();
        for(; it < itend; it++){
            m_basicVariableValues.newNonZero(*it, it.getIndex());
        }
        m_startingBasisFinder->findStartingBasis(startingBasisStratgy);
        if(m_loadBasis){
            std::string loadFormat = m_loadFormat;
            std::string filename = m_loadFilename;
            filename.append(".").append(m_loadFormat);
            //TODO: Exception when file not exits
            std::transform(loadFormat.begin(),loadFormat.end(),loadFormat.begin(),::toupper);
            if(loadFormat.compare("BAS") == 0){
                loadBasis(filename.c_str(), new BasisHeadBAS, true);
            } else if (loadFormat.compare("PBF") == 0){
                loadBasis(filename.c_str(), new BasisHeadPanOpt, true);
            } else {
                throw ParameterException("Invalid load basis file format!");
            }
        }

//initializing EXPAND tolerance
        Numerical::Double expandStep = 0;
        if (SimplexParameterHandler::getInstance().getIntegerParameterValue("expand_dual_phaseI") == 1) {
            m_expandingTolerance =
                SimplexParameterHandler::getInstance().getDoubleParameterValue("expand_multiplier_dphI") *
                SimplexParameterHandler::getInstance().getDoubleParameterValue("e_optimality");
                expandStep = (1 -
                SimplexParameterHandler::getInstance().getDoubleParameterValue("expand_multiplier_dphI") ) *
                SimplexParameterHandler::getInstance().getDoubleParameterValue("e_optimality") /
                SimplexParameterHandler::getInstance().getIntegerParameterValue("expand_divider_dphI");
        }

        for (m_iterationIndex = 1; m_iterationIndex <= iterationLimit &&
             (m_solveTimer.getRunningTime()/1000000) < timeLimit; m_iterationIndex++) {

            if(m_saveBasis){
                if ((m_iterationIndex == m_saveIteration) ||
                    (m_savePeriodically != 0 && ((m_iterationIndex % m_savePeriodically) == 0) )){
                    stringstream numStream;
                    numStream << m_iterationIndex;
                    std::string saveFormat = m_saveFormat;
                    std::string filename = m_saveFilename;
                    filename.append("_").append(numStream.str()).append(".").append(m_saveFormat);
                    std::transform(saveFormat.begin(),saveFormat.end(),saveFormat.begin(),::toupper);
                    if(saveFormat.compare("BAS") == 0){
                        saveBasis(filename.c_str(), new BasisHeadBAS, true);
                    } else if (saveFormat.compare("PBF") == 0){
                        saveBasis(filename.c_str(), new BasisHeadPanOpt, true);
                    } else {
                        throw ParameterException("Invalid save basis file format!");
                    }
                }
            }

            //incrementing EXPAND tolerance
            if (SimplexParameterHandler::getInstance().getIntegerParameterValue("expand_dual_phaseI") == 1)
            {
                m_expandingTolerance += expandStep;
                 //resetting EXPAND tolerance TODO:atgondolni
                if (m_expandingTolerance >=
                        SimplexParameterHandler::getInstance().getDoubleParameterValue("e_optimality"))
                {
                    m_expandingTolerance =
                            SimplexParameterHandler::getInstance().getDoubleParameterValue("expand_multiplier_dphI") *
                            SimplexParameterHandler::getInstance().getDoubleParameterValue("e_optimality");
                }
            }

            if(reinversionCounter == reinversionFrequency){
                m_freshBasis = true;
                reinversionCounter = 0;
                m_inversionTimer.start();
                reinvert();
                m_inversionTimer.stop();
                m_computeBasicSolutionTimer.start();
                computeBasicSolution();
                m_computeBasicSolutionTimer.stop();
                m_computeReducedCostsTimer.start();
                computeReducedCosts();
                m_computeReducedCostsTimer.stop();
                m_computeFeasibilityTimer.start();
                computeFeasibility();
                m_computeFeasibilityTimer.stop();
            }
            try{
                m_checkFeasibilityTimer.start();
                checkFeasibility();
                m_checkFeasibilityTimer.stop();
                m_priceTimer.start();
                price();
                m_priceTimer.stop();
                m_selectPivotTimer.start();
                selectPivot();
                m_selectPivotTimer.stop();
                m_updateTimer.start();
                //TODO: Do not use the update formulas before reinversion
                //TODO: UGY NEM JO AHOGY VOLT, az append is az update-be van!
//                if(reinversionCounter+1 < reinversionFrequency){
                update();
//                }
                m_updateTimer.stop();

                if(m_debugLevel>1 || (m_debugLevel==1 && m_freshBasis)){
                    m_iterationReport.createIterationReport();
                    m_iterationReport.writeIterationReport();
                }
                m_freshBasis = false;
                reinversionCounter++;
            }
            catch ( const OptimalException & exception ) {
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    m_solveTimer.stop();
                    throw exception;
                } else {
//                    LPINFO("TRIGGERING REINVERSION TO CHECK OPTIMALITY! ");
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            }
            catch ( const PrimalInfeasibleException & exception ) {
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    m_solveTimer.stop();
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            }
            catch ( const DualInfeasibleException & exception ) {
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    m_solveTimer.stop();
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            }
            catch ( const PrimalUnboundedException & exception ) {
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    m_solveTimer.stop();
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            }
            catch ( const DualUnboundedException & exception ) {
                //Check the result with triggering reinversion
                if(m_freshBasis){
                    m_solveTimer.stop();
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            }
            catch ( const NumericalException & exception ) {
                //Check the result with triggering reinversion
                LPINFO("TRIGGERING REINVERSION TO HANDLE NUMERICAL ERROR! ");
                if(m_freshBasis){
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                    m_iterationIndex--;
                }
            }
        }

    } catch ( const OptimalException & exception ) {
        LPINFO("OPTIMAL SOLUTION found! ");
        //TODO: Put this into the solution report
//        LPINFO("The objective value: " << m_objectiveValue);
        // TODO: postsovle, post scaling
        // TODO: Save optimal basis if necessary
    } catch ( const PrimalInfeasibleException & exception ) {
        LPERROR("The problem is PRIMAL INFEASIBLE.");
    } catch ( const DualInfeasibleException & exception ) {
        LPERROR("The problem is DUAL INFEASIBLE.");
    } catch ( const PrimalUnboundedException & exception ) {
        LPERROR("The problem is PRIMAL UNBOUNDED.");
    } catch ( const DualUnboundedException & exception ) {
        LPERROR("The problem is DUAL UNBOUNDED.");
    } catch ( const NumericalException & exception ) {
        LPERROR("Numerical error: "<<exception.getMessage());
    } catch ( const ParameterException & exception ) {
        LPERROR( exception.getMessage() );
    } catch ( const SyntaxErrorException & exception ) {
        exception.show();
    } catch ( const PanOptException & exception ) {
        LPERROR( exception.getMessage() );
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

    m_iterationReport.createSolutionReport();
    m_iterationReport.writeSolutionReport();

    if(m_saveBasis && m_saveLastBasis){
        std::string saveFormat = m_saveFormat;
        std::string filename = m_saveFilename;
        filename.append("_last.").append(m_saveFormat);
        std::transform(saveFormat.begin(),saveFormat.end(),saveFormat.begin(),::toupper);
        if(saveFormat.compare("BAS") == 0){
            saveBasis(filename.c_str(), new BasisHeadBAS, true);
        } else if (saveFormat.compare("PBF") == 0){
            saveBasis(filename.c_str(), new BasisHeadPanOpt, true);
        } else {
            throw ParameterException("Invalid save basis file format!");
        }
    }

    if(m_enableExport){
        m_iterationReport.createExportReport();
        m_iterationReport.writeExportReport(m_exportFilename);
    }

    releaseModules();
}

void Simplex::initModules() {
    m_startingBasisFinder = new StartingBasisFinder(*m_simplexModel, &m_basisHead, &m_variableStates, &m_basicVariableValues);
    int factorizationType = SimplexParameterHandler::getInstance().getIntegerParameterValue("factorization_type");
    if (factorizationType == 0){
        m_basis = new PfiBasis(*m_simplexModel, &m_basisHead, &m_variableStates, m_basicVariableValues);
    } else {
        LPERROR("Wrong parameter: factorization_type");
        //TODO: Throw parameter exception
    }
}

void Simplex::releaseModules() {
    if(m_startingBasisFinder){
        delete m_startingBasisFinder;
        m_startingBasisFinder = 0;
    }
    if(m_basis){
        delete m_basis;
        m_basis = 0;
    }
}

const std::vector<Numerical::Double> Simplex::getPrimalSolution() const {
    std::vector<Numerical::Double> result;
    unsigned int totalVariableCount = m_simplexModel->getColumnCount() + m_simplexModel->getRowCount();
    result.reserve(totalVariableCount);
    for(unsigned int i = 0; i<totalVariableCount; i++) {
        result[i] = *(m_variableStates.getAttachedData(i));
    }
    return result;
}


const std::vector<Numerical::Double> Simplex::getDualSolution() const {
    return std::vector<Numerical::Double>();
    //TODO
}


void Simplex::reinvert() throw (NumericalException) {
    m_basis->invert();
}

void Simplex::computeBasicSolution() throw (NumericalException) {
    m_basicVariableValues = m_simplexModel->getRhs();
    m_objectiveValue = - m_simplexModel->getCostConstant();


    const unsigned int columnCount = m_simplexModel->getColumnCount();
    //x_B=B^{-1}*(b-\SUM{U*x_U}-\SUM{L*x_L})
    IndexList<const Numerical::Double *>::Iterator it;
    IndexList<const Numerical::Double *>::Iterator itend;
    //This iterates through Simplex::NONBASIC_AT_LB and Simplex::NONBASIC_AT_UB too -- BUG with 2 partitions
    m_variableStates.getIterators(&it, &itend, Simplex::NONBASIC_AT_LB,2);

    for(; it != itend; it++) {
        if(*(it.getAttached()) != 0){
            if(it.getData() < columnCount){
                m_basicVariableValues.addVector(-1 * *(it.getAttached()), m_simplexModel->getMatrix().column(it.getData()));
                m_objectiveValue += m_simplexModel->getCostVector().at(it.getData()) * *(it.getAttached());
            } else {
                m_basicVariableValues.set(it.getData() - columnCount,
                                          Numerical::stableSub(m_basicVariableValues.at(it.getData() - columnCount), *(it.getAttached())));
            }
        }
    }

    //    This also sets the basic solution since the pointers of the basic variables point to the basic variable values vector
    m_basis->Ftran(m_basicVariableValues);

    m_variableStates.getIterators(&it, &itend, Simplex::BASIC);
    for(; it != itend; it++) {
        if(it.getData() < columnCount){
            m_objectiveValue += m_simplexModel->getCostVector().at(it.getData()) * *(it.getAttached());
        }
    }
}

void Simplex::computeReducedCosts() throw (NumericalException) {
    m_reducedCosts.clear();
    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();

    //Get the c_B vector
    Vector simplexMultiplier(rowCount);
    simplexMultiplier.setSparsityRatio(DENSE);
    const Vector& costVector = m_simplexModel->getCostVector();
    for(unsigned int i = 0; i<m_basisHead.size(); i++){
        if(m_basisHead[i] < (int) columnCount && costVector.at(m_basisHead[i]) != 0.0){
            simplexMultiplier.setNewNonzero(i, costVector.at(m_basisHead[i]));
        }
    }
    //Compute simplex multiplier
    m_basis->Btran(simplexMultiplier);

    //For each variable
    for(unsigned int i = 0; i < rowCount + columnCount; i++) {
        //Compute the dot product and the reduced cost
        Numerical::Double reducedCost;
        if(i < columnCount){
            reducedCost = Numerical::stableSub(costVector.at(i), simplexMultiplier.dotProduct(m_simplexModel->getMatrix().column(i)));
        } else {
            reducedCost = -1 * simplexMultiplier.at(i - columnCount);
        }
        if(reducedCost != 0.0){
            m_reducedCosts.setNewNonzero(i, reducedCost);
        }
    }
}
