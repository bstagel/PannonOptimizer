/**
 * @file pnsinterface.cpp
 */

#include <lp/pnsinterface.h>
#include <simplex/simplexcontroller.h>
#include <simplex/simplexparameterhandler.h>
#include <lp/manualmodelbuilder.h>
#include <lp/model.h>

PnsInterface::PnsInterface():
    m_simplex(new SimplexController()),
    m_builder(nullptr),
    m_model(new Model())
{

}

PnsInterface::~PnsInterface()
{
    if(m_simplex){
        delete m_simplex;
    }
    if(m_model){
        delete m_model;
    }
}

void PnsInterface::loadProblem (  const int numcols, const int numrows,
                    const int* columnStartIndex, const int* index,
                    const double* value,
                    const double* collb, const double* colub,
                    const double* obj,
                    const double* rowlb, const double* rowub)
{
    m_builder = new ManualModelBuilder();
    m_builder->setName("PNS model");
    for(int i=0; i<numcols; i++){
        Variable variable = Variable::createBoundedTypeVariable((std::string("x")+std::to_string(i)).c_str(),
                                                                collb[i], colub[i]);
        //columnStartIndex has n+1 elements
        int nonzeros= columnStartIndex[i+1] - columnStartIndex[i];
        const double * valuePointer = value + columnStartIndex[i];
        const unsigned int * indexPointer = (unsigned int*)const_cast<int*> (index + columnStartIndex[i]);
        m_builder->addVariable(variable, obj[i], nonzeros, valuePointer, indexPointer);
    }
    for(int i=0; i<numrows; i++){
        m_builder->setConstraint(i, Constraint::createRangeTypeConstraint((std::string("c")+std::to_string(i)).c_str(),
                                                                          rowlb[i],rowub[i]));
    }
    m_model->build(*m_builder);
    delete m_builder;
}

void PnsInterface::setLogLevel(int value)
{
    SimplexParameterHandler::getInstance().setParameterValue("Global.debug_level", value);
}

void PnsInterface::setRowName(int rowIndex, std::string & name)
{
    m_model->changeRowName(rowIndex, name);
}

void PnsInterface::setColumnName(int colIndex, std::string & name)
{
    m_model->changeColumnName(colIndex, name);
}

void PnsInterface::chgRowLower(const double * rowLower)
{
    m_model->changeConstraintLower(rowLower);
}

void PnsInterface::chgRowUpper(const double * rowUpper)
{
    m_model->changeConstraintUpper(rowUpper);
}

void PnsInterface::chgColumnLower(const double * columnLower)
{
    m_model->changeVariableLower(columnLower);
}

void PnsInterface::chgColumnUpper(const double * columnUpper)
{
    m_model->changeVariableUpper(columnUpper);
}

void PnsInterface::chgObjCoefficients(const double * objIn)
{
    m_model->changeObjectiveCoefficients(objIn);
}

int PnsInterface::writeMps(const char *filename,
             int formatType, int numberAcross,
             double objSense) const
{
    __UNUSED(filename);
    __UNUSED(formatType);
    __UNUSED(numberAcross);
    __UNUSED(objSense);
    LPERROR("MPS writing is not supported in the Pannon Optimizer yet!");
    return 0;
}

int PnsInterface::dual()
{
    m_simplex->solve(*m_model);
    return 0;
}

bool PnsInterface::isProvenOptimal() const
{
    return m_simplex->isOptimal();
}

const double * PnsInterface::primalColumnSolution() const
{
    return m_simplex->getPrimalSolution().begin().operator->();
}

const double * PnsInterface::getReducedCost() const
{
    return m_simplex->getReducedCosts().begin().operator->();
}
