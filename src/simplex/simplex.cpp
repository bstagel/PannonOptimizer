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
#include <simplex/basisheadbas.h>

#include <utils/thirdparty/prettyprint.h>

const static char * ITERATION_INDEX_NAME = "iteration";

Simplex::Simplex():
    m_simplexModel(NULL),
    m_variableStates(0,0),
    m_variableFeasibilities(0,0),
    m_reducedCostFeasibilities(0,0),
    m_basicVariableValues(0),
    m_reducedCosts(0),
    m_objectiveValue(0),
    m_phaseIObjectiveValue(0),
    m_baseChanged(false),
    m_startingBasisFinder(NULL),
    m_basis(NULL),
    m_iterationIndex(0)
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

std::vector<IterationReportField> Simplex::getIterationReportFields(
        enum ITERATION_REPORT_FIELD_TYPE & type) const {
    std::vector<IterationReportField> result;

    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
    {
        IterationReportField iterationField(ITERATION_INDEX_NAME, 10, 2, IterationReportField::IRF_CENTER,
                                    IterationReportField::IRF_INT, *this);
        result.push_back(iterationField);
    }
        break;

    case IterationReportProvider::IRF_SOLUTION:
        break;

    }

    return result;
}

std::string Simplex::getIterationReportString(const std::string & name,
                                                      enum ITERATION_REPORT_FIELD_TYPE & type) const {
    __UNUSED(name);
    __UNUSED(type);
    return std::string("");
}

int Simplex::getIterationReportInteger(const std::string & name,
                                       enum ITERATION_REPORT_FIELD_TYPE & type) const {

    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
        if (name == ITERATION_INDEX_NAME) {
            return m_iterationIndex;
        }
        break;

    case IterationReportProvider::IRF_SOLUTION:
        break;

    }

    return 0;
}

double Simplex::getIterationReportFloat(const std::string & name,
                                        enum ITERATION_REPORT_FIELD_TYPE & type) const {
    __UNUSED(name);
    __UNUSED(type);
    return 0.0;
}

bool Simplex::getIterationReportBool(const std::string & name,
                                     enum ITERATION_REPORT_FIELD_TYPE & type) const {
    __UNUSED(name);
    __UNUSED(type);
    return false;
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
    basisWriter->startWrting(fileName);
    int variableIndex;
    unsigned int position = 0;
    STL_FOREACH(std::vector<int>, m_basisHead, basisHeadIter) {
        variableIndex = *basisHeadIter;
        Variable variable = m_simplexModel->getVariable(variableIndex);
        basisWriter->addBasicVariable( variable, position, m_basicVariableValues.at(position) );
        position++;
    }

    IndexList<const Numerical::Double *>::Iterator iter, iterEnd;
    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_LB, 1);
    for (; iter != iterEnd; iter++) {
        variableIndex = iter.getData();
        Variable variable = m_simplexModel->getVariable(variableIndex);
        // TODO: itt az erteket nem kell majd atadni
        basisWriter->addNonbasicVariable(variable, Simplex::NONBASIC_AT_LB, variable.getLowerBound());
    }

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_UB, 1);
    for (; iter != iterEnd; iter++) {
        variableIndex = iter.getData();
        Variable variable = m_simplexModel->getVariable(variableIndex);
        // TODO: itt az erteket nem kell majd atadni
        basisWriter->addNonbasicVariable(variable, Simplex::NONBASIC_AT_UB, variable.getUpperBound());
    }

    basisWriter->finishWriting();

    if (releaseWriter == true) {
        delete basisWriter;
    }
}

void Simplex::loadBasis(const char * fileName, BasisHeadIO * basisReader, bool releaseReader) {
    const unsigned int variableCount = m_simplexModel->getColumnCount() + m_simplexModel->getRowCount();
    basisReader->startReading(fileName, variableCount);
    unsigned int index;
    for (index = 0; index < variableCount; index++) {
        basisReader->addVariable(m_simplexModel->getVariable(index));
    }

    basisReader->finishReading(&m_basisHead, &m_variableStates);

    if (releaseReader) {
        delete basisReader;
    }
}

void Simplex::registerIntoIterationReport(const IterationReportProvider & provider) {
    m_iterationReport.addProviderForStart(provider);
    m_iterationReport.addProviderForIteration(provider);
    m_iterationReport.addProviderForSolution(provider);
}

void Simplex::solve() {
    initModules();

    ParameterHandler & simplexParameters = SimplexParameterHandler::getInstance();

    const unsigned int iterationLimit = simplexParameters.getParameterValue("iteration_limit");
    const double timeLimit = simplexParameters.getParameterValue("time_limit");
    StartingBasisFinder::STARTING_BASIS_STRATEGY startingBasisStratgy =
            (StartingBasisFinder::STARTING_BASIS_STRATEGY)simplexParameters.getParameterValue("starting_basis");
    unsigned int reinversionFrequency = simplexParameters.getParameterValue("reinversion_frequency");
    unsigned int reinversionCounter = reinversionFrequency;
    Timer timer;

//    registerIntoIterationReport(*this);
    m_iterationReport.addProviderForStart(*this);
    m_iterationReport.addProviderForIteration(*this);
    m_iterationReport.addProviderForSolution(*this);


    m_iterationReport.createStartReport();
    m_iterationReport.writeStartReport();

    //loadBasis("basis.bas", new BasisHeadBAS, true);

    try {
        timer.start();
        m_basicVariableValues.prepareForData(m_simplexModel->getRhs().nonZeros(), m_basicVariableValues.length(), DENSE);
        Vector::NonzeroIterator it = m_simplexModel->getRhs().beginNonzero();
        Vector::NonzeroIterator itend = m_simplexModel->getRhs().endNonzero();
        for(; it < itend; it++){
            m_basicVariableValues.newNonZero(*it, it.getIndex());
        }
        m_startingBasisFinder->findStartingBasis(startingBasisStratgy);

        for (m_iterationIndex = 0; m_iterationIndex < iterationLimit && timer.getTotalElapsed() < timeLimit; m_iterationIndex++) {
            //LPINFO("\n    Iteration: "<<m_iterationIndex);

            // ITTEN MENTJUK KI A BAZIST:
            saveBasis("basis.bas", new BasisHeadBAS, true);

            if(reinversionCounter == reinversionFrequency){
                reinversionCounter = 0;
                LPINFO("reinvert");
                reinvert();
                //                LPINFO("computeBasicSolution");
                computeBasicSolution();
                //                LPINFO("computeReducedCosts");
                computeReducedCosts();
                //                LPINFO("computeFeasibility");
                computeFeasibility();
            }
            try{
                //                LPINFO("checkFeasibility");
                checkFeasibility();
                //                LPINFO("price");
                price();
                //                LPINFO("selectPivot");
                selectPivot();
                //                LPINFO("update");
                reinversionCounter++;
                if(reinversionCounter < reinversionFrequency){
                    update();
                }
            }
            catch ( const OptimalException & exception ) {
                //Check the result with triggering reinversion
                if(reinversionCounter == 0){
                    throw exception;
                } else {
                    LPINFO("TRIGGERING REINVERSION TO CHECK OPTIMALITY! ");
                    reinversionCounter = reinversionFrequency;
                }
            }
            catch ( const PrimalInfeasibleException & exception ) {
                //Check the result with triggering reinversion
                if(reinversionCounter == 0){
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                }
            }
            catch ( const DualInfeasibleException & exception ) {
                //Check the result with triggering reinversion
                if(reinversionCounter == 0){
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                }
            }
            catch ( const PrimalUnboundedException & exception ) {
                //Check the result with triggering reinversion
                if(reinversionCounter == 0){
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                }
            }
            catch ( const DualUnboundedException & exception ) {
                //Check the result with triggering reinversion
                if(reinversionCounter == 0){
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                }
            }
            catch ( const NumericalException & exception ) {
                //Check the result with triggering reinversion
                LPINFO("TRIGGERING REINVERSION TO HANDLE NUMERICAL ERROR! ");
                if(reinversionCounter == 0){
                    throw exception;
                } else {
                    reinversionCounter = reinversionFrequency;
                }
            }
            m_iterationReport.createIterationReport();
            m_iterationReport.writeIterationReport();
        }
        // TODO: ez a stop sosem fog vegrehajtodni, ha
        // akarmilyen exceptionnel lepunk ki!
        timer.stop();

    } catch ( const OptimalException & exception ) {
        LPINFO("OPTIMAL SOLUTION found! ");
        LPINFO("The objective value: " << m_objectiveValue);
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
                                          m_basicVariableValues.at(it.getData() - columnCount) - *(it.getAttached()));
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

void Simplex::transform(int incomingIndex,
                        int outgoingIndex,
                        const std::vector<unsigned int>& boundflips,
                        Numerical::Double primalTheta) {

    //Todo update the solution properly
    //    Numerical::Double boundflipTheta = 0.0;
    std::vector<unsigned int>::const_iterator it = boundflips.begin();
    std::vector<unsigned int>::const_iterator itend = boundflips.end();
    //TODO Atgondolni hogy mit mikor kell ujraszamolni
    for(; it < itend; it++){
        LPWARNING("BOUNDFLIPPING");
        const Variable& variable = m_simplexModel->getVariable(*it);
        Numerical::Double boundDistance = Numerical::fabs(variable.getLowerBound() - variable.getUpperBound());
        Vector alpha = m_simplexModel->getMatrix().column(*it);
        m_basis->Ftran(alpha);
        if(m_variableStates.where(*it) == Simplex::NONBASIC_AT_LB) {
            m_basicVariableValues.addVector(-1 * boundDistance, alpha);
        } else if(m_variableStates.where(*it) == Simplex::NONBASIC_AT_UB){
            m_basicVariableValues.addVector(-1 * boundDistance, alpha);
        } else {
            LPERROR("Boundflipping variable in the basis (or superbasic)!");
            //TODO Throw some exception here
        }
    }

    if(outgoingIndex != -1 && incomingIndex != -1){
        //Save whether the basis is to be changed
        m_baseChanged = true;

        unsigned int rowCount = m_simplexModel->getRowCount();
        unsigned int columnCount = m_simplexModel->getColumnCount();

        //TODO: Az alpha vajon sparse vagy dense?
        Vector alpha(rowCount);
        if(incomingIndex < (int)columnCount){
            alpha = m_simplexModel->getMatrix().column(incomingIndex);
        } else {
            alpha.setNewNonzero(incomingIndex - columnCount, 1);
        }
        m_basis->Ftran(alpha);
        m_basicVariableValues.addVector(-1 * primalTheta, alpha);

        //The incoming variable is NONBASIC thus the attached data gives the appropriate bound or zero
        m_basis->append(alpha, outgoingIndex, incomingIndex);
        m_basicVariableValues.set(outgoingIndex, *(m_variableStates.getAttachedData(incomingIndex)) + primalTheta);
    }
}
