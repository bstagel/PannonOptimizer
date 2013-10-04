/**
 * @file simplex.cpp
 */

#include <simplex/simplex.h>
#include <utils/parameterhandler.h>
#include <utils/timer.h>
#include <utils/exceptions.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/startingbasisfinder.h>
#include <simplex/pfibasis.h>
#include <globals.h>

Simplex::Simplex():
    m_simplexModel(NULL),
    m_variableStates(0,0),
    m_variableFeasibilities(0,0),
    m_reducedCostFeasibilities(0,0),
    m_basicVariableValues(0),
    m_reducedCosts(0),
    m_objectiveValue(0),
    m_phaseIObjectiveValue(0),
    m_startingBasisFinder(NULL),
    m_basis(NULL)
{
    m_basicVariableValues.setSparsityRatio(DENSE);
    m_reducedCosts.setSparsityRatio(DENSE);
}

Simplex::~Simplex() {
    releaseModules();

    if(m_simplexModel){
        delete m_simplexModel;
        m_simplexModel = 0;
    }
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
}

void Simplex::constraintAdded()
{

}

void Simplex::variableAdded()
{

}

void Simplex::solve() {
    initModules();
    ParameterHandler & simplexParameters = SimplexParameterHandler::getInstance();
    unsigned int iterationIndex;
    const unsigned int iterationLimit = simplexParameters.getParameterValue("iteration_limit");
    const double timeLimit = simplexParameters.getParameterValue("time_limit");
    StartingBasisFinder::STARTING_BASIS_STRATEGY startingBasisStratgy =
            (StartingBasisFinder::STARTING_BASIS_STRATEGY)simplexParameters.getParameterValue("starting_basis");
    unsigned int reinversionFrequency = simplexParameters.getParameterValue("reinversion_frequency");
    unsigned int reinversionCounter = 0;
    Timer timer;

    try {
        timer.start();
        m_basicVariableValues.prepareForData(m_simplexModel->getRhs().nonZeros(), m_basicVariableValues.length(), DENSE);
        Vector::NonzeroIterator it = m_simplexModel->getRhs().beginNonzero();
        Vector::NonzeroIterator itend = m_simplexModel->getRhs().endNonzero();
        for(; it < itend; it++){
            m_basicVariableValues.newNonZero(*it, it.getIndex());
        }
        m_startingBasisFinder->findStartingBasis(startingBasisStratgy);

        for (iterationIndex = 0;
             iterationIndex < iterationLimit && timer.getTotalElapsed() < timeLimit;
             iterationIndex++) {
            if(reinversionCounter == reinversionFrequency){
                reinversionCounter = 0;
                reinvert();
                computeBasicSolution();
                computeReducedCosts();
                computeFeasibility();
            }
            try{
                checkFeasibility();
                price();
                selectPivot();
                reinversionCounter++;
                if(reinversionCounter < reinversionFrequency){
                    update();
                }
//                iterate();
            }
            catch ( const OptimizationResultException & exception ) {
                   //Check the result with triggering reinversion
                if(reinversionCounter == 0){
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                }
            }
        }
        timer.stop();

    } catch ( const OptimalException & exception ) {
        LPINFO("Optimal solution found!");
        // TODO: postsovle, post scaling
        // TODO: Save optimal basis if necessary
    } catch ( const InfeasibleException & exception ) {
        LPERROR("The problem is infeasible.");
    } catch ( const UnboundedException & exception ) {
        LPERROR("The problem is unbounded.");
    } catch ( const NumericalException & exception ) {
        LPERROR("Numerical error!");
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

    releaseModules();
}

void Simplex::initModules() {
    m_startingBasisFinder = new StartingBasisFinder(*m_simplexModel, &m_basisHead, &m_variableStates, &m_basicVariableValues);
    double factorizationType = SimplexParameterHandler::getInstance().getParameterValue("factorization_type");
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

}

void Simplex::computeReducedCosts() throw (NumericalException) {

}
