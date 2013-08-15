/**
 * @file simplexmodel.cpp
 */

#include <simplex/simplexmodel.h>

SimplexModel::SimplexModel(const Model & model):
    m_model(model)
{

}

void SimplexModel::makeComputationalForm()
{
    register unsigned int i, j;

    const Matrix & matrix = getMatrix();
    const std::vector<Constraint> & constraints = getConstraints();

    const unsigned int rowCount = matrix.rowCount();
//    const unsigned int columnCount = matrix.columnCount();
//    const unsigned int variablesSize = rowCount + columnCount;

//    variables.resize(variablesSize);

    m_rhs.reInit(rowCount, 10);

    for (i=0, j=0; i < rowCount; i++, j++) {

        switch(constraints.at(i).getType())
        {
        case Constraint::LESS_OR_EQUAL: {
            m_logicalVariables[j].setLowerBound(0.);
            m_logicalVariables[j].setUpperBound(PInfinity);
            m_rhs.set(i, constraints.at(i).getUpperBound());
            break;
        }

        case Constraint::GREATER_OR_EQUAL: {
            m_logicalVariables[j].setLowerBound(-PInfinity);
            m_logicalVariables[j].setUpperBound(0.);
            m_rhs.set(i, constraints.at(i).getLowerBound());
            break;
        }

        case Constraint::RANGE: {
            m_logicalVariables[j].setLowerBound(0.);
            m_logicalVariables[j].setUpperBound(constraints.at(i).getUpperBound() - constraints.at(i).getLowerBound());
            m_rhs.set(i, constraints.at(i).getUpperBound());
            break;
        }

        case Constraint::NON_BINDING: {
            m_logicalVariables[j].setLowerBound(-PInfinity);
            m_logicalVariables[j].setUpperBound(PInfinity);
            // TODO Nonbinding constraint value
            m_rhs.set(i, 0.);
            Numerical::Double temp = constraints.at(i).getUpperBound();
            Numerical::Double temp2 = constraints.at(i).getLowerBound();
            if ( temp != -PInfinity) {
                m_rhs.set(i, temp);
            } else if ( temp2 != PInfinity) {
                m_rhs.set(i, temp2);
            } else {
                m_rhs.set(i, 0.);
            }
            break;
        }

        case Constraint::EQUALITY: {
            m_logicalVariables[j].setLowerBound(0.);
            m_logicalVariables[j].setUpperBound(0.);
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

void SimplexModel::print(std::ostream out) const
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
        out << " + y" << i << " = " << m_rhs[i] <<"\n";
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
