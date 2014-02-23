/**
 * @file simplexmodel.cpp
 */

#include <simplex/simplexmodel.h>

SimplexModel::SimplexModel(const Model & model):
    m_model(model)
{
    m_rhs.setSparsityRatio(DENSE);
    makeComputationalForm();
}

void SimplexModel::makeComputationalForm()
{

    unsigned int i;

    const Matrix & matrix = getMatrix();
    const std::vector<Constraint> & constraints = getConstraints();

    const unsigned int rowCount = matrix.rowCount();

//    const unsigned int columnCount = matrix.columnCount();
//    const unsigned int variablesSize = rowCount + columnCount;

//    variables.resize(variablesSize);

    m_rhs.reInit(rowCount);
    //TODO HA ez megjavul a sparsityratio itt nem kell
    m_rhs.setSparsityRatio(DENSE);
    m_logicalVariables.resize(rowCount);

    for (i=0; i < rowCount; i++) {
        m_logicalVariables[i].setName(constraints.at(i).getName());
        switch(constraints.at(i).getType())
        {
        case Constraint::LESS_OR_EQUAL: {
            m_logicalVariables[i].setLowerBound(0.);
            m_logicalVariables[i].setUpperBound(Numerical::Infinity);
            m_rhs.set(i, constraints.at(i).getUpperBound());
            break;
        }

        case Constraint::GREATER_OR_EQUAL: {
            m_logicalVariables[i].setLowerBound(-Numerical::Infinity);
            m_logicalVariables[i].setUpperBound(0.);
            m_rhs.set(i, constraints.at(i).getLowerBound());
            break;
        }

        case Constraint::RANGE: {
            m_logicalVariables[i].setLowerBound(0.);
            m_logicalVariables[i].setUpperBound(constraints.at(i).getUpperBound() - constraints.at(i).getLowerBound());
            m_rhs.set(i, constraints.at(i).getUpperBound());
            break;
        }

        case Constraint::NON_BINDING: {
            m_logicalVariables[i].setLowerBound(-Numerical::Infinity);
            m_logicalVariables[i].setUpperBound(Numerical::Infinity);
            m_rhs.set(i, 0.);
            Numerical::Double temp = constraints.at(i).getUpperBound();
            Numerical::Double temp2 = constraints.at(i).getLowerBound();
            if ( temp != -Numerical::Infinity) {
                m_rhs.set(i, temp);
            } else if ( temp2 != Numerical::Infinity) {
                m_rhs.set(i, temp2);
            } else {
                m_rhs.set(i, 0.);
            }
            break;
        }

        case Constraint::EQUALITY: {
            m_logicalVariables[i].setLowerBound(0.);
            m_logicalVariables[i].setUpperBound(0.);
            m_rhs.set(i, constraints.at(i).getUpperBound());
            break;
        }

        default: {
            LPWARNING("Unhandled case");
            break;
        }
        }
    }
}

void SimplexModel::print(std::ostream& out) const
{
    out << (getObjectiveType()==MINIMIZE?"min":"max");
    Vector::Iterator it = getCostVector().begin();
    Vector::Iterator end = getCostVector().end();
    for(unsigned int i=0; it<end; i++, it++){
        out << (i==0?" ":"+ ") << *it << "*x" << i << " \n";
    }
    out << "\n s.t.: \n";
    for (unsigned int i = 0; i<getMatrix().rowCount();i++){
        const Vector & row=getMatrix().row(i);
        Vector::Iterator it = row.begin();
        Vector::Iterator end = row.end();
        for(unsigned int j=0; it<end; j++, it++){
            out << (j==0?" ":"+ ") << *it << "*x" << j << " ";
        }
        out << " + y" << i << " = " << m_rhs.at(i) <<"\n";
    }
    out << "\n";
    for(unsigned int i = 0; i<getStructuralVariables().size();i++){
        const Variable & variable = getVariable(i);
        out << variable.getLowerBound() << " x" << i << " " << variable.getUpperBound()<<"\n";
    }
    for(unsigned int i = 0; i<getLogicalVariables().size();i++){
        const Variable & variable = m_logicalVariables[i];
        out << variable.getLowerBound() << " y" << i << " " << variable.getUpperBound()<<"\n";
    }
}
