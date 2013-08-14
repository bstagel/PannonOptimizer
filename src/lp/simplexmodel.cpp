/**
 * @file simplexmodel.cpp
 */

#include <lp/simplexmodel.h>

SimplexModel::SimplexModel()
{

}

void SimplexModel::makeComputationalForm()
{
    register unsigned int i, j;

    const unsigned int rowCount = m_matrix.rowCount();
    const unsigned int columnCount = m_matrix.columnCount();
    const unsigned int variablesSize = rowCount + columnCount;

    m_variables.resize(variablesSize);

    m_rhs.reInit(rowCount, 10);

    for (i=0, j=columnCount; i < rowCount; i++, j++) {

        switch(m_constraints.at(i).getType())
        {
        case Constraint::LESS_OR_EQUAL: {
            m_variables[j].setLowerBound(0.);
            m_variables[j].setUpperBound(PInfinity);
            m_rhs.set(i, m_constraints.at(i).getUpperBound());
            break;
        }

        case Constraint::GREATER_OR_EQUAL: {
            m_variables[j].setLowerBound(-PInfinity);
            m_variables[j].setUpperBound(0.);
            m_rhs.set(i, m_constraints.at(i).getLowerBound());
            break;
        }

        case Constraint::RANGE: {
            m_variables[j].setLowerBound(0.);
            m_variables[j].setUpperBound(m_constraints.at(i).getUpperBound() - m_constraints.at(i).getLowerBound());
            m_rhs.set(i, m_constraints.at(i).getUpperBound());
            break;
        }

        case Constraint::NON_BINDING: {
            m_variables[j].setLowerBound(-PInfinity);
            m_variables[j].setUpperBound(PInfinity);
            // TODO Nonbinding constraint value
            m_rhs.set(i, 0.);
            Numerical::Double temp = m_constraints.at(i).getUpperBound();
            Numerical::Double temp2 = m_constraints.at(i).getLowerBound();
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
            m_variables[j].setLowerBound(0.);
            m_variables[j].setUpperBound(0.);
            m_rhs.set(i, m_constraints.at(i).getUpperBound());
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
    out << (m_objectiveType==MINIMIZE?"min":"max");
    Vector::ConstIterator it = m_costVector.begin();
    Vector::ConstIterator end = m_costVector.end();
    for(unsigned int i=0; it<end; i++, it++){
        out << (i==0?" ":"+ ") << *it << "*x" << i << " \n";
    }
    out << "\n s.t.: \n";
    for (unsigned int i = 0; i<m_matrix.rowCount();i++){
        const Vector & row=m_matrix.row(i);
        Vector::ConstIterator it = row.begin();
        Vector::ConstIterator end = row.end();
        for(unsigned int j=0; it<end; j++, it++){
            out << (j==0?" ":"+ ") << *it << "*x" << j << " ";
        }
        out << " + y" << i << " = " << m_rhs[i] <<"\n";
    }
    out << "\n";
    for(unsigned int i = 0; i<m_variables.size();i++){
        const Variable & variable = m_variables[i];
        if(i<m_matrix.rowCount()){
            out << variable.getLowerBound() << " x" << i << " " << variable.getUpperBound();
        } else {
            out << variable.getLowerBound() << " y" << i << " " << variable.getUpperBound();
        }
    }
}
