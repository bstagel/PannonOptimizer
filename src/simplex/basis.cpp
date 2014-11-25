/**
 * @file basis.cpp
 */
#include <simplex/basis.h>
#include <simplex/simplex.h>
#include <simplex/simplexmodel.h>
#include <simplex/simplexparameterhandler.h>
#include <utils/thirdparty/prettyprint.h>
#include <fstream>

int etaExpSum = 0;
double etaExpSquareSum = 0;
int etaExpCount = 0;

Basis::Basis(const SimplexModel& model,
             std::vector<int>* basisHead,
             IndexList<const Numerical::Double*>* variableStates,
             const Vector &basicVariableValues) :
    m_model(model),
    m_basisHead(basisHead),
    m_variableStates(variableStates),
    m_basicVariableValues(basicVariableValues),
    m_isFresh(false),
    m_basisNonzeros(0),
    m_inverseNonzeros(0),
    m_singularityCounter(0),
    m_inversion(SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_pivot"))
{
}

Basis::~Basis()
{

}

void Basis::setNewHead() {
    //This vector indicates the pattern of the basis columns
    std::vector<char> nonbasic(m_model.getColumnCount() + m_model.getRowCount(), false);
    //First mark the positions of the given basis head
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); ++it) {
        nonbasic[*it] = true;
    }
    //Update the basis head with the recomputed one
    m_basisHead->assign(m_basisNewHead.begin(), m_basisNewHead.end());
    //Set the basic variables state and remove their traces from the pattern vector
    for (std::vector<int>::iterator it = m_basisHead->begin(); it < m_basisHead->end(); ++it) {
        nonbasic[*it] = false;
        if(m_variableStates->where(*it) != Simplex::BASIC) {
            m_variableStates->move(*it, Simplex::BASIC, &(m_basicVariableValues.at(it - m_basisHead->begin())));
        } else {
            m_variableStates->setAttachedData(*it, &(m_basicVariableValues.at(it - m_basisHead->begin())));
        }
    }
    //If the pattern vector still contains true values then the basis head is modified, thus some variables
    //are out of the basis, these must be marked as nonbasic and their states must be updated too.
    for (std::vector<char>::iterator it = nonbasic.begin(); it < nonbasic.end(); ++it) {
        if (*it == true) {
            const Variable& variable = m_model.getVariable(*it);
            if (variable.getType() == Variable::FREE) {
                m_variableStates->move(*it, Simplex::NONBASIC_FREE, &ZERO);
            } else if (variable.getType() == Variable::MINUS) {
                m_variableStates->move(*it, Simplex::NONBASIC_AT_UB, &(variable.getUpperBound()));
                return;
            } else if (variable.getType() == Variable::PLUS) {
                m_variableStates->move(*it, Simplex::NONBASIC_AT_LB, &(variable.getLowerBound()));
                return;
            } else if (variable.getType() == Variable::FIXED) {
                m_variableStates->move(*it, Simplex::NONBASIC_FIXED, &(variable.getLowerBound()));
                return;
            } else {
                m_variableStates->move(*it, Simplex::NONBASIC_AT_LB, &(variable.getLowerBound()));
                return;
            }
        }
    }
}


Vector* Basis::createEta(const Vector& vector, int pivotPosition)
{
    Vector* eta = new Vector(vector.length(), Vector::SPARSE_VECTOR);
    eta->prepareForData(vector.nonZeros(), vector.length());
    //TODO Ezt vajon lehet gyorsabban?
    Numerical::Double atPivot = vector.at(pivotPosition);

#ifndef NDEBUG
    if(Numerical::fabs(atPivot) < m_inversion){
        LPWARNING("The eta vector pivot element is small: "<<atPivot << " ; "<<pivotPosition);
    }
#endif
    if (Numerical::equals(atPivot, 0)) {
        throw NumericalException(std::string("NUMERICAL problem: Pivot element is ") +  std::to_string(atPivot)
                                 + std::string(" at row ") + std::to_string(pivotPosition) );
    } else {
        Vector::NonzeroIterator it = vector.beginNonzero();
        Vector::NonzeroIterator endit = vector.endNonzero();
        for (; it < endit; ++it) {
            if (it.getIndex() == (unsigned int) pivotPosition) {
                eta->newNonZero(1 / atPivot, pivotPosition);
            } else {
                eta->newNonZero(-(*it) / atPivot, it.getIndex());
                //                if(Numerical::fabs(-(*it) / atPivot) < Numerical::AbsoluteTolerance){
                //                    LPERROR("INVERSION ERROR!: "<<"atPivot: "<<atPivot);
                //                    LPERROR("INVERSION ERROR!: "<<"(*it): "<<(*it));
                //                    LPERROR("INVERSION ERROR!: "<<"index: "<<it.getIndex()<< " value: "<<Numerical::fabs(-(*it) / atPivot));
                //                }
            }
        }
        DEVINFO(D::PFIMAKER, "Eta vector created with pivot " << pivotPosition);
        return eta;
    }
}

void Basis::printActiveSubmatrix() const
{
#ifndef NDEBUG
    DEVINFO(D::PFIMAKER, "Active submatrix pattern by columns");
    for (int i = 0; i < (int) m_basicColumns.size(); i++) {
        std::string s;
        for (std::vector<const Vector*>::const_iterator it = m_basicColumns.begin(); it < m_basicColumns.end(); ++it) {
            s += Numerical::equals((*it)->at(i), 0) ? "-" : "X";
        }
        DEVINFO(D::PFIMAKER, s);
    }

#endif //!NDEBUG
}

