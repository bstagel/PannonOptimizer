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

const static char * EXPORT_PROBLEM_NAME = "export_problem_name";
const static char * EXPORT_ITERATION = "export_iteration";
const static char * EXPORT_PHASE1_ITERATION = "export_phase1_iteration";
const static char * EXPORT_TIME = "export_time";
const static char * EXPORT_PHASE1_TIME = "export_phase1_time";
const static char * EXPORT_SOLUTION = "export_solution";
const static char * EXPORT_FALLBACK = "export_fallback";
const static char * EXPORT_SINGULARITY = "export_singularity";
const static char * EXPORT_TRIGGERED_REINVERSION = "export_triggered_reinversion";
const static char * EXPORT_BAD_ITERATION = "export_bad_iteration";
const static char * EXPORT_DEGENERATION = "export_degeneration";
const static char * EXPORT_E_ABSOLUTE = "export_e_absolute";
const static char * EXPORT_E_RELATIVE = "export_e_relative";
const static char * EXPORT_E_FEASIBILITY = "export_e_feasibility";
const static char * EXPORT_E_OPTIMALITY = "export_e_optimality";
const static char * EXPORT_E_PIVOT = "export_e_pivot";
const static char * EXPORT_PIVOT_THRESHOLD = "export_pivot_threshold";
const static char * EXPORT_NONLINEAR_DUAL_PHASEI_FUNCTION = "export_nonlinear_dual_phaseI_function";
const static char * EXPORT_NONLINEAR_DUAL_PHASEII_FUNCTION = "export_nonlinear_dual_phaseII_function";
const static char * EXPORT_ENABLE_FAKE_FEASIBILITY = "export_enable_fake_feasibility";

Simplex::Simplex():
    m_iterationIndex(0),
    m_masterTolerance(0),
    m_toleranceStep(0),
    m_simplexModel(NULL),
    m_variableStates(0,0),
    m_basicVariableFeasibilities(0,0),
    m_reducedCostFeasibilities(0,0),
    m_basicVariableValues(0),
    m_reducedCosts(0),
    m_objectiveValue(0),
    m_phaseIObjectiveValue(-Numerical::Infinity),
    m_feasible(false),
    m_baseChanged(false),
    m_freshBasis(false),
    m_debugLevel(0),
    m_referenceObjective(0),
    m_phase1Iteration(0),
    m_phase1Time(0.0),
    m_fallbacks(0),
    m_triggeredReinversion(0),
    m_badIterations(0),
    m_degenerateIterations(0),
    m_startingBasisFinder(NULL),
    m_basis(NULL)
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
            result.push_back(IterationReportField(EXPORT_PROBLEM_NAME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_STRING, *this));
            result.push_back(IterationReportField(EXPORT_PHASE1_ITERATION, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_ITERATION, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_PHASE1_TIME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  6, IterationReportField::IRF_FIXED));
            result.push_back(IterationReportField(EXPORT_TIME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  6, IterationReportField::IRF_FIXED));
            result.push_back(IterationReportField(EXPORT_SOLUTION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  10, IterationReportField::IRF_SCIENTIFIC));
            result.push_back(IterationReportField(EXPORT_FALLBACK,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_SINGULARITY,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_TRIGGERED_REINVERSION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_BAD_ITERATION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_DEGENERATION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_E_ABSOLUTE,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  10, IterationReportField::IRF_SCIENTIFIC));
            result.push_back(IterationReportField(EXPORT_E_RELATIVE,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  10, IterationReportField::IRF_SCIENTIFIC));
            result.push_back(IterationReportField(EXPORT_E_FEASIBILITY, 20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  10, IterationReportField::IRF_SCIENTIFIC));
            result.push_back(IterationReportField(EXPORT_E_OPTIMALITY, 20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  10, IterationReportField::IRF_SCIENTIFIC));
            result.push_back(IterationReportField(EXPORT_E_PIVOT, 20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  10, IterationReportField::IRF_SCIENTIFIC));
            result.push_back(IterationReportField(EXPORT_PIVOT_THRESHOLD, 20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  10, IterationReportField::IRF_SCIENTIFIC));
            // Ratio test research set
        } if (m_exportType == 1) {
            result.push_back(IterationReportField(EXPORT_PROBLEM_NAME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_STRING, *this));
            result.push_back(IterationReportField(EXPORT_PHASE1_ITERATION, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_ITERATION, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_PHASE1_TIME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  6, IterationReportField::IRF_FIXED));
            result.push_back(IterationReportField(EXPORT_TIME, 20, 0, IterationReportField::IRF_LEFT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  6, IterationReportField::IRF_FIXED));
            result.push_back(IterationReportField(EXPORT_SOLUTION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_FLOAT, *this,
                                                  10, IterationReportField::IRF_SCIENTIFIC));
            result.push_back(IterationReportField(EXPORT_FALLBACK,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_SINGULARITY,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_TRIGGERED_REINVERSION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_BAD_ITERATION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_DEGENERATION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_NONLINEAR_DUAL_PHASEI_FUNCTION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_NONLINEAR_DUAL_PHASEII_FUNCTION,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_ENABLE_FAKE_FEASIBILITY,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
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

Entry Simplex::getIterationEntry(const string &name, ITERATION_REPORT_FIELD_TYPE &type) const {
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
        } else if (name == SOLUTION_INVERSION_TIMER_NAME) {
            reply.m_double = m_inversionTimer.getCPUTotalElapsed();
        } else if (name == SOLUTION_COMPUTE_BASIC_SOLUTION_TIMER_NAME) {
            reply.m_double = m_computeBasicSolutionTimer.getCPUTotalElapsed();
        } else if (name == SOLUTION_COMPUTE_REDUCED_COSTS_TIMER_NAME) {
            reply.m_double = m_computeReducedCostsTimer.getCPUTotalElapsed();
        } else if (name == SOLUTION_COMPUTE_FEASIBILITY_TIMER_NAME) {
            reply.m_double = m_computeFeasibilityTimer.getCPUTotalElapsed();
        } else if (name == SOLUTION_CHECK_FEASIBILITY_TIMER_NAME) {
            reply.m_double = m_checkFeasibilityTimer.getCPUTotalElapsed();
        } else if (name == SOLUTION_PRICE_TIMER_NAME) {
            reply.m_double = m_priceTimer.getCPUTotalElapsed();
        } else if (name == SOLUTION_SELECT_PIVOT_TIMER_NAME) {
            reply.m_double = m_selectPivotTimer.getCPUTotalElapsed();
        } else if (name == SOLUTION_UPDATE_TIMER_NAME) {
            reply.m_double = m_updateTimer.getCPUTotalElapsed();
        }
        break;

    case IterationReportProvider::IRF_EXPORT:
        if(name == EXPORT_PROBLEM_NAME){
            reply.m_string = new std::string(m_simplexModel->getName());
        } else if(name == EXPORT_PHASE1_ITERATION){
            reply.m_integer = m_phase1Iteration;
        } else if(name == EXPORT_ITERATION){
            reply.m_integer = m_iterationIndex;
        } else if(name == EXPORT_PHASE1_TIME){
            reply.m_double = m_phase1Time;
        } else if(name == EXPORT_TIME){
            reply.m_double = m_solveTimer.getCPUTotalElapsed();
        } else if(name == EXPORT_SOLUTION){
            reply.m_double = m_objectiveValue;
        } else if(name == EXPORT_FALLBACK){
            reply.m_integer = m_fallbacks;
        } else if(name == EXPORT_SINGULARITY){
            reply.m_integer = m_basis->getSingularityCount();
        } else if(name == EXPORT_TRIGGERED_REINVERSION){
            reply.m_integer = m_triggeredReinversion;
        } else if(name == EXPORT_BAD_ITERATION){
            reply.m_integer = m_badIterations;
        } else if(name == EXPORT_DEGENERATION){
            reply.m_integer = m_degenerateIterations;
        }  else if(name == EXPORT_E_ABSOLUTE){
            reply.m_double = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_absolute");
        } else if(name == EXPORT_E_RELATIVE){
            reply.m_double = LinalgParameterHandler::getInstance().getDoubleParameterValue("e_relative");
        } else if(name == EXPORT_E_FEASIBILITY){
            reply.m_double = SimplexParameterHandler::getInstance().getDoubleParameterValue("e_feasibility");
        } else if(name == EXPORT_E_OPTIMALITY){
            reply.m_double = SimplexParameterHandler::getInstance().getDoubleParameterValue("e_optimality");
        } else if(name == EXPORT_E_PIVOT){
            reply.m_double = SimplexParameterHandler::getInstance().getDoubleParameterValue("e_pivot");
        } else if(name == EXPORT_PIVOT_THRESHOLD){
            reply.m_double = SimplexParameterHandler::getInstance().getDoubleParameterValue("pivot_threshold");
        } else if(name == EXPORT_NONLINEAR_DUAL_PHASEI_FUNCTION){
            reply.m_integer = SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_dual_phaseI_function");
        } else if(name == EXPORT_NONLINEAR_DUAL_PHASEII_FUNCTION){
            reply.m_integer = SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_dual_phaseII_function");
        } else if(name == EXPORT_ENABLE_FAKE_FEASIBILITY){
            reply.m_integer = SimplexParameterHandler::getInstance().getIntegerParameterValue("enable_fake_feasibility");
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
    m_basicVariableFeasibilities.init(rowCount, FEASIBILITY_ENUM_LENGTH);
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

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_FIXED, 1);
    for (; iter != iterEnd; iter++) {
        variableIndex = iter.getData();
        const Variable * variable = &m_simplexModel->getVariable(variableIndex);
        // TODO: itt az erteket nem kell majd atadni
        basisWriter->addNonbasicVariable(variable, variableIndex, Simplex::NONBASIC_FIXED, variable->getLowerBound());
    }

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

        initWorkingTolerance();

        for (m_iterationIndex = 1; m_iterationIndex <= iterationLimit &&
             (m_solveTimer.getCPURunningTime()) < timeLimit; m_iterationIndex++) {

            if(m_saveBasis){
                if ((m_iterationIndex == m_saveIteration) ||
                        (m_savePeriodically != 0 && ((m_iterationIndex % m_savePeriodically) == 0) )){
                    LPERROR("Saving basis "<<m_iterationIndex);
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

            computeWorkingTolerance();

            //            if(m_iterationIndex>438){
            //                LPINFO("BEGIN FEASIBILITY OK: "<<Checker::checkFeasibilityConditions(*this, true));
            //                LPINFO("BEGIN OPTIMALITY OK: "<<Checker::checkOptimalityConditions(*this, true));
            //                LPINFO("BEGIN CONSTRAINTS OK: "<<Checker::checkAllConstraints(*this, true));
            //                LPINFO("BEGIN NONBASICS OK: "<<Checker::checkNonbasicVariableStates(*this, true));
            //                LPINFO("BEGIN BASICS OK: "<<Checker::checkBasicVariableStates(*this, true));
            //            }

            if(reinversionCounter == reinversionFrequency){
                m_freshBasis = true;
                releaseLocks();

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
                setReferenceObjective();

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

                checkReferenceObjective();
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
                if(m_freshBasis){
                    m_solveTimer.stop();
                    throw exception;
                } else {
#ifndef NDEBUG
                    LPINFO("TRIGGERING REINVERSION TO HANDLE NUMERICAL ERROR! (" << exception.getMessage() <<")");
#endif
                    m_triggeredReinversion++;
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
    if ( m_simplexModel->getModel().isScaled() ) {
        const std::vector<Numerical::Double> & columnMultipliers = m_simplexModel->getModel().getColumnMultipliers();
        for(unsigned int i = 0; i<totalVariableCount; i++) {
            result[i] = *(m_variableStates.getAttachedData(i)) / columnMultipliers[i];
        }
    } else {
        for(unsigned int i = 0; i<totalVariableCount; i++) {
            result[i] = *(m_variableStates.getAttachedData(i));
        }
    }
    return result;
}


const std::vector<Numerical::Double> Simplex::getDualSolution() const {
    return std::vector<Numerical::Double>();
    //TODO
}


void Simplex::reinvert() {
    m_basis->invert();
}

void Simplex::computeBasicSolution() {
    m_basicVariableValues = m_simplexModel->getRhs();
    m_objectiveValue = - m_simplexModel->getCostConstant();

    //    Checker::checkPfiWithFtran(*this);
    //    Checker::checkPfiWithBtran(*this);
    //    Checker::checkPfiWithReducedCost(*this);

    const unsigned int columnCount = m_simplexModel->getColumnCount();
    //x_B=B^{-1}*(b-\SUM{U*x_U}-\SUM{L*x_L})
    IndexList<const Numerical::Double *>::Iterator it;
    IndexList<const Numerical::Double *>::Iterator itend;
    //This iterates through Simplex::NONBASIC_AT_LB and Simplex::NONBASIC_AT_UB too -- BUG with 2 partitions
    m_variableStates.getIterators(&it, &itend, Simplex::NONBASIC_AT_LB,3);

    for(; it != itend; it++) {
        if(*(it.getAttached()) != 0){
            if(it.getData() < columnCount){
                m_basicVariableValues.addVector(-1 * *(it.getAttached()), m_simplexModel->getMatrix().column(it.getData()), Numerical::ADD_ABS);
                m_objectiveValue += m_simplexModel->getCostVector().at(it.getData()) * *(it.getAttached());
            } else {
                m_basicVariableValues.set(it.getData() - columnCount,
                                          Numerical::stableSub(m_basicVariableValues.at(it.getData() - columnCount), *(it.getAttached())));
                //                                          m_basicVariableValues.at(it.getData() - columnCount) - *(it.getAttached()));
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

void Simplex::computeReducedCosts() {
    m_reducedCosts.clear();
    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();

    //Get the c_B vector
    Vector simplexMultiplier(rowCount);
    simplexMultiplier.setSparsityRatio(DENSE);
    const Vector& costVector = m_simplexModel->getCostVector();
    for(unsigned int i = 0; i<m_basisHead.size(); i++){
        // TODO: majd a perturbacio miatt el kell tekinteni a feltetel elso feletol
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
        //        if (Numerical::fabs(reducedCost) < 1e-5 && Numerical::fabs(reducedCost) != 0 ) LPINFO("d_"<<i<<" "<<reducedCost);
    }

    //TODO: Ez plusz egy csomo ido
    //b-\SUM{U*x_U}-\SUM{L*x_L}
    //    Vector modifiedRhs;
    //    modifiedRhs = m_simplexModel->getRhs();
    //    IndexList<const Numerical::Double *>::Iterator it;
    //    IndexList<const Numerical::Double *>::Iterator itend;
    //    m_variableStates.getIterators(&it, &itend, Simplex::NONBASIC_AT_LB,3);

    //    for(; it != itend; it++) {
    //        if(*(it.getAttached()) != 0){
    //            if(it.getData() < columnCount){
    //                modifiedRhs.addVector(-1 * *(it.getAttached()), m_simplexModel->getMatrix().column(it.getData()), Numerical::ADD_ABS);
    //            } else {
    //                modifiedRhs.set(it.getData() - columnCount,
    //                                Numerical::stableSub(modifiedRhs.at(it.getData() - columnCount), *(it.getAttached())));
    //            }
    //        }
    //    }
    //    m_dualObjectiveValue = simplexMultiplier.dotProduct(modifiedRhs);
    //    m_dualObjectiveValue = simplexMultiplier.dotProduct(m_simplexModel->getRhs());
}
